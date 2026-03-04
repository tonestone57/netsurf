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
#include "content/handlers/javascript/quickjs/storage.h"
#include "content/urldb.h"
#include "content/hlcache.h"
#include "content/content.h"
#include "content/handlers/html/private.h"

static void qjsky_node_finalizer(JSRuntime *rt, JSValue val);
static void qjsky_event_finalizer(JSRuntime *rt, JSValue val);
static JSValue qjsky_event_ctor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv);
static JSValue qjsky_interface_ctor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv);

static JSClassDef qjsky_node_class = { "Node", .finalizer = qjsky_node_finalizer };
static JSClassDef qjsky_event_class = { "Event", .finalizer = qjsky_event_finalizer };
static JSClassDef qjsky_dom_implementation_class = { "DOMImplementation" };

static JSValue qjsky_dom_implementation_hasFeature(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return JS_TRUE;
}

static const JSCFunctionListEntry qjsky_dom_implementation_proto_funcs[] = {
	JS_CFUNC_DEF("hasFeature", 2, qjsky_dom_implementation_hasFeature),
};

static JSValue qjsky_event_target_addEventListener(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
static JSValue qjsky_event_target_removeEventListener(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);

static const JSCFunctionListEntry qjsky_node_proto_funcs[] = {
	JS_CFUNC_DEF("addEventListener", 3, qjsky_event_target_addEventListener),
	JS_CFUNC_DEF("removeEventListener", 3, qjsky_event_target_removeEventListener),
};

static JSValue qjsky_document_get_cookie(JSContext *ctx, JSValueConst this_val);
static JSValue qjsky_document_set_cookie(JSContext *ctx, JSValueConst this_val, JSValueConst val);
static JSValue qjsky_document_get_implementation(JSContext *ctx, JSValueConst this_val);

static const JSCFunctionListEntry qjsky_document_proto_funcs[] = {
	JS_CGETSET_DEF("cookie", qjsky_document_get_cookie, qjsky_document_set_cookie),
	JS_CGETSET_DEF("implementation", qjsky_document_get_implementation, NULL),
};

static const JSCFunctionListEntry qjsky_element_proto_funcs[] = {
};

static const JSCFunctionListEntry qjsky_html_element_proto_funcs[] = {
};

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
		if (heap->node_key != NULL) {
			void *data = NULL;
			dom_node_get_user_data(node, heap->node_key, &data);
			/* Only clear if it's actually pointing to this object being finalized */
			if (data == JS_VALUE_GET_PTR(val)) {
				dom_node_set_user_data(node, heap->node_key, NULL, NULL, NULL);
			}
		}
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

dom_string *qjsky_js_value_to_dom_string(JSContext *ctx, JSValue val)
{
	const char *str = JS_ToCString(ctx, val);
	if (!str) return NULL;
	dom_string *dstr;
	dom_exception err = dom_string_create((const uint8_t *)str, strlen(str), &dstr);
	JS_FreeCString(ctx, str);
	return (err == DOM_NO_ERR) ? dstr : NULL;
}

JSValue qjsky_dom_string_to_js_value(JSContext *ctx, dom_string *str)
{
	if (!str) return JS_NULL;
	return JS_NewStringLen(ctx, (const char *)dom_string_data(str), dom_string_byte_length(str));
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

JSValue qjsky_push_node(JSContext *ctx, struct dom_node *node)
{
	if (!node) return JS_NULL;

	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue global = JS_GetGlobalObject(ctx);

	/* Check for existing wrapper in user data */
	if (heap->node_key != NULL) {
		void *data = NULL;
		dom_node_get_user_data(node, heap->node_key, &data);
		if (data != NULL) {
			JS_FreeValue(ctx, global);
			return JS_DupValue(ctx, JS_MKPTR(JS_TAG_OBJECT, data));
		}
	}

	/* Determine correct prototype */
	dom_node_type nodetype;
	dom_exception err;
	JSAtom proto_atom = heap->node_proto_atom;

	err = dom_node_get_node_type(node, &nodetype);
	if (err == DOM_NO_ERR) {
		switch (nodetype) {
		case DOM_DOCUMENT_NODE:
			proto_atom = heap->document_proto_atom;
			break;
		case DOM_ELEMENT_NODE: {
			dom_string *namespace;
			err = dom_node_get_namespace(node, &namespace);
			if (err == DOM_NO_ERR && namespace != NULL) {
				if (dom_string_isequal(namespace, corestring_dom_html_namespace)) {
					proto_atom = heap->html_element_proto_atom;
				} else {
					proto_atom = heap->element_proto_atom;
				}
				dom_string_unref(namespace);
			} else {
				proto_atom = heap->element_proto_atom;
			}
			break;
		}
		case DOM_TEXT_NODE:
			proto_atom = heap->text_proto_atom;
			break;
		case DOM_COMMENT_NODE:
			proto_atom = heap->comment_proto_atom;
			break;
		default:
			break;
		}
	}

	JSValue proto = JS_GetProperty(ctx, global, proto_atom);
	JSValue obj = JS_NewObjectProtoClass(ctx, proto, heap->node_class_id);
	JS_FreeValue(ctx, proto);

	if (JS_IsException(obj)) {
		JS_FreeValue(ctx, global);
		return obj;
	}

	JS_SetOpaque(obj, node);
	dom_node_ref(node);

	JS_FreeValue(ctx, global);

	/* Attach private maps for handlers and listeners */
	JS_SetProperty(ctx, obj, heap->handler_map_atom, JS_NewObject(ctx));
	JS_SetProperty(ctx, obj, heap->handler_listener_map_atom, JS_NewObject(ctx));
	JS_SetProperty(ctx, obj, heap->libdom_registered_atom, JS_NewObject(ctx));

	/* Store wrapper in user data (as a raw pointer, weak ref) */
	if (heap->node_key != NULL) {
		dom_node_set_user_data(node, heap->node_key, JS_VALUE_GET_PTR(obj), NULL, NULL);
	}

	return obj;
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
	struct dom_ui_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
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
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	int32_t x;
	dom_mouse_event_get_screen_x(evt, &x);
	return JS_NewInt32(ctx, x);
}

static JSValue qjsky_mouseevent_get_screenY(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	int32_t y;
	dom_mouse_event_get_screen_y(evt, &y);
	return JS_NewInt32(ctx, y);
}

static JSValue qjsky_mouseevent_get_clientX(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	int32_t x;
	dom_mouse_event_get_client_x(evt, &x);
	return JS_NewInt32(ctx, x);
}

static JSValue qjsky_mouseevent_get_clientY(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	int32_t y;
	dom_mouse_event_get_client_y(evt, &y);
	return JS_NewInt32(ctx, y);
}

static JSValue qjsky_mouseevent_get_ctrlKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_mouse_event_get_ctrl_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_mouseevent_get_shiftKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_mouse_event_get_shift_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_mouseevent_get_altKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_mouse_event_get_alt_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_mouseevent_get_metaKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_mouse_event_get_meta_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_mouseevent_get_button(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_mouse_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
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
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
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
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
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
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	dom_key_location loc;
	dom_keyboard_event_get_location(evt, &loc);
	return JS_NewInt32(ctx, loc);
}

static JSValue qjsky_keyboardevent_get_ctrlKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_keyboard_event_get_ctrl_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_keyboardevent_get_shiftKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_keyboard_event_get_shift_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_keyboardevent_get_altKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
	if (!evt) return JS_EXCEPTION;
	bool key;
	dom_keyboard_event_get_alt_key(evt, &key);
	return JS_NewBool(ctx, key);
}

static JSValue qjsky_keyboardevent_get_metaKey(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_keyboard_event *evt = JS_GetOpaque2(ctx, this_val, heap->event_class_id);
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

static JSValue qjsky_interface_ctor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv)
{
	return JS_ThrowTypeError(ctx, "Illegal constructor");
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
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));

	exc = dom_event_get_type(evt, &name);
	if (exc != DOM_NO_ERR) return;

	exc = dom_event_get_current_target(evt, &targ);
	if (exc != DOM_NO_ERR) {
		dom_string_unref(name);
		return;
	}

	JSValue node_val = qjsky_push_node(ctx, (struct dom_node *)targ);
	JSValue handler_map = JS_GetProperty(ctx, node_val, heap->handler_map_atom);
	JSValue handler_listener_map = JS_GetProperty(ctx, node_val, heap->handler_listener_map_atom);

	JSAtom prop = JS_NewAtomLen(ctx, (const char *)dom_string_data(name), dom_string_byte_length(name));

	/* Check for 'on' attribute style handler */
	JSValue handler = JS_GetProperty(ctx, handler_map, prop);
	if (JS_IsFunction(ctx, handler)) {
		JSValue event_val = qjsky_push_event(ctx, evt);
		JSValue ret = JS_Call(ctx, handler, node_val, 1, &event_val);
		if (JS_IsException(ret)) qjs_log_exception(ctx, "Event handler error");

		if (JS_IsBool(ret) && !JS_ToBool(ctx, ret)) {
			dom_event_prevent_default(evt);
		}

		JS_FreeValue(ctx, ret);
		JS_FreeValue(ctx, event_val);
	}
	JS_FreeValue(ctx, handler);

	/* Check for addEventListener style handlers */
	JSValue listeners = JS_GetProperty(ctx, handler_listener_map, prop);
	if (JS_IsObject(listeners)) {
		uint32_t len = 0;
		JSValue len_val = JS_GetPropertyStr(ctx, listeners, "length");
		JS_ToUint32(ctx, &len, len_val);
		JS_FreeValue(ctx, len_val);

		/* Create a copy to handle listeners added/removed during dispatch */
		JSValue *listeners_copy = malloc(sizeof(JSValue) * len);
		if (listeners_copy == NULL) {
			JS_FreeValue(ctx, listeners);
			JS_FreeAtom(ctx, prop);
			JS_FreeValue(ctx, handler_listener_map);
			JS_FreeValue(ctx, handler_map);
			JS_FreeValue(ctx, node_val);
			dom_node_unref((struct dom_node *)targ);
			dom_string_unref(name);
			return;
		}
		for (uint32_t i = 0; i < len; i++) {
			listeners_copy[i] = JS_GetPropertyUint32(ctx, listeners, i);
		}

		dom_event_flow_phase phase;
		dom_event_get_event_phase(evt, &phase);

		for (uint32_t i = 0; i < len; i++) {
			JSValue entry = listeners_copy[i];
			JSValue func = JS_GetPropertyStr(ctx, entry, "func");
			JSValue c_val = JS_GetPropertyStr(ctx, entry, "capture");
			bool capture = JS_ToBool(ctx, c_val);
			JS_FreeValue(ctx, c_val);

			bool skip = false;
			if (phase == DOM_CAPTURING_PHASE && !capture) skip = true;
			else if (phase == DOM_BUBBLING_PHASE && capture) skip = true;

			if (!skip && JS_IsFunction(ctx, func)) {
				JSValue event_val = qjsky_push_event(ctx, evt);
				JSValue ret = JS_Call(ctx, func, node_val, 1, &event_val);
				if (JS_IsException(ret)) qjs_log_exception(ctx, "Event listener error");
				JS_FreeValue(ctx, ret);
				JS_FreeValue(ctx, event_val);
			}
			JS_FreeValue(ctx, func);
			JS_FreeValue(ctx, entry);
		}
		free(listeners_copy);
	}
	JS_FreeValue(ctx, listeners);

	JS_FreeAtom(ctx, prop);
	JS_FreeValue(ctx, handler_listener_map);
	JS_FreeValue(ctx, handler_map);
	JS_FreeValue(ctx, node_val);

	JSContext *ctx1;
	while (JS_ExecutePendingJob(JS_GetRuntime(ctx), &ctx1) > 0);

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

	/* Mark as registered in libdom */
	JSValue node_val = qjsky_push_node(ctx, (struct dom_node *)ele);
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue libdom_reg = JS_GetProperty(ctx, node_val, heap->libdom_registered_atom);
	JSAtom prop = JS_NewAtomLen(ctx, (const char *)dom_string_data(name), dom_string_byte_length(name));
	JS_SetProperty(ctx, libdom_reg, prop, JS_TRUE);
	JS_FreeAtom(ctx, prop);
	JS_FreeValue(ctx, libdom_reg);
	JS_FreeValue(ctx, node_val);
}

