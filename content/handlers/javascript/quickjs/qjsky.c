#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "content/handlers/javascript/quickjs/qjs_utils.h"
#include "utils/log.h"
#include "utils/corestrings.h"
#include "utils/ring.h"
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

static void qjsky_node_finalizer(JSRuntime *rt, JSValue val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(rt);
	struct dom_node *node = JS_GetOpaque(val, heap->node_class_id);
	if (node) {
		dom_node_unref(node);
	}
}

static JSClassDef qjsky_node_class = {
	"Node",
	.finalizer = qjsky_node_finalizer,
};

void qjsky_init_runtime(struct jsheap *heap)
{
	JS_NewClassID(&heap->node_class_id);
	JS_NewClass(heap->rt, heap->node_class_id, &qjsky_node_class);
	JS_SetRuntimeOpaque(heap->rt, heap);
}

void qjsky_init_context(JSContext *ctx)
{
	JSRuntime *rt = JS_GetRuntime(ctx);
	struct jsheap *heap = JS_GetRuntimeOpaque(rt);
	JSValue global = JS_GetGlobalObject(ctx);
	JSValue map = JS_NewMap(ctx);

	/* Use a Symbol for better isolation of the memoization map */
	if (heap->node_map_atom == JS_ATOM_NULL) {
		JSValue sym = JS_NewSymbol(ctx, "__qjskyNodeMap", 0);
		heap->node_map_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
	}

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

	/* Call map.get(key) */
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

	/* Store in memoization map */
	JSValue set_fn = JS_GetPropertyStr(ctx, map, "set");
	JSValue argv[2] = { key, JS_DupValue(ctx, obj) };
	JSValue set_ret = JS_Call(ctx, set_fn, map, 2, argv);
	JS_FreeValue(ctx, argv[1]);
	JS_FreeValue(ctx, set_fn);
	JS_FreeValue(ctx, set_ret);
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

	if (timer->repeating) {
		guit->misc->schedule(timer->ms, qjsky_timer_cb, timer);
	} else {
		struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(timer->ctx));
		qjsky_timer_t **timer_ring = (qjsky_timer_t **)&heap->timer_ring;
		RING_REMOVE(*timer_ring, timer);
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

	qjsky_timer_t **timer_ring = (qjsky_timer_t **)&heap->timer_ring;
	RING_INSERT(*timer_ring, timer);
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
	qjsky_timer_t **timer_ring = (qjsky_timer_t **)&heap->timer_ring;
	qjsky_timer_t *timer;
	qjsky_timer_t *next;
	bool again;

	do {
		again = false;
		timer = *timer_ring;
		if (timer == NULL) break;

		do {
			next = timer->r_next;
			if (timer->ctx == ctx) {
				guit->misc->schedule(-1, qjsky_timer_cb, timer);
				JS_FreeValue(ctx, timer->func);
				RING_REMOVE(*timer_ring, timer);
				free(timer);
				again = true;
				break;
			}
			timer = next;
		} while (timer != *timer_ring);
	} while (again);
}

/* Console Integration */

static JSValue qjsky_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	for (int i = 0; i < argc; i++) {
		const char *str = JS_ToCString(ctx, argv[i]);
		if (str) {
			NSLOG(jserrors, INFO, "%s", str);
			JS_FreeCString(ctx, str);
		}
	}
	return JS_UNDEFINED;
}

void qjsky_init_console(JSContext *ctx)
{
	static const JSCFunctionListEntry qjsky_console_funcs[] = {
		JS_CFUNC_DEF("log", 1, qjsky_console_log),
		JS_CFUNC_DEF("info", 1, qjsky_console_log),
		JS_CFUNC_DEF("warn", 1, qjsky_console_log),
		JS_CFUNC_DEF("error", 1, qjsky_console_log),
	};
	JSValue global = JS_GetGlobalObject(ctx);
	JSValue console = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, console, qjsky_console_funcs, sizeof(qjsky_console_funcs)/sizeof(qjsky_console_funcs[0]));
	JS_SetPropertyStr(ctx, global, "console", console);
	JS_FreeValue(ctx, global);
}
