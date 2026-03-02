#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "content/handlers/javascript/quickjs/qjs_utils.h"
#include "utils/log.h"
#include "utils/corestrings.h"
#include "utils/ring.h"
#include "netsurf/misc.h"
#include "desktop/gui_internal.h"

/* Timer tracking */
typedef struct qjsky_timer_s {
	JSContext *ctx;
	JSValue func;
	bool repeating;
	int ms;
	int handle;
	struct qjsky_timer_s *r_next;
	struct qjsky_timer_s *r_prev;
} qjsky_timer_t;

static JSClassID qjsky_event_class_id = 0;

static void qjsky_node_finalizer(JSRuntime *rt, JSValue val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(rt);
	struct dom_node *node = JS_GetOpaque(val, heap->node_class_id);
	if (node) {
		dom_node_unref(node);
	}
}

static void qjsky_event_finalizer(JSRuntime *rt, JSValue val)
{
	struct dom_event *evt = JS_GetOpaque(val, qjsky_event_class_id);
	if (evt) {
		dom_event_unref(evt);
	}
}

static JSClassDef qjsky_node_class = {
	"Node",
	.finalizer = qjsky_node_finalizer,
};

static JSClassDef qjsky_event_class = {
	"Event",
	.finalizer = qjsky_event_finalizer,
};

void qjsky_init_runtime(struct jsheap *heap)
{
	JS_NewClassID(&heap->node_class_id);
	JS_NewClass(heap->rt, heap->node_class_id, &qjsky_node_class);

	if (qjsky_event_class_id == 0) {
		JS_NewClassID(&qjsky_event_class_id);
	}
	JS_NewClass(heap->rt, qjsky_event_class_id, &qjsky_event_class);

	JS_SetRuntimeOpaque(heap->rt, heap);
}

void qjsky_init_context(JSContext *ctx)
{
	JSRuntime *rt = JS_GetRuntime(ctx);
	struct jsheap *heap = JS_GetRuntimeOpaque(rt);
	JSValue global = JS_GetGlobalObject(ctx);

	/* Ensure support for necessary intrinsics */
	JS_AddIntrinsicMapSet(ctx);
	JS_AddIntrinsicBigInt(ctx);

	/* Initialize atoms */
	if (heap->node_map_atom == JS_ATOM_NULL) {
		heap->node_map_atom = JS_NewAtom(ctx, "__qjskyNodeMap");
		heap->handler_map_atom = JS_NewAtom(ctx, "__qjskyHandlerMap");
		heap->handler_listener_map_atom = JS_NewAtom(ctx, "__qjskyHandlerListenerMap");
	}

	JSValue map_ctor = JS_GetPropertyStr(ctx, global, "Map");
	JSValue map = JS_CallConstructor(ctx, map_ctor, 0, NULL);
	JS_FreeValue(ctx, map_ctor);

	JS_DefinePropertyValue(ctx, global, heap->node_map_atom, map, JS_PROP_CONFIGURABLE);
	JS_FreeValue(ctx, global);
}