static JSValue qjsky_event_target_addEventListener(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_node *node = JS_GetOpaque2(ctx, this_val, heap->node_class_id);
	if (!node) return JS_EXCEPTION;

	if (argc < 2) return JS_EXCEPTION;

	dom_string *name = qjsky_js_value_to_dom_string(ctx, argv[0]);
	JSValue func = argv[1];
	bool capture = false;

	if (argc > 2) {
		if (JS_IsBool(argv[2])) {
			capture = JS_ToBool(ctx, argv[2]);
		} else if (JS_IsObject(argv[2])) {
			JSValue cap = JS_GetPropertyStr(ctx, argv[2], "capture");
			capture = JS_ToBool(ctx, cap);
			JS_FreeValue(ctx, cap);
		}
	}

	JSValue handler_listener_map = JS_GetProperty(ctx, this_val, heap->handler_listener_map_atom);
	JSAtom prop = JS_ValueToAtom(ctx, argv[0]);

	JSValue listeners = JS_GetProperty(ctx, handler_listener_map, prop);
	if (!JS_IsObject(listeners)) {
		JS_FreeValue(ctx, listeners);
		listeners = JS_NewArray(ctx);
		JS_SetProperty(ctx, handler_listener_map, prop, JS_DupValue(ctx, listeners));
	}

	/* Add to listeners array */
	uint32_t len = 0;
	JSValue len_val = JS_GetPropertyStr(ctx, listeners, "length");
	JS_ToUint32(ctx, &len, len_val);
	JS_FreeValue(ctx, len_val);

	/* Check for duplicate */
	for (uint32_t i = 0; i < len; i++) {
		JSValue entry = JS_GetPropertyUint32(ctx, listeners, i);
		JSValue f = JS_GetPropertyStr(ctx, entry, "func");
		JSValue c = JS_GetPropertyStr(ctx, entry, "capture");
		bool cb = JS_ToBool(ctx, c);
		bool match = (JS_StrictEquality(ctx, f, func) && cb == capture);
		JS_FreeValue(ctx, c);
		JS_FreeValue(ctx, f);
		JS_FreeValue(ctx, entry);
		if (match) {
			JS_FreeValue(ctx, listeners);
			JS_FreeAtom(ctx, prop);
			JS_FreeValue(ctx, handler_listener_map);
			dom_string_unref(name);
			return JS_UNDEFINED;
		}
	}

	JSValue entry = JS_NewObject(ctx);
	JS_SetPropertyStr(ctx, entry, "func", JS_DupValue(ctx, func));
	JS_SetPropertyStr(ctx, entry, "capture", JS_NewBool(ctx, capture));
	JS_SetPropertyUint32(ctx, listeners, len, entry);

	JS_FreeValue(ctx, listeners);
	JS_FreeAtom(ctx, prop);
	JS_FreeValue(ctx, handler_listener_map);

	/* Register with libdom if not already registered for this type */
	JSValue libdom_reg = JS_GetProperty(ctx, this_val, heap->libdom_registered_atom);
	JSValue is_reg = JS_GetProperty(ctx, libdom_reg, prop);
	if (!JS_ToBool(ctx, is_reg)) {
		dom_event_listener *listen = NULL;
		dom_event_listener_create(qjsky_generic_event_handler, ctx, &listen);
		dom_event_target_add_event_listener(node, name, listen, false);
		dom_event_target_add_event_listener(node, name, listen, true);
		dom_event_listener_unref(listen);
		JS_SetProperty(ctx, libdom_reg, prop, JS_TRUE);
	}
	JS_FreeValue(ctx, is_reg);
	JS_FreeValue(ctx, libdom_reg);

	dom_string_unref(name);
	return JS_UNDEFINED;
}

