#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <nsutils/base64.h>
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "content/handlers/javascript/quickjs/qjs_utils.h"
#include "utils/log.h"
#include "utils/corestrings.h"
#include "utils/ring.h"
#include "netsurf/misc.h"
#include "desktop/gui_internal.h"
#include "desktop/browser_private.h"
#include "desktop/scrollbar.h"

/* Forward declarations */
static void qjsky_node_finalizer(JSRuntime *rt, JSValue val);
static void qjsky_event_finalizer(JSRuntime *rt, JSValue val);
static void qjsky_uievent_finalizer(JSRuntime *rt, JSValue val);
static void qjsky_mouseevent_finalizer(JSRuntime *rt, JSValue val);
static void qjsky_keyboardevent_finalizer(JSRuntime *rt, JSValue val);
static JSValue qjsky_event_ctor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv);

/* Class Definitions */
static JSClassDef qjsky_node_class = {
	"Node",
	.finalizer = qjsky_node_finalizer,
};

static JSClassDef qjsky_event_class = {
	"Event",
	.finalizer = qjsky_event_finalizer,
};

static JSClassDef qjsky_uievent_class = {
	"UIEvent",
	.finalizer = qjsky_uievent_finalizer,
};

static JSClassDef qjsky_mouseevent_class = {
	"MouseEvent",
	.finalizer = qjsky_mouseevent_finalizer,
};

static JSClassDef qjsky_keyboardevent_class = {
	"KeyboardEvent",
	.finalizer = qjsky_keyboardevent_finalizer,
};

extern JSClassDef qjsky_xhr_class;
extern JSClassDef qjsky_location_class;
extern JSClassDef qjsky_history_class;
extern JSClassDef qjsky_navigator_class;
extern JSClassDef qjsky_screen_class;
extern JSClassDef qjsky_barprop_class;
extern JSClassDef qjsky_url_class;
extern JSClassDef qjsky_urlsearchparams_class;

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

static void qjsky_timer_cb(void *p)
{
	qjsky_timer_t *timer = (qjsky_timer_t *)p;
	JSContext *ctx = timer->ctx;
	int handle = timer->handle;
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));

	JSValue ret = JS_Call(ctx, timer->func, JS_UNDEFINED, 0, NULL);
	if (JS_IsException(ret)) qjs_log_exception(ctx, "JS Timer Error");
	JS_FreeValue(ctx, ret);

	/* Run pending jobs (Promises) after timer callback */
	qjs_run_jobs(ctx);

	/* Check if timer is still in the ring (might have been cleared by JS) */
	qjsky_timer_t *timer_ring = (qjsky_timer_t *)heap->timer_ring;
	qjsky_timer_t *curr = timer_ring;
	qjsky_timer_t *found = NULL;
	if (curr) {
		do {
			if (curr->handle == handle) {
				found = curr;
				break;
			}
			curr = curr->r_next;
		} while (curr != timer_ring);
	}

	if (found == NULL) return;

	if (found->repeating) {
		guit->misc->schedule(found->ms, qjsky_timer_cb, found);
	} else {
		RING_REMOVE(timer_ring, found);
		heap->timer_ring = timer_ring;
		JS_FreeValue(ctx, found->func);
		free(found);
	}
}

static JSValue qjsky_set_timer(JSContext *ctx, int argc, JSValueConst *argv, bool repeating)
{
	if (argc < 1) return JS_EXCEPTION;

	int32_t ms = 0;
	if (argc > 1) {
		if (JS_ToInt32(ctx, &ms, argv[1]) < 0) ms = 0;
	}
	if (ms < 0) ms = 0;

	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_timer_t *timer = malloc(sizeof(*timer));
	if (!timer) return JS_EXCEPTION;

	timer->ctx = ctx;
	timer->func = JS_DupValue(ctx, argv[0]);
	timer->repeating = repeating;
	timer->ms = ms;
	timer->handle = heap->next_timer_handle++;

	qjsky_timer_t *timer_ring = (qjsky_timer_t *)heap->timer_ring;
	RING_INSERT(timer_ring, timer);
	heap->timer_ring = timer_ring;
	guit->misc->schedule(timer->ms, qjsky_timer_cb, timer);

	return JS_NewInt32(ctx, timer->handle);
}

static JSValue qjsky_window_setTimeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return qjsky_set_timer(ctx, argc, argv, false);
}

static JSValue qjsky_window_setInterval(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return qjsky_set_timer(ctx, argc, argv, true);
}