JSValue qjsky_push_node(JSContext *ctx, struct dom_node *node)
{
	if (!node) return JS_NULL;

	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue global = JS_GetGlobalObject(ctx);
	JSValue map = JS_GetProperty(ctx, global, heap->node_map_atom);

	/* Key for the map: use BigUint64 for pointer precision */
	JSValue key = JS_NewBigUint64(ctx, (uint64_t)(uintptr_t)node);

	JSValue get_fn = JS_GetPropertyStr(ctx, map, "get");
	JSValue existing = JS_Call(ctx, get_fn, map, 1, &key);
	JS_FreeValue(ctx, get_fn);

	if (!JS_IsUndefined(existing) && !JS_IsNull(existing)) {
		JS_FreeValue(ctx, key);
		JS_FreeValue(ctx, map);
		JS_FreeValue(ctx, global);
		return existing;
	}
	JS_FreeValue(ctx, existing);

	/* Create object with proper class */
	JSValue obj = JS_NewObjectClass(ctx, heap->node_class_id);
	if (JS_IsException(obj)) {
		JS_FreeValue(ctx, key);
		JS_FreeValue(ctx, map);
		JS_FreeValue(ctx, global);
		return obj;
	}

	JS_SetOpaque(obj, node);
	dom_node_ref(node);

	/* Attach private maps for handlers and listeners */
	JS_DefinePropertyValue(ctx, obj, heap->handler_map_atom, JS_NewObject(ctx), 0);
	JS_DefinePropertyValue(ctx, obj, heap->handler_listener_map_atom, JS_NewObject(ctx), 0);

	/* Store in memoization map */
	JSValue set_fn = JS_GetPropertyStr(ctx, map, "set");
	JSValue args[2] = { key, JS_DupValue(ctx, obj) };
	JSValue ret = JS_Call(ctx, set_fn, map, 2, args);
	JS_FreeValue(ctx, ret);
	JS_FreeValue(ctx, args[1]); /* Fix leak from review */
	JS_FreeValue(ctx, set_fn);
	JS_FreeValue(ctx, key);

	JS_FreeValue(ctx, map);
	JS_FreeValue(ctx, global);

	return obj;
}

struct dom_node *qjsky_get_node(JSContext *ctx, JSValue val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	return JS_GetOpaque(val, heap->node_class_id);
}

/* String Conversion Helpers */

dom_string *qjsky_js_value_to_dom_string(JSContext *ctx, JSValue val)
{
	const char *str = JS_ToCString(ctx, val);
	dom_string *ret = NULL;
	if (str) {
		dom_string_create((const uint8_t *)str, strlen(str), &ret);
		JS_FreeCString(ctx, str);
	}
	return ret;
}

JSValue qjsky_dom_string_to_js_value(JSContext *ctx, dom_string *str)
{
	if (!str) return JS_NULL;
	return JS_NewStringLen(ctx, (const char *)dom_string_data(str), dom_string_byte_length(str));
}

/* Timer Support */

static void qjsky_timer_cb(void *p)
{
	qjsky_timer_t *timer = p;
	JSValue ret = JS_Call(timer->ctx, timer->func, JS_UNDEFINED, 0, NULL);
	if (JS_IsException(ret)) qjs_log_exception(timer->ctx, "JS Timer Error");
	JS_FreeValue(timer->ctx, ret);

	/* Use context to run pending jobs (Promises) after timer callback */
	JSContext *ctx1;
	while (JS_ExecutePendingJob(JS_GetRuntime(timer->ctx), &ctx1) > 0);

	if (timer->repeating) {
		guit->misc->schedule(timer->ms, qjsky_timer_cb, timer);
	} else {
		struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(timer->ctx));
		qjsky_timer_t *timer_ring = (qjsky_timer_t *)heap->timer_ring;
		RING_REMOVE(timer_ring, timer);
		heap->timer_ring = timer_ring;
		JS_FreeValue(timer->ctx, timer->func);
		free(timer);
	}
}

static JSValue qjsky_set_timer(JSContext *ctx, int argc, JSValueConst *argv, bool repeating)
{
	if (argc < 2) return JS_EXCEPTION;

	qjsky_timer_t *timer = malloc(sizeof(*timer));
	if (!timer) return JS_EXCEPTION;

	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));

	timer->ctx = ctx;
	timer->func = JS_DupValue(ctx, argv[0]);
	JS_ToInt32(ctx, &timer->ms, argv[1]);
	timer->repeating = repeating;
	timer->handle = heap->next_timer_handle++;

	qjsky_timer_t *timer_ring = (qjsky_timer_t *)heap->timer_ring;
	RING_INSERT(timer_ring, timer);
	heap->timer_ring = timer_ring;
	guit->misc->schedule(timer->ms, qjsky_timer_cb, timer);

	return JS_NewInt32(ctx, timer->handle);
}

static JSValue qjsky_setTimeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return qjsky_set_timer(ctx, argc, argv, false);
}