static JSValue qjsky_event_target_removeEventListener(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_node *node = JS_GetOpaque2(ctx, this_val, heap->node_class_id);
	if (!node) return JS_EXCEPTION;

	if (argc < 2) return JS_EXCEPTION;

	dom_string *name = qjsky_js_value_to_dom_string(ctx, argv[0]);
	JSValue func = argv[1];
	bool capture = false;

	if (argc > 2) {
		if (JS_IsBool(argv[2])) {
			capture = JS_ToBool(ctx, argv[2]);
		} else if (JS_IsObject(argv[2])) {
			JSValue cap = JS_GetPropertyStr(ctx, argv[2], "capture");
			capture = JS_ToBool(ctx, cap);
			JS_FreeValue(ctx, cap);
		}
	}

	JSValue handler_listener_map = JS_GetProperty(ctx, this_val, heap->handler_listener_map_atom);
	JSAtom prop = JS_ValueToAtom(ctx, argv[0]);
	JSValue listeners = JS_GetProperty(ctx, handler_listener_map, prop);

	bool removed_from_js = false;
	if (JS_IsObject(listeners)) {
		uint32_t len = 0;
		JSValue len_val = JS_GetPropertyStr(ctx, listeners, "length");
		JS_ToUint32(ctx, &len, len_val);
		JS_FreeValue(ctx, len_val);

		for (uint32_t i = 0; i < len; i++) {
			JSValue entry = JS_GetPropertyUint32(ctx, listeners, i);
			JSValue f = JS_GetPropertyStr(ctx, entry, "func");
			JSValue c = JS_GetPropertyStr(ctx, entry, "capture");
			bool cb = JS_ToBool(ctx, c);

			if (JS_StrictEquality(ctx, f, func) && cb == capture) {
				/* Found it, remove from array */
				JSValue splice_fn = JS_GetPropertyStr(ctx, listeners, "splice");
				JSValue splice_args[2];
				splice_args[0] = JS_NewUint32(ctx, i);
				splice_args[1] = JS_NewUint32(ctx, 1);
				JSValue splice_ret = JS_Call(ctx, splice_fn, listeners, 2, splice_args);
				JS_FreeValue(ctx, splice_ret);
				JS_FreeValue(ctx, splice_fn);
				removed_from_js = true;
				JS_FreeValue(ctx, c);
				JS_FreeValue(ctx, f);
				JS_FreeValue(ctx, entry);
				break;
			}
			JS_FreeValue(ctx, c);
			JS_FreeValue(ctx, f);
			JS_FreeValue(ctx, entry);
		}
	}

	if (removed_from_js) {
		/* Check if we still have any listeners for this type and phase */
		bool still_have_listener = false;
		uint32_t len = 0;
		JSValue len_val = JS_GetPropertyStr(ctx, listeners, "length");
		JS_ToUint32(ctx, &len, len_val);
		JS_FreeValue(ctx, len_val);

		for (uint32_t i = 0; i < len; i++) {
			JSValue entry = JS_GetPropertyUint32(ctx, listeners, i);
			JSValue c = JS_GetPropertyStr(ctx, entry, "capture");
			bool cb = JS_ToBool(ctx, c);
			if (cb == capture) {
				still_have_listener = true;
				JS_FreeValue(ctx, c);
				JS_FreeValue(ctx, entry);
				break;
			}
			JS_FreeValue(ctx, c);
			JS_FreeValue(ctx, entry);
		}

		if (!still_have_listener) {
			dom_event_listener *listen = NULL;
			dom_event_listener_create(qjsky_generic_event_handler, ctx, &listen);
			dom_event_target_remove_event_listener(node, name, listen, capture);
			dom_event_listener_unref(listen);

			/* If no listeners left at all for this type, unmark libdom registered */
			if (len == 0) {
				JSValue libdom_reg = JS_GetProperty(ctx, this_val, heap->libdom_registered_atom);
				JS_SetProperty(ctx, libdom_reg, prop, JS_FALSE);
				JS_FreeValue(ctx, libdom_reg);
			}
		}
	}

	JS_FreeValue(ctx, listeners);
	JS_FreeAtom(ctx, prop);
	JS_FreeValue(ctx, handler_listener_map);

	dom_string_unref(name);
	return JS_UNDEFINED;
}