static JSValue qjsky_window_clearTimeout(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	if (argc < 1) return JS_UNDEFINED;
	int32_t handle;
	if (JS_ToInt32(ctx, &handle, argv[0]) < 0) return JS_UNDEFINED;

	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_timer_t *timer_ring = (qjsky_timer_t *)heap->timer_ring;
	qjsky_timer_t *timer = timer_ring;
	if (timer) {
		do {
			if (timer->handle == handle) {
				guit->misc->schedule(-1, qjsky_timer_cb, timer);
				RING_REMOVE(timer_ring, timer);
				heap->timer_ring = timer_ring;
				JS_FreeValue(ctx, timer->func);
				free(timer);
				break;
			}
			timer = timer->r_next;
		} while (timer != timer_ring);
	}
	return JS_UNDEFINED;
}

void qjsky_timer_init(JSContext *ctx)
{
	JSValue global = JS_GetGlobalObject(ctx);
	JS_SetPropertyStr(ctx, global, "setTimeout", JS_NewCFunction(ctx, qjsky_window_setTimeout, "setTimeout", 2));
	JS_SetPropertyStr(ctx, global, "setInterval", JS_NewCFunction(ctx, qjsky_window_setInterval, "setInterval", 2));
	JS_SetPropertyStr(ctx, global, "clearTimeout", JS_NewCFunction(ctx, qjsky_window_clearTimeout, "clearTimeout", 1));
	JS_SetPropertyStr(ctx, global, "clearInterval", JS_NewCFunction(ctx, qjsky_window_clearTimeout, "clearInterval", 1));
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
	struct jsheap *heap = JS_GetRuntimeOpaque(rt);
	struct dom_event *evt = JS_GetOpaque(val, heap->event_class_id);
	if (evt) {
		dom_event_unref(evt);
	}
}

static void qjsky_uievent_finalizer(JSRuntime *rt, JSValue val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(rt);
	struct dom_event *evt = JS_GetOpaque(val, heap->uievent_class_id);
	if (evt) {
		dom_event_unref(evt);
	}
}

static void qjsky_mouseevent_finalizer(JSRuntime *rt, JSValue val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(rt);
	struct dom_event *evt = JS_GetOpaque(val, heap->mouseevent_class_id);
	if (evt) {
		dom_event_unref(evt);
	}
}

static void qjsky_keyboardevent_finalizer(JSRuntime *rt, JSValue val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(rt);
	struct dom_event *evt = JS_GetOpaque(val, heap->keyboardevent_class_id);
	if (evt) {
		dom_event_unref(evt);
	}
}

void qjsky_init_runtime(struct jsheap *heap)
{
	JS_NewClassID(&heap->node_class_id);
	JS_NewClass(heap->rt, heap->node_class_id, &qjsky_node_class);

	JS_NewClassID(&heap->event_class_id);
	JS_NewClass(heap->rt, heap->event_class_id, &qjsky_event_class);

	JS_NewClassID(&heap->uievent_class_id);
	JS_NewClass(heap->rt, heap->uievent_class_id, &qjsky_uievent_class);

	JS_NewClassID(&heap->mouseevent_class_id);
	JS_NewClass(heap->rt, heap->mouseevent_class_id, &qjsky_mouseevent_class);

	JS_NewClassID(&heap->keyboardevent_class_id);
	JS_NewClass(heap->rt, heap->keyboardevent_class_id, &qjsky_keyboardevent_class);

	JS_NewClassID(&heap->xhr_class_id);
	JS_NewClass(heap->rt, heap->xhr_class_id, &qjsky_xhr_class);

	JS_NewClassID(&heap->location_class_id);
	JS_NewClass(heap->rt, heap->location_class_id, &qjsky_location_class);

	JS_NewClassID(&heap->history_class_id);
	JS_NewClass(heap->rt, heap->history_class_id, &qjsky_history_class);

	JS_NewClassID(&heap->navigator_class_id);
	JS_NewClass(heap->rt, heap->navigator_class_id, &qjsky_navigator_class);

	JS_NewClassID(&heap->screen_class_id);
	JS_NewClass(heap->rt, heap->screen_class_id, &qjsky_screen_class);

	JS_NewClassID(&heap->barprop_class_id);
	JS_NewClass(heap->rt, heap->barprop_class_id, &qjsky_barprop_class);

	JS_NewClassID(&heap->url_class_id);
	JS_NewClass(heap->rt, heap->url_class_id, &qjsky_url_class);

	JS_NewClassID(&heap->urlsearchparams_class_id);
	JS_NewClass(heap->rt, heap->urlsearchparams_class_id, &qjsky_urlsearchparams_class);

	JS_SetRuntimeOpaque(heap->rt, heap);
}