static JSValue qjsky_setInterval(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return qjsky_set_timer(ctx, argc, argv, true);
}

void qjsky_timer_init(JSContext *ctx)
{
	JSValue global = JS_GetGlobalObject(ctx);
	JS_SetPropertyStr(ctx, global, "setTimeout", JS_NewCFunction(ctx, qjsky_setTimeout, "setTimeout", 2));
	JS_SetPropertyStr(ctx, global, "setInterval", JS_NewCFunction(ctx, qjsky_setInterval, "setInterval", 2));
	JS_FreeValue(ctx, global);
}

void qjsky_timer_cleanup(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_timer_t *timer_ring = (qjsky_timer_t *)heap->timer_ring;
	qjsky_timer_t *timer;
	qjsky_timer_t *next;
	bool again;

	do {
		again = false;
		timer = timer_ring;
		if (timer == NULL) break;

		do {
			next = timer->r_next;
			if (timer->ctx == ctx) {
				guit->misc->schedule(-1, qjsky_timer_cb, timer);
				JS_FreeValue(ctx, timer->func);
				RING_REMOVE(timer_ring, timer);
				free(timer);
				again = true;
				break;
			}
			timer = next;
		} while (timer != timer_ring);
	} while (again);
	heap->timer_ring = timer_ring;
}

/* Console Integration */

static void qjsky_log_at_level(JSContext *ctx, int argc, JSValueConst *argv, nslog_level level)
{
	for (int i = 0; i < argc; i++) {
		const char *str = JS_ToCString(ctx, argv[i]);
		if (str) {
			/* NetSurf NSLOG expects a literal for the level, so we dispatch based on our own switch */
			switch(level) {
				case NSLOG_LEVEL_WARNING: NSLOG(jserrors, WARNING, "%s", str); break;
				case NSLOG_LEVEL_ERROR: NSLOG(jserrors, ERROR, "%s", str); break;
				case NSLOG_LEVEL_INFO:
				default: NSLOG(jserrors, INFO, "%s", str); break;
			}
			JS_FreeCString(ctx, str);
		}
	}
}

static JSValue qjsky_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	qjsky_log_at_level(ctx, argc, argv, NSLOG_LEVEL_INFO);
	return JS_UNDEFINED;
}

static JSValue qjsky_console_warn(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	qjsky_log_at_level(ctx, argc, argv, NSLOG_LEVEL_WARNING);
	return JS_UNDEFINED;
}

static JSValue qjsky_console_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	qjsky_log_at_level(ctx, argc, argv, NSLOG_LEVEL_ERROR);
	return JS_UNDEFINED;
}

void qjsky_init_console(JSContext *ctx)
{
	static const JSCFunctionListEntry qjsky_console_funcs[] = {
		JS_CFUNC_DEF("log", 1, qjsky_console_log),
		JS_CFUNC_DEF("info", 1, qjsky_console_log),
		JS_CFUNC_DEF("warn", 1, qjsky_console_warn),
		JS_CFUNC_DEF("error", 1, qjsky_console_error),
	};
	JSValue global = JS_GetGlobalObject(ctx);
	JSValue console = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, console, qjsky_console_funcs, sizeof(qjsky_console_funcs)/sizeof(qjsky_console_funcs[0]));
	JS_SetPropertyStr(ctx, global, "console", console);
	JS_FreeValue(ctx, global);
}

/* Window.alert */

static JSValue qjsky_window_alert(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	const char *msg = "";
	bool free_msg = false;
	if (argc > 0) {
		msg = JS_ToCString(ctx, argv[0]);
		if (msg == NULL) {
			msg = "[Error converting message]";
		} else {
			free_msg = true;
		}
	}

	NSLOG(netsurf, INFO, "JS ALERT: %s", msg);

	if (free_msg) {
		JS_FreeCString(ctx, msg);
	}

	return JS_UNDEFINED;
}

/* Event Support */