static JSValue qjsky_document_get_cookie(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_node *node = JS_GetOpaque2(ctx, this_val, heap->node_class_id);
	if (!node) return JS_EXCEPTION;

	dom_node_type nodetype;
	if (dom_node_get_node_type(node, &nodetype) != DOM_NO_ERR || nodetype != DOM_DOCUMENT_NODE) {
		return JS_ThrowTypeError(ctx, "Expected Document node");
	}

	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (!thread || !thread->doc_priv) return JS_UNDEFINED;
	struct html_content *htmlc = thread->doc_priv;

	char *cookie_str = urldb_get_cookie(llcache_handle_get_url(htmlc->base.llcache), false);
	if (cookie_str == NULL) return JS_NewString(ctx, "");

	JSValue val = JS_NewString(ctx, cookie_str);
	free(cookie_str);
	return val;
}

static JSValue qjsky_document_set_cookie(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_node *node = JS_GetOpaque2(ctx, this_val, heap->node_class_id);
	if (!node) return JS_EXCEPTION;

	dom_node_type nodetype;
	if (dom_node_get_node_type(node, &nodetype) != DOM_NO_ERR || nodetype != DOM_DOCUMENT_NODE) {
		return JS_ThrowTypeError(ctx, "Expected Document node");
	}

	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (!thread || !thread->doc_priv) return JS_UNDEFINED;
	struct html_content *htmlc = thread->doc_priv;

	const char *cookie_str = JS_ToCString(ctx, val);
	if (cookie_str == NULL) return JS_EXCEPTION;

	urldb_set_cookie(cookie_str, llcache_handle_get_url(htmlc->base.llcache), NULL);

	JS_FreeCString(ctx, cookie_str);
	return JS_UNDEFINED;
}