static JSValue qjsky_event_get_type(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_string *type;
	dom_event_get_type(evt, &type);
	JSValue val = qjsky_dom_string_to_js_value(ctx, type);
	dom_string_unref(type);
	return val;
}

static JSValue qjsky_event_get_target(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_event_target *targ;
	dom_event_get_target(evt, &targ);
	JSValue val = qjsky_push_node(ctx, (struct dom_node *)targ);
	dom_node_unref((struct dom_node *)targ);
	return val;
}

static JSValue qjsky_event_get_currentTarget(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_event_target *targ;
	dom_event_get_current_target(evt, &targ);
	JSValue val = qjsky_push_node(ctx, (struct dom_node *)targ);
	dom_node_unref((struct dom_node *)targ);
	return val;
}

static JSValue qjsky_event_get_eventPhase(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_event_flow_phase phase;
	dom_event_get_event_phase(evt, &phase);
	return JS_NewInt32(ctx, phase);
}

static JSValue qjsky_event_get_bubbles(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	bool bubbles;
	dom_event_get_bubbles(evt, &bubbles);
	return JS_NewBool(ctx, bubbles);
}

static JSValue qjsky_event_get_cancelable(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	bool cancelable;
	dom_event_get_cancelable(evt, &cancelable);
	return JS_NewBool(ctx, cancelable);
}

static JSValue qjsky_event_stopPropagation(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_event_stop_propagation(evt);
	return JS_UNDEFINED;
}

static JSValue qjsky_event_stopImmediatePropagation(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_event_stop_immediate_propagation(evt);
	return JS_UNDEFINED;
}

static JSValue qjsky_event_preventDefault(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_event_prevent_default(evt);
	return JS_UNDEFINED;
}

static JSValue qjsky_event_get_defaultPrevented(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	bool prevented;
	dom_event_is_default_prevented(evt, &prevented);
	return JS_NewBool(ctx, prevented);
}

static JSValue qjsky_event_get_isTrusted(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	bool trusted;
	dom_event_get_is_trusted(evt, &trusted);
	return JS_NewBool(ctx, trusted);
}

static JSValue qjsky_event_initEvent(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	if (argc < 1) return JS_EXCEPTION;

	dom_string *type = qjsky_js_value_to_dom_string(ctx, argv[0]);
	bool bubbles = argc > 1 ? JS_ToBool(ctx, argv[1]) : false;
	bool cancelable = argc > 2 ? JS_ToBool(ctx, argv[2]) : false;

	dom_event_init(evt, type, bubbles, cancelable);
	dom_string_unref(type);
	return JS_UNDEFINED;
}

static const JSCFunctionListEntry qjsky_event_proto_funcs[] = {
	JS_CGETSET_DEF("type", qjsky_event_get_type, NULL),
	JS_CGETSET_DEF("target", qjsky_event_get_target, NULL),
	JS_CGETSET_DEF("currentTarget", qjsky_event_get_currentTarget, NULL),
	JS_CGETSET_DEF("eventPhase", qjsky_event_get_eventPhase, NULL),
	JS_CGETSET_DEF("bubbles", qjsky_event_get_bubbles, NULL),
	JS_CGETSET_DEF("cancelable", qjsky_event_get_cancelable, NULL),
	JS_CGETSET_DEF("defaultPrevented", qjsky_event_get_defaultPrevented, NULL),
	JS_CGETSET_DEF("isTrusted", qjsky_event_get_isTrusted, NULL),
	JS_CFUNC_DEF("stopPropagation", 0, qjsky_event_stopPropagation),
	JS_CFUNC_DEF("stopImmediatePropagation", 0, qjsky_event_stopImmediatePropagation),
	JS_CFUNC_DEF("preventDefault", 0, qjsky_event_preventDefault),
	JS_CFUNC_DEF("initEvent", 3, qjsky_event_initEvent),
};

static JSValue qjsky_uievent_get_detail(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_ui_event *evt = JS_GetOpaque2(ctx, this_val, heap->uievent_class_id);
	if (!evt) return JS_EXCEPTION;
	int32_t detail;
	dom_ui_event_get_detail(evt, &detail);
	return JS_NewInt32(ctx, detail);
}

static const JSCFunctionListEntry qjsky_uievent_proto_funcs[] = {
	JS_CGETSET_DEF("detail", qjsky_uievent_get_detail, NULL),
};