static JSValue qjsky_event_get_type(JSContext *ctx, JSValueConst this_val)
{
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, qjsky_event_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_string *type;
	dom_event_get_type(evt, &type);
	JSValue val = qjsky_dom_string_to_js_value(ctx, type);
	dom_string_unref(type);
	return val;
}

static JSValue qjsky_event_get_target(JSContext *ctx, JSValueConst this_val)
{
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, qjsky_event_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_event_target *targ;
	dom_event_get_target(evt, &targ);
	JSValue val = qjsky_push_node(ctx, (struct dom_node *)targ);
	dom_node_unref((struct dom_node *)targ);
	return val;
}

static JSValue qjsky_event_get_currentTarget(JSContext *ctx, JSValueConst this_val)
{
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, qjsky_event_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_event_target *targ;
	dom_event_get_current_target(evt, &targ);
	JSValue val = qjsky_push_node(ctx, (struct dom_node *)targ);
	dom_node_unref((struct dom_node *)targ);
	return val;
}

static JSValue qjsky_event_get_eventPhase(JSContext *ctx, JSValueConst this_val)
{
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, qjsky_event_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_event_flow_phase phase;
	dom_event_get_event_phase(evt, &phase);
	return JS_NewInt32(ctx, phase);
}

static JSValue qjsky_event_get_bubbles(JSContext *ctx, JSValueConst this_val)
{
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, qjsky_event_class_id);
	if (!evt) return JS_EXCEPTION;
	bool bubbles;
	dom_event_get_bubbles(evt, &bubbles);
	return JS_NewBool(ctx, bubbles);
}

static JSValue qjsky_event_get_cancelable(JSContext *ctx, JSValueConst this_val)
{
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, qjsky_event_class_id);
	if (!evt) return JS_EXCEPTION;
	bool cancelable;
	dom_event_get_cancelable(evt, &cancelable);
	return JS_NewBool(ctx, cancelable);
}

static const JSCFunctionListEntry qjsky_event_proto_funcs[] = {
	JS_CGETSET_DEF("type", qjsky_event_get_type, NULL),
	JS_CGETSET_DEF("target", qjsky_event_get_target, NULL),
	JS_CGETSET_DEF("currentTarget", qjsky_event_get_currentTarget, NULL),
	JS_CGETSET_DEF("eventPhase", qjsky_event_get_eventPhase, NULL),
	JS_CGETSET_DEF("bubbles", qjsky_event_get_bubbles, NULL),
	JS_CGETSET_DEF("cancelable", qjsky_event_get_cancelable, NULL),
};

static JSValue qjsky_get_handler(JSContext *ctx, struct dom_element *ele, dom_string *name)
{
	JSValue node_val = qjsky_push_node(ctx, (struct dom_node *)ele);
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue handler_map = JS_GetProperty(ctx, node_val, heap->handler_map_atom);

	JSAtom prop = JS_NewAtomLen(ctx, (const char *)dom_string_data(name), dom_string_byte_length(name));
	JSValue handler = JS_GetProperty(ctx, handler_map, prop);
	JS_FreeAtom(ctx, prop);

	JS_FreeValue(ctx, handler_map);
	JS_FreeValue(ctx, node_val);
	return handler;
}

static void qjsky_generic_event_handler(dom_event *evt, void *pw)
{
	JSContext *ctx = (JSContext *)pw;
	dom_string *name;
	dom_exception exc;
	dom_event_target *targ;

	exc = dom_event_get_type(evt, &name);
	if (exc != DOM_NO_ERR) return;

	exc = dom_event_get_current_target(evt, &targ);
	if (exc != DOM_NO_ERR) {
		dom_string_unref(name);
		return;
	}

	JSValue handler = qjsky_get_handler(ctx, (struct dom_element *)targ, name);
	if (JS_IsFunction(ctx, handler)) {
		JSValue node_val = qjsky_push_node(ctx, (struct dom_node *)targ);
		JSValue event_val = qjsky_push_event(ctx, evt);
		JSValue ret = JS_Call(ctx, handler, node_val, 1, &event_val);
		if (JS_IsException(ret)) qjs_log_exception(ctx, "Event handler error");

		if (JS_IsBool(ret) && !JS_ToBool(ctx, ret)) {
			dom_event_prevent_default(evt);
		}

		JS_FreeValue(ctx, ret);
		JS_FreeValue(ctx, event_val);
		JS_FreeValue(ctx, node_val);

		/* Run pending jobs (Promises) after event handler */
		JSContext *ctx1;
		while (JS_ExecutePendingJob(JS_GetRuntime(ctx), &ctx1) > 0);
	}
	JS_FreeValue(ctx, handler);

	dom_node_unref((struct dom_node *)targ);
	dom_string_unref(name);
}