static JSValue qjsky_document_get_implementation(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	struct dom_node *node = JS_GetOpaque2(ctx, this_val, heap->node_class_id);
	if (!node) return JS_EXCEPTION;

	dom_node_type nodetype;
	if (dom_node_get_node_type(node, &nodetype) != DOM_NO_ERR || nodetype != DOM_DOCUMENT_NODE) {
		return JS_ThrowTypeError(ctx, "Expected Document node");
	}

	JSValue proto = JS_GetClassProto(ctx, heap->dom_implementation_class_id);
	JSValue obj = JS_NewObjectProtoClass(ctx, proto, heap->dom_implementation_class_id);
	JS_FreeValue(ctx, proto);
	return obj;
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

	JSValue proto = JS_GetProperty(ctx, global, proto_atom);
	JSValue obj = JS_NewObjectProtoClass(ctx, proto, heap->event_class_id);
	JS_FreeValue(ctx, proto);
	JS_FreeValue(ctx, global);

	if (JS_IsException(obj)) {
		return obj;
	}

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

void qjsky_init_runtime(struct jsheap *heap)
{
	JS_NewClassID(&heap->node_class_id);
	JS_NewClass(heap->rt, heap->node_class_id, &qjsky_node_class);

	JS_NewClassID(&heap->event_class_id);
	JS_NewClass(heap->rt, heap->event_class_id, &qjsky_event_class);

	JS_NewClassID(&heap->dom_implementation_class_id);
	JS_NewClass(heap->rt, heap->dom_implementation_class_id, &qjsky_dom_implementation_class);

	JS_NewClassID(&heap->xhr_class_id);
	/* XHR class initialized in xhr.c */

	JS_NewClassID(&heap->location_class_id);
	JS_NewClassID(&heap->history_class_id);
	JS_NewClassID(&heap->navigator_class_id);
	JS_NewClassID(&heap->screen_class_id);
	JS_NewClassID(&heap->barprop_class_id);
	JS_NewClassID(&heap->url_class_id);
	JS_NewClassID(&heap->urlsearchparams_class_id);
	JS_NewClassID(&heap->storage_class_id);
	JS_NewClass(heap->rt, heap->storage_class_id, &qjsky_storage_class);

	heap->handler_map_atom = JS_ATOM_NULL;
	heap->handler_listener_map_atom = JS_ATOM_NULL;
	heap->libdom_registered_atom = JS_ATOM_NULL;
	heap->node_proto_atom = JS_ATOM_NULL;
	heap->document_proto_atom = JS_ATOM_NULL;
	heap->element_proto_atom = JS_ATOM_NULL;
	heap->html_element_proto_atom = JS_ATOM_NULL;
	heap->text_proto_atom = JS_ATOM_NULL;
	heap->comment_proto_atom = JS_ATOM_NULL;
	heap->dom_implementation_proto_atom = JS_ATOM_NULL;
	heap->event_proto_atom = JS_ATOM_NULL;
	heap->uievent_proto_atom = JS_ATOM_NULL;
	heap->mouseevent_proto_atom = JS_ATOM_NULL;
	heap->keyboardevent_proto_atom = JS_ATOM_NULL;
	heap->storage_proto_atom = JS_ATOM_NULL;
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
	if (heap->handler_map_atom == JS_ATOM_NULL) {
		JSValue sym_ctor = JS_GetPropertyStr(ctx, global, "Symbol");
		JSValue arg;
		JSValue sym;

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

		arg = JS_NewString(ctx, "libdomRegistered");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->libdom_registered_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "nodeProto");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->node_proto_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "documentProto");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->document_proto_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "elementProto");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->element_proto_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "htmlElementProto");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->html_element_proto_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "textProto");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->text_proto_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "commentProto");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->comment_proto_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "domImplementationProto");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->dom_implementation_proto_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "eventProto");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->event_proto_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "domImplementationCtor");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->dom_implementation_ctor_atom = JS_ValueToAtom(ctx, sym);
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

		arg = JS_NewString(ctx, "nodeCtor");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->node_ctor_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "documentCtor");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->document_ctor_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "elementCtor");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->element_ctor_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "htmlElementCtor");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->html_element_ctor_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "textCtor");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->text_ctor_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "commentCtor");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->comment_ctor_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "eventCtor");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->event_ctor_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "uieventCtor");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->uievent_ctor_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "mouseeventCtor");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->mouseevent_ctor_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		arg = JS_NewString(ctx, "keyboardeventCtor");
		sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->keyboardevent_ctor_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);

		JS_FreeValue(ctx, sym_ctor);
	}

	/* Set up DOM hierarchy prototypes */
	JSValue node_proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, node_proto, qjsky_node_proto_funcs, sizeof(qjsky_node_proto_funcs)/sizeof(qjsky_node_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->node_class_id, JS_DupValue(ctx, node_proto));

	JSValue document_proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, document_proto, qjsky_document_proto_funcs, sizeof(qjsky_document_proto_funcs)/sizeof(qjsky_document_proto_funcs[0]));
	JS_SetPrototype(ctx, document_proto, node_proto);

	JSValue element_proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, element_proto, qjsky_element_proto_funcs, sizeof(qjsky_element_proto_funcs)/sizeof(qjsky_element_proto_funcs[0]));
	JS_SetPrototype(ctx, element_proto, node_proto);

	JSValue html_element_proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, html_element_proto, qjsky_html_element_proto_funcs, sizeof(qjsky_html_element_proto_funcs)/sizeof(qjsky_html_element_proto_funcs[0]));
	JS_SetPrototype(ctx, html_element_proto, element_proto);

	JSValue text_proto = JS_NewObject(ctx);
	JS_SetPrototype(ctx, text_proto, node_proto);

	JSValue comment_proto = JS_NewObject(ctx);
	JS_SetPrototype(ctx, comment_proto, node_proto);

	JSValue dom_implementation_proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, dom_implementation_proto, qjsky_dom_implementation_proto_funcs, sizeof(qjsky_dom_implementation_proto_funcs)/sizeof(qjsky_dom_implementation_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->dom_implementation_class_id, JS_DupValue(ctx, dom_implementation_proto));

	/* Set up constructors and expose them on global object for instanceof support */
	JSValue node_ctor = JS_NewCFunction2(ctx, qjsky_interface_ctor, "Node", 0, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, node_ctor, node_proto);
	JS_SetPropertyStr(ctx, global, "Node", JS_DupValue(ctx, node_ctor));
	JS_SetProperty(ctx, global, heap->node_ctor_atom, JS_DupValue(ctx, node_ctor));

	JSValue document_ctor = JS_NewCFunction2(ctx, qjsky_interface_ctor, "Document", 0, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, document_ctor, document_proto);
	JS_SetPropertyStr(ctx, global, "Document", JS_DupValue(ctx, document_ctor));
	JS_SetProperty(ctx, global, heap->document_ctor_atom, JS_DupValue(ctx, document_ctor));

	JSValue element_ctor = JS_NewCFunction2(ctx, qjsky_interface_ctor, "Element", 0, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, element_ctor, element_proto);
	JS_SetPropertyStr(ctx, global, "Element", JS_DupValue(ctx, element_ctor));
	JS_SetProperty(ctx, global, heap->element_ctor_atom, JS_DupValue(ctx, element_ctor));

	JSValue html_element_ctor = JS_NewCFunction2(ctx, qjsky_interface_ctor, "HTMLElement", 0, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, html_element_ctor, html_element_proto);
	JS_SetPropertyStr(ctx, global, "HTMLElement", JS_DupValue(ctx, html_element_ctor));
	JS_SetProperty(ctx, global, heap->html_element_ctor_atom, JS_DupValue(ctx, html_element_ctor));

	JSValue text_ctor = JS_NewCFunction2(ctx, qjsky_interface_ctor, "Text", 0, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, text_ctor, text_proto);
	JS_SetPropertyStr(ctx, global, "Text", JS_DupValue(ctx, text_ctor));
	JS_SetProperty(ctx, global, heap->text_ctor_atom, JS_DupValue(ctx, text_ctor));

	JSValue comment_ctor = JS_NewCFunction2(ctx, qjsky_interface_ctor, "Comment", 0, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, comment_ctor, comment_proto);
	JS_SetPropertyStr(ctx, global, "Comment", JS_DupValue(ctx, comment_ctor));
	JS_SetProperty(ctx, global, heap->comment_ctor_atom, JS_DupValue(ctx, comment_ctor));

	JSValue dom_implementation_ctor = JS_NewCFunction2(ctx, qjsky_interface_ctor, "DOMImplementation", 0, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, dom_implementation_ctor, dom_implementation_proto);
	JS_SetPropertyStr(ctx, global, "DOMImplementation", JS_DupValue(ctx, dom_implementation_ctor));
	JS_SetProperty(ctx, global, heap->dom_implementation_ctor_atom, JS_DupValue(ctx, dom_implementation_ctor));

	/* Attach prototypes to global using hidden atoms for internal use */
	JS_SetProperty(ctx, global, heap->node_proto_atom, JS_DupValue(ctx, node_proto));
	JS_SetProperty(ctx, global, heap->document_proto_atom, JS_DupValue(ctx, document_proto));
	JS_SetProperty(ctx, global, heap->element_proto_atom, JS_DupValue(ctx, element_proto));
	JS_SetProperty(ctx, global, heap->html_element_proto_atom, JS_DupValue(ctx, html_element_proto));
	JS_SetProperty(ctx, global, heap->text_proto_atom, JS_DupValue(ctx, text_proto));
	JS_SetProperty(ctx, global, heap->comment_proto_atom, JS_DupValue(ctx, comment_proto));
	JS_SetProperty(ctx, global, heap->dom_implementation_proto_atom, JS_DupValue(ctx, dom_implementation_proto));

	/* Initialize Event prototypes and inheritance */
	JSValue event_proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, event_proto, qjsky_event_proto_funcs, sizeof(qjsky_event_proto_funcs)/sizeof(qjsky_event_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->event_class_id, JS_DupValue(ctx, event_proto));

	JSValue event_ctor = JS_NewCFunction2(ctx, qjsky_event_ctor, "Event", 1, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, event_ctor, event_proto);
	JS_SetPropertyStr(ctx, global, "Event", JS_DupValue(ctx, event_ctor));
	JS_SetProperty(ctx, global, heap->event_ctor_atom, JS_DupValue(ctx, event_ctor));

	JSValue uievent_proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, uievent_proto, qjsky_uievent_proto_funcs, sizeof(qjsky_uievent_proto_funcs)/sizeof(qjsky_uievent_proto_funcs[0]));
	JS_SetPrototype(ctx, uievent_proto, event_proto);

	JSValue uievent_ctor = JS_NewCFunction2(ctx, qjsky_interface_ctor, "UIEvent", 0, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, uievent_ctor, uievent_proto);
	JS_SetPropertyStr(ctx, global, "UIEvent", JS_DupValue(ctx, uievent_ctor));
	JS_SetProperty(ctx, global, heap->uievent_ctor_atom, JS_DupValue(ctx, uievent_ctor));

	JSValue mouseevent_proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, mouseevent_proto, qjsky_mouseevent_proto_funcs, sizeof(qjsky_mouseevent_proto_funcs)/sizeof(qjsky_mouseevent_proto_funcs[0]));
	JS_SetPrototype(ctx, mouseevent_proto, uievent_proto);

	JSValue mouseevent_ctor = JS_NewCFunction2(ctx, qjsky_interface_ctor, "MouseEvent", 0, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, mouseevent_ctor, mouseevent_proto);
	JS_SetPropertyStr(ctx, global, "MouseEvent", JS_DupValue(ctx, mouseevent_ctor));
	JS_SetProperty(ctx, global, heap->mouseevent_ctor_atom, JS_DupValue(ctx, mouseevent_ctor));

	JSValue keyboardevent_proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, keyboardevent_proto, qjsky_keyboardevent_proto_funcs, sizeof(qjsky_keyboardevent_proto_funcs)/sizeof(qjsky_keyboardevent_proto_funcs[0]));
	JS_SetPrototype(ctx, keyboardevent_proto, uievent_proto);

	JSValue keyboardevent_ctor = JS_NewCFunction2(ctx, qjsky_interface_ctor, "KeyboardEvent", 0, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, keyboardevent_ctor, keyboardevent_proto);
	JS_SetPropertyStr(ctx, global, "KeyboardEvent", JS_DupValue(ctx, keyboardevent_ctor));
	JS_SetProperty(ctx, global, heap->keyboardevent_ctor_atom, JS_DupValue(ctx, keyboardevent_ctor));

	/* Attach prototypes to global using hidden atoms and consume references */
	JS_SetProperty(ctx, global, heap->event_proto_atom, JS_DupValue(ctx, event_proto));
	JS_SetProperty(ctx, global, heap->uievent_proto_atom, JS_DupValue(ctx, uievent_proto));
	JS_SetProperty(ctx, global, heap->mouseevent_proto_atom, JS_DupValue(ctx, mouseevent_proto));
	JS_SetProperty(ctx, global, heap->keyboardevent_proto_atom, JS_DupValue(ctx, keyboardevent_proto));

	/* Free all locally created values as they are now owned by global or the runtime */
	JS_FreeValue(ctx, node_proto);
	JS_FreeValue(ctx, document_proto);
	JS_FreeValue(ctx, element_proto);
	JS_FreeValue(ctx, html_element_proto);
	JS_FreeValue(ctx, text_proto);
	JS_FreeValue(ctx, comment_proto);
	JS_FreeValue(ctx, dom_implementation_proto);
	JS_FreeValue(ctx, event_proto);
	JS_FreeValue(ctx, uievent_proto);
	JS_FreeValue(ctx, mouseevent_proto);
	JS_FreeValue(ctx, keyboardevent_proto);

	JS_FreeValue(ctx, node_ctor);
	JS_FreeValue(ctx, document_ctor);
	JS_FreeValue(ctx, element_ctor);
	JS_FreeValue(ctx, html_element_ctor);
	JS_FreeValue(ctx, text_ctor);
	JS_FreeValue(ctx, comment_ctor);
	JS_FreeValue(ctx, dom_implementation_ctor);
	JS_FreeValue(ctx, event_ctor);
	JS_FreeValue(ctx, uievent_ctor);
	JS_FreeValue(ctx, mouseevent_ctor);
	JS_FreeValue(ctx, keyboardevent_ctor);

	JS_FreeValue(ctx, global);
}