static JSValue qjsky_mouseevent_get_screenX(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->mouseevent_class_id);
	if (!evt) return JS_EXCEPTION;
	int32_t x;
	dom_mouse_event_get_screen_x(evt, &x);
	return JS_NewInt32(ctx, x);
}

static JSValue qjsky_mouseevent_get_screenY(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->mouseevent_class_id);
	if (!evt) return JS_EXCEPTION;
	int32_t y;
	dom_mouse_event_get_screen_y(evt, &y);
	return JS_NewInt32(ctx, y);
}

static JSValue qjsky_mouseevent_get_clientX(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->mouseevent_class_id);
	if (!evt) return JS_EXCEPTION;
	int32_t x;
	dom_mouse_event_get_client_x(evt, &x);
	return JS_NewInt32(ctx, x);
}

static JSValue qjsky_mouseevent_get_clientY(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->mouseevent_class_id);
	if (!evt) return JS_EXCEPTION;
	int32_t y;
	dom_mouse_event_get_client_y(evt, &y);
	return JS_NewInt32(ctx, y);
}

static JSValue qjsky_mouseevent_get_ctrlKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->mouseevent_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_mouse_event_get_ctrl_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_mouseevent_get_shiftKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->mouseevent_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_mouse_event_get_shift_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_mouseevent_get_altKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->mouseevent_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_mouse_event_get_alt_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_mouseevent_get_metaKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->mouseevent_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_mouse_event_get_meta_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_mouseevent_get_button(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->mouseevent_class_id);
	if (!evt) return JS_EXCEPTION;
	unsigned short button;
	dom_mouse_event_get_button(evt, &button);
	return JS_NewInt32(ctx, button);
}

static const JSCFunctionListEntry qjsky_mouseevent_proto_funcs[] = {
	JS_CGETSET_DEF("screenX", qjsky_mouseevent_get_screenX, NULL),
	JS_CGETSET_DEF("screenY", qjsky_mouseevent_get_screenY, NULL),
	JS_CGETSET_DEF("clientX", qjsky_mouseevent_get_clientX, NULL),
	JS_CGETSET_DEF("clientY", qjsky_mouseevent_get_clientY, NULL),
	JS_CGETSET_DEF("ctrlKey", qjsky_mouseevent_get_ctrlKey, NULL),
	JS_CGETSET_DEF("shiftKey", qjsky_mouseevent_get_shiftKey, NULL),
	JS_CGETSET_DEF("altKey", qjsky_mouseevent_get_altKey, NULL),
	JS_CGETSET_DEF("metaKey", qjsky_mouseevent_get_metaKey, NULL),
	JS_CGETSET_DEF("button", qjsky_mouseevent_get_button, NULL),
};

static JSValue qjsky_keyboardevent_get_key(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->keyboardevent_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_string *key;
	dom_keyboard_event_get_key(evt, &key);
	JSValue val = qjsky_dom_string_to_js_value(ctx, key);
	dom_string_unref(key);
	return val;
}

static JSValue qjsky_keyboardevent_get_code(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->keyboardevent_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_string *code;
	dom_keyboard_event_get_code(evt, &code);
	JSValue val = qjsky_dom_string_to_js_value(ctx, code);
	dom_string_unref(code);
	return val;
}

static JSValue qjsky_keyboardevent_get_location(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->keyboardevent_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_key_location loc;
	dom_keyboard_event_get_location(evt, &loc);
	return JS_NewInt32(ctx, loc);
}

static JSValue qjsky_keyboardevent_get_ctrlKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->keyboardevent_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_keyboard_event_get_ctrl_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_keyboardevent_get_shiftKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->keyboardevent_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_keyboard_event_get_shift_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_keyboardevent_get_altKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->keyboardevent_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_keyboard_event_get_alt_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_keyboardevent_get_metaKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->keyboardevent_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_keyboard_event_get_meta_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static const JSCFunctionListEntry qjsky_keyboardevent_proto_funcs[] = {
	JS_CGETSET_DEF("key", qjsky_keyboardevent_get_key, NULL),
	JS_CGETSET_DEF("code", qjsky_keyboardevent_get_code, NULL),
	JS_CGETSET_DEF("location", qjsky_keyboardevent_get_location, NULL),
	JS_CGETSET_DEF("ctrlKey", qjsky_keyboardevent_get_ctrlKey, NULL),
	JS_CGETSET_DEF("shiftKey", qjsky_keyboardevent_get_shiftKey, NULL),
	JS_CGETSET_DEF("altKey", qjsky_keyboardevent_get_altKey, NULL),
	JS_CGETSET_DEF("metaKey", qjsky_keyboardevent_get_metaKey, NULL),
};