void qjsky_register_event_listener_for(JSContext *ctx, struct dom_element *ele, dom_string *name, dom_string *value, bool capture)
{
	dom_event_listener *listen = NULL;
	dom_exception exc;

	if (value != NULL) {
		JSValue node_val = qjsky_push_node(ctx, (struct dom_node *)ele);
		struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
		JSValue handler_map = JS_GetProperty(ctx, node_val, heap->handler_map_atom);

		/* Wrap attribute code in a function expression: (function(event){ ... }) */
		size_t vlen = dom_string_byte_length(value);
		const char *vdata = (const char *)dom_string_data(value);
		const char *prefix = "(function(event){";
		const char *suffix = "})";
		size_t code_len = strlen(prefix) + vlen + strlen(suffix);
		char *code = malloc(code_len + 1);
		if (code) {
			sprintf(code, "%s%.*s%s", prefix, (int)vlen, vdata, suffix);
			JSValue func = JS_Eval(ctx, code, code_len, "inline-handler", JS_EVAL_TYPE_GLOBAL);
			if (!JS_IsException(func)) {
				JSAtom prop = JS_NewAtomLen(ctx, (const char *)dom_string_data(name), dom_string_byte_length(name));
				JS_SetProperty(ctx, handler_map, prop, func);
				JS_FreeAtom(ctx, prop);
			} else {
				qjs_log_exception(ctx, "Failed to compile inline handler");
			}
			free(code);
		}
		JS_FreeValue(ctx, handler_map);
		JS_FreeValue(ctx, node_val);
	}

	exc = dom_event_listener_create(qjsky_generic_event_handler, ctx, &listen);
	if (exc != DOM_NO_ERR) return;

	dom_event_target_add_event_listener(ele, name, listen, capture);
	dom_event_listener_unref(listen);
}

JSValue qjsky_push_event(JSContext *ctx, dom_event *evt)
{
	JSValue obj = JS_NewObjectClass(ctx, qjsky_event_class_id);
	if (JS_IsException(obj)) return obj;

	JS_SetOpaque(obj, evt);
	dom_event_ref(evt);

	/* Set prototype if it hasn't been set yet */
	JSValue global = JS_GetGlobalObject(ctx);
	JSValue event_proto = JS_GetPropertyStr(ctx, global, "__qjskyEventProto");
	if (JS_IsUndefined(event_proto)) {
		JS_FreeValue(ctx, event_proto);
		event_proto = JS_NewObject(ctx);
		JS_SetPropertyFunctionList(ctx, event_proto, qjsky_event_proto_funcs, sizeof(qjsky_event_proto_funcs)/sizeof(qjsky_event_proto_funcs[0]));
		JS_SetPropertyStr(ctx, global, "__qjskyEventProto", JS_DupValue(ctx, event_proto));
	}
	JS_SetPrototype(ctx, obj, event_proto);
	JS_FreeValue(ctx, event_proto);
	JS_FreeValue(ctx, global);

	return obj;
}

void qjsky_init_window(JSContext *ctx)
{
	JSValue global = JS_GetGlobalObject(ctx);
	JS_SetPropertyStr(ctx, global, "alert", JS_NewCFunction(ctx, qjsky_window_alert, "alert", 1));
	JS_FreeValue(ctx, global);
}