static JSValue qjsky_event_ctor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv)
{
	if (argc < 1) return JS_EXCEPTION;
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));

	dom_event *evt;
	if (dom_event_create(&evt) != DOM_NO_ERR) return JS_EXCEPTION;

	dom_string *type = qjsky_js_value_to_dom_string(ctx, argv[0]);
	bool bubbles = false;
	bool cancelable = false;

	if (argc > 1 && JS_IsObject(argv[1])) {
		JSValue b = JS_GetPropertyStr(ctx, argv[1], "bubbles");
		bubbles = JS_ToBool(ctx, b);
		JS_FreeValue(ctx, b);
		JSValue c = JS_GetPropertyStr(ctx, argv[1], "cancelable");
		cancelable = JS_ToBool(ctx, c);
		JS_FreeValue(ctx, c);
	}

	dom_event_init(evt, type, bubbles, cancelable);
	dom_string_unref(type);

	JSValue proto = JS_GetClassProto(ctx, heap->event_class_id);
	JSValue obj = JS_NewObjectProtoClass(ctx, proto, heap->event_class_id);
	JS_FreeValue(ctx, proto);

	if (JS_IsException(obj)) {
		dom_event_unref(evt);
		return obj;
	}
	JS_SetOpaque(obj, evt);
	return obj;
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
		JSValue sym_ctor = JS_GetPropertyStr(ctx, global, "Symbol");
		JSValue arg;
		JSValue sym;

		arg = JS_NewString(ctx, "nodeMap");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->node_map_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "handlerMap");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->handler_map_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "handlerListenerMap");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->handler_listener_map_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "eventProto");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->event_proto_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "uieventProto");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->uievent_proto_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "mouseeventProto");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->mouseevent_proto_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "keyboardeventProto");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->keyboardevent_proto_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		JS_FreeValue(ctx, sym_ctor);
	}

	JSValue map_ctor = JS_GetPropertyStr(ctx, global, "Map");
	JSValue map = JS_CallConstructor(ctx, map_ctor, 0, NULL);
	JS_FreeValue(ctx, map_ctor);

	JS_SetProperty(ctx, global, heap->node_map_atom, map);

	/* Initialize Event prototypes and inheritance */
	JSValue event_proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, event_proto, qjsky_event_proto_funcs, sizeof(qjsky_event_proto_funcs)/sizeof(qjsky_event_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->event_class_id, JS_DupValue(ctx, event_proto));

	JSValue event_ctor = JS_NewCFunction2(ctx, qjsky_event_ctor, "Event", 1, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, event_ctor, event_proto);
	JS_SetPropertyStr(ctx, global, "Event", event_ctor);

	JSValue uievent_proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, uievent_proto, qjsky_uievent_proto_funcs, sizeof(qjsky_uievent_proto_funcs)/sizeof(qjsky_uievent_proto_funcs[0]));
	JS_SetPrototype(ctx, uievent_proto, event_proto);
	JS_SetClassProto(ctx, heap->uievent_class_id, JS_DupValue(ctx, uievent_proto));

	JSValue mouseevent_proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, mouseevent_proto, qjsky_mouseevent_proto_funcs, sizeof(qjsky_mouseevent_proto_funcs)/sizeof(qjsky_mouseevent_proto_funcs[0]));
	JS_SetPrototype(ctx, mouseevent_proto, uievent_proto);
	JS_SetClassProto(ctx, heap->mouseevent_class_id, JS_DupValue(ctx, mouseevent_proto));

	JSValue keyboardevent_proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, keyboardevent_proto, qjsky_keyboardevent_proto_funcs, sizeof(qjsky_keyboardevent_proto_funcs)/sizeof(qjsky_keyboardevent_proto_funcs[0]));
	JS_SetPrototype(ctx, keyboardevent_proto, uievent_proto);
	JS_SetClassProto(ctx, heap->keyboardevent_class_id, JS_DupValue(ctx, keyboardevent_proto));

	/* Attach prototypes to global and consume references */
	JS_SetProperty(ctx, global, heap->event_proto_atom, event_proto);
	JS_SetProperty(ctx, global, heap->uievent_proto_atom, uievent_proto);
	JS_SetProperty(ctx, global, heap->mouseevent_proto_atom, mouseevent_proto);
	JS_SetProperty(ctx, global, heap->keyboardevent_proto_atom, keyboardevent_proto);

	JS_FreeValue(ctx, global);
}

JSValue qjsky_push_node(JSContext *ctx, struct dom_node *node)
{
	if (!node) return JS_NULL;

	JSRuntime *rt = JS_GetRuntime(ctx);
	struct jsheap *heap = JS_GetRuntimeOpaque(rt);
	JSValue global = JS_GetGlobalObject(ctx);
	JSValue map = JS_GetProperty(ctx, global, heap->node_map_atom);
	JS_FreeValue(ctx, global);

	/* Key for the map: use BigUint64 for pointer precision */
	JSValue key = JS_NewBigUint64(ctx, (uint64_t)(uintptr_t)node);

	JSValue get_fn = JS_GetPropertyStr(ctx, map, "get");
	JSValue existing = JS_Call(ctx, get_fn, map, 1, &key);
	JS_FreeValue(ctx, get_fn);

	if (!JS_IsUndefined(existing)) {
		JS_FreeValue(ctx, key);
		JS_FreeValue(ctx, map);
		return existing;
	}

	JSValue obj = JS_NewObjectClass(ctx, heap->node_class_id);
	if (JS_IsException(obj)) {
		JS_FreeValue(ctx, key);
		JS_FreeValue(ctx, map);
		return obj;
	}
	JS_SetOpaque(obj, node);
	dom_node_ref(node);

	/* Attach private maps for handlers and listeners */
	JS_SetProperty(ctx, obj, heap->handler_map_atom, JS_NewObject(ctx));
	JS_SetProperty(ctx, obj, heap->handler_listener_map_atom, JS_NewObject(ctx));

	/* Store in memoization map */
	JSValue set_fn = JS_GetPropertyStr(ctx, map, "set");
	JSValue args[2] = { key, JS_DupValue(ctx, obj) };
	JSValue ret = JS_Call(ctx, set_fn, map, 2, args);
	JS_FreeValue(ctx, ret);
	JS_FreeValue(ctx, args[1]);
	JS_FreeValue(ctx, set_fn);
	JS_FreeValue(ctx, key);

	JS_FreeValue(ctx, map);

	return obj;
}

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

		qjs_run_jobs(ctx);
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
	if (!evt) return JS_NULL;

	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue global = JS_GetGlobalObject(ctx);
	JSAtom proto_atom = heap->event_proto_atom;
	dom_string *type = NULL;
	dom_event_get_type(evt, &type);

	if (type != NULL) {
		if (dom_string_isequal(type, corestring_dom_click) ||
		    dom_string_isequal(type, corestring_dom_dblclick) ||
		    dom_string_isequal(type, corestring_dom_mousedown) ||
		    dom_string_isequal(type, corestring_dom_mouseup) ||
		    dom_string_isequal(type, corestring_dom_mousemove) ||
		    dom_string_isequal(type, corestring_dom_mouseover) ||
		    dom_string_isequal(type, corestring_dom_mouseout) ||
		    dom_string_isequal(type, corestring_dom_mouseenter) ||
		    dom_string_isequal(type, corestring_dom_mouseleave)) {
			proto_atom = heap->mouseevent_proto_atom;
		} else if (dom_string_isequal(type, corestring_dom_keydown) ||
			   dom_string_isequal(type, corestring_dom_keyup) ||
			   dom_string_isequal(type, corestring_dom_keypress)) {
			proto_atom = heap->keyboardevent_proto_atom;
		} else if (dom_string_isequal(type, corestring_dom_load) ||
			   dom_string_isequal(type, corestring_dom_unload) ||
			   dom_string_isequal(type, corestring_dom_abort) ||
			   dom_string_isequal(type, corestring_dom_error) ||
			   dom_string_isequal(type, corestring_dom_select) ||
			   dom_string_isequal(type, corestring_dom_resize) ||
			   dom_string_isequal(type, corestring_dom_scroll)) {
			proto_atom = heap->uievent_proto_atom;
		}
		dom_string_unref(type);
	}

	JSValue obj = JS_NewObjectClass(ctx, heap->event_class_id);
	if (JS_IsException(obj)) {
		JS_FreeValue(ctx, global);
		return obj;
	}

	JSValue proto = JS_GetProperty(ctx, global, proto_atom);
	JS_SetPrototype(ctx, obj, proto);
	JS_FreeValue(ctx, proto);
	JS_FreeValue(ctx, global);

	JS_SetOpaque(obj, evt);
	dom_event_ref(evt);

	return obj;
}

static JSValue qjsky_window_confirm(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return JS_TRUE;
}

static JSValue qjsky_window_prompt(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return JS_NULL;
}

static JSValue qjsky_window_atob(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	if (argc < 1) return JS_EXCEPTION;
	const char *str = JS_ToCString(ctx, argv[0]);
	if (!str) return JS_EXCEPTION;

	size_t len = strlen(str);
	char *clean = malloc(len + 1);
	size_t clean_len = 0;
	if (clean) {
		for (size_t i = 0; i < len; i++) {
			if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r' && str[i] != '\f') {
				clean[clean_len++] = str[i];
			}
		}
		clean[clean_len] = '\0';
	}

	uint8_t *out;
	size_t out_len;
	nsuerror res = nsu_base64_decode_alloc((const uint8_t *)(clean ? clean : str), clean ? clean_len : len, &out, &out_len);
	JS_FreeCString(ctx, str);
	if (clean) free(clean);

	if (res != NSUERROR_OK) {
		return JS_ThrowTypeError(ctx, "Invalid base64 string");
	}

	JSValue ret = JS_NewStringLen(ctx, (const char *)out, out_len);
	free(out);
	return ret;
}

static JSValue qjsky_window_btoa(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	if (argc < 1) return JS_EXCEPTION;
	const char *str = JS_ToCString(ctx, argv[0]);
	if (!str) return JS_EXCEPTION;

	size_t len = strlen(str);
	for (size_t i = 0; i < len; i++) {
		if ((unsigned char)str[i] > 0xFF) {
			JS_FreeCString(ctx, str);
			return JS_ThrowTypeError(ctx, "Input contains characters outside U+0000 to U+00FF");
		}
	}

	uint8_t *out;
	size_t out_len;
	nsuerror res = nsu_base64_encode_alloc((const uint8_t *)str, len, &out, &out_len);
	JS_FreeCString(ctx, str);

	if (res != NSUERROR_OK) {
		return JS_ThrowInternalError(ctx, "Base64 encoding failed");
	}

	JSValue ret = JS_NewStringLen(ctx, (const char *)out, out_len);
	free(out);
	return ret;
}

static JSValue qjsky_window_requestAnimationFrame(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	if (argc < 1) return JS_EXCEPTION;
	JSValue global = JS_GetGlobalObject(ctx);
	JSValue setTimeout = JS_GetPropertyStr(ctx, global, "setTimeout");
	JSValue date_ctor = JS_GetPropertyStr(ctx, global, "Date");
	JSValue now_fn = JS_GetPropertyStr(ctx, date_ctor, "now");
	JSValue timestamp = JS_Call(ctx, now_fn, date_ctor, 0, NULL);

	JSValue args[3];
	args[0] = JS_DupValue(ctx, argv[0]);
	args[1] = JS_NewInt32(ctx, 16);
	args[2] = timestamp;

	JSValue handle = JS_Call(ctx, setTimeout, global, 3, args);

	JS_FreeValue(ctx, args[0]);
	JS_FreeValue(ctx, args[1]);
	JS_FreeValue(ctx, args[2]);
	JS_FreeValue(ctx, now_fn);
	JS_FreeValue(ctx, date_ctor);
	JS_FreeValue(ctx, setTimeout);
	JS_FreeValue(ctx, global);

	return handle;
}

static JSValue qjsky_window_cancelAnimationFrame(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	if (argc < 1) return JS_UNDEFINED;
	JSValue global = JS_GetGlobalObject(ctx);
	JSValue clearTimeout = JS_GetPropertyStr(ctx, global, "clearTimeout");

	JSValue ret = JS_Call(ctx, clearTimeout, global, 1, argv);
	JS_FreeValue(ctx, ret);

	JS_FreeValue(ctx, clearTimeout);
	JS_FreeValue(ctx, global);

	return JS_UNDEFINED;
}

static JSValue qjsky_window_get_scrollX(JSContext *ctx, JSValueConst this_val)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	int sx = 0;
	if (thread && thread->win_priv) {
		struct browser_window *bw = thread->win_priv;
		if (bw->scroll_x) sx = scrollbar_get_offset(bw->scroll_x);
	}
	return JS_NewInt32(ctx, sx);
}

static JSValue qjsky_window_get_scrollY(JSContext *ctx, JSValueConst this_val)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	int sy = 0;
	if (thread && thread->win_priv) {
		struct browser_window *bw = thread->win_priv;
		if (bw->scroll_y) sy = scrollbar_get_offset(bw->scroll_y);
	}
	return JS_NewInt32(ctx, sy);
}

static JSValue qjsky_window_get_innerWidth(JSContext *ctx, JSValueConst this_val)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	int w = 0;
	if (thread && thread->win_priv) {
		w = ((struct browser_window *)thread->win_priv)->width;
	}
	return JS_NewInt32(ctx, w);
}

static JSValue qjsky_window_get_innerHeight(JSContext *ctx, JSValueConst this_val)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	int h = 0;
	if (thread && thread->win_priv) {
		h = ((struct browser_window *)thread->win_priv)->height;
	}
	return JS_NewInt32(ctx, h);
}

static JSValue qjsky_window_get_devicePixelRatio(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewFloat64(ctx, 1.0);
}

static JSValue qjsky_window_get_length(JSContext *ctx, JSValueConst this_val)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	int count = 0;
	if (thread && thread->win_priv) {
		struct browser_window *bw = thread->win_priv;
		count = bw->iframe_count;
		if (bw->browser_window_type == BROWSER_WINDOW_FRAMESET) {
			count += bw->rows * bw->cols;
		}
	}
	return JS_NewInt32(ctx, count);
}

static JSValue qjsky_window_get_top(JSContext *ctx, JSValueConst this_val)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (thread && thread->win_priv) {
		struct browser_window *top = browser_window_get_root(thread->win_priv);
		if (top == thread->win_priv) {
			return JS_GetGlobalObject(ctx);
		}
	}
	return JS_NULL;
}

static JSValue qjsky_window_get_self(JSContext *ctx, JSValueConst this_val)
{
	return JS_GetGlobalObject(ctx);
}

static JSValue qjsky_window_get_opener(JSContext *ctx, JSValueConst this_val)
{
	return JS_NULL;
}

static JSValue qjsky_window_get_parent(JSContext *ctx, JSValueConst this_val)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (thread && thread->win_priv) {
		struct browser_window *bw = thread->win_priv;
		if (bw->parent == NULL || bw->parent == bw) {
			return JS_GetGlobalObject(ctx);
		}
	}
	return JS_NULL;
}

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

void qjsky_init_window(JSContext *ctx)
{
	JSValue global = JS_GetGlobalObject(ctx);
	JSAtom atom;

	JS_SetPropertyStr(ctx, global, "alert", JS_NewCFunction(ctx, qjsky_window_alert, "alert", 1));
	JS_SetPropertyStr(ctx, global, "confirm", JS_NewCFunction(ctx, qjsky_window_confirm, "confirm", 1));
	JS_SetPropertyStr(ctx, global, "prompt", JS_NewCFunction(ctx, qjsky_window_prompt, "prompt", 2));
	JS_SetPropertyStr(ctx, global, "atob", JS_NewCFunction(ctx, qjsky_window_atob, "atob", 1));
	JS_SetPropertyStr(ctx, global, "btoa", JS_NewCFunction(ctx, qjsky_window_btoa, "btoa", 1));
	JS_SetPropertyStr(ctx, global, "requestAnimationFrame", JS_NewCFunction(ctx, qjsky_window_requestAnimationFrame, "requestAnimationFrame", 1));
	JS_SetPropertyStr(ctx, global, "cancelAnimationFrame", JS_NewCFunction(ctx, qjsky_window_cancelAnimationFrame, "cancelAnimationFrame", 1));

	atom = JS_NewAtom(ctx, "window");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_self, "get_window", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	atom = JS_NewAtom(ctx, "self");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_self, "get_self", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	atom = JS_NewAtom(ctx, "frames");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_self, "get_frames", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	atom = JS_NewAtom(ctx, "opener");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_opener, "get_opener", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	atom = JS_NewAtom(ctx, "scrollX");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_scrollX, "get_scrollX", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	atom = JS_NewAtom(ctx, "scrollY");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_scrollY, "get_scrollY", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	atom = JS_NewAtom(ctx, "pageXOffset");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_scrollX, "get_scrollX", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	atom = JS_NewAtom(ctx, "pageYOffset");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_scrollY, "get_scrollY", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	atom = JS_NewAtom(ctx, "innerWidth");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_innerWidth, "get_innerWidth", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	atom = JS_NewAtom(ctx, "innerHeight");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_innerHeight, "get_innerHeight", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	atom = JS_NewAtom(ctx, "devicePixelRatio");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_devicePixelRatio, "get_devicePixelRatio", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	atom = JS_NewAtom(ctx, "length");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_length, "get_length", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	atom = JS_NewAtom(ctx, "top");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_top, "get_top", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	atom = JS_NewAtom(ctx, "parent");
	JS_DefinePropertyGetSet(ctx, global, atom,
				JS_NewCFunction(ctx, qjsky_window_get_parent, "get_parent", 0),
				JS_UNDEFINED, 0);
	JS_FreeAtom(ctx, atom);

	JS_FreeValue(ctx, global);
}
