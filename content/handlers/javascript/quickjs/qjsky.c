#include <string.h>
#include <stdlib.h>
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "utils/log.h"
#include "utils/utils.h"
#include "nsutils/time.h"

static JSClassID qjsky_node_class_id = 0;

static void qjsky_node_finalizer(JSRuntime *rt, JSValue val)
{
	struct dom_node *node = JS_GetOpaque(val, qjsky_node_class_id);
	if (node) {
		dom_node_unref(node);
	}
}

static JSClassDef qjsky_node_class = {
	"Node",
	.finalizer = qjsky_node_finalizer,
};

void qjsky_init_runtime(JSRuntime *rt)
{
	if (qjsky_node_class_id == 0) {
		JS_NewClassID(&qjsky_node_class_id);
	}
	JS_NewClass(rt, qjsky_node_class_id, &qjsky_node_class);
	/* Prototype setup is done per context in current design */
}

JSValue qjsky_push_node(JSContext *ctx, struct dom_node *node)
{
	/* TODO: Implement proper prototype resolution based on node type.
	 * Currently creating objects with JS_NULL prototype.
	 */
	JSValue obj = JS_NewObjectProtoClass(ctx, JS_NULL, qjsky_node_class_id);
	if (JS_IsException(obj)) return obj;

	JS_SetOpaque(obj, node);
	dom_node_ref(node);

	return obj;
}

struct dom_node *qjsky_get_node(JSContext *ctx, JSValue val)
{
	return JS_GetOpaque(val, qjsky_node_class_id);
}

/* Task 9: Console Integration */
static JSValue qjsky_console_log(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    for (int i = 0; i < argc; i++) {
        const char *str = JS_ToCString(ctx, argv[i]);
        if (str) {
            NSLOG(netsurf, INFO, "JS: %s", str);
            JS_FreeCString(ctx, str);
        }
    }
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry qjsky_console_proto_funcs[] = {
    JS_CFUNC_DEF("log", 1, qjsky_console_log),
    JS_CFUNC_DEF("warn", 1, qjsky_console_log),
    JS_CFUNC_DEF("error", 1, qjsky_console_log),
    JS_CFUNC_DEF("info", 1, qjsky_console_log),
    JS_CFUNC_DEF("debug", 1, qjsky_console_log),
};

void qjsky_init_console(JSContext *ctx)
{
    JSValue global_obj = JS_GetGlobalObject(ctx);
    JSValue console = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, console, qjsky_console_proto_funcs,
                               countof(qjsky_console_proto_funcs));
    JS_SetPropertyStr(ctx, global_obj, "console", console);
    JS_FreeValue(ctx, global_obj);
}

/* Task 15: Window self/window/top */
void qjsky_init_window_aliases(JSContext *ctx)
{
    JSValue global_obj = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global_obj, "window", JS_DupValue(ctx, global_obj));
    JS_SetPropertyStr(ctx, global_obj, "self", JS_DupValue(ctx, global_obj));
    JS_SetPropertyStr(ctx, global_obj, "top", JS_DupValue(ctx, global_obj));
    JS_FreeValue(ctx, global_obj);
}

/* Task 19: Performance.now() */
static JSValue qjsky_performance_now(JSContext *ctx, JSValueConst this_val,
                                     int argc, JSValueConst *argv)
{
    uint64_t now;
    nsu_getmonotonic_ms(&now);
    return JS_NewFloat64(ctx, (double)now);
}

static const JSCFunctionListEntry qjsky_performance_proto_funcs[] = {
    JS_CFUNC_DEF("now", 0, qjsky_performance_now),
};

void qjsky_init_performance(JSContext *ctx)
{
    JSValue global_obj = JS_GetGlobalObject(ctx);
    JSValue performance = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, performance, qjsky_performance_proto_funcs,
                               countof(qjsky_performance_proto_funcs));
    JS_SetPropertyStr(ctx, global_obj, "performance", performance);
    JS_FreeValue(ctx, global_obj);
}

/* Task 18: Navigator API (partial) */
static JSValue qjsky_navigator_get_userAgent(JSContext *ctx, JSValueConst this_val)
{
    return JS_NewString(ctx, "NetSurf/3.11 (QuickJS)");
}

static JSValue qjsky_navigator_get_platform(JSContext *ctx, JSValueConst this_val)
{
    return JS_NewString(ctx, "NetSurf-Generic");
}

static const JSCFunctionListEntry qjsky_navigator_proto_funcs[] = {
    JS_CGETSET_DEF("userAgent", qjsky_navigator_get_userAgent, NULL),
    JS_CGETSET_DEF("platform", qjsky_navigator_get_platform, NULL),
};

void qjsky_init_navigator(JSContext *ctx)
{
    JSValue global_obj = JS_GetGlobalObject(ctx);
    JSValue navigator = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, navigator, qjsky_navigator_proto_funcs,
                               countof(qjsky_navigator_proto_funcs));
    JS_SetPropertyStr(ctx, global_obj, "navigator", navigator);
    JS_FreeValue(ctx, global_obj);
}

/* Task 22: Node.textContent */
static JSValue qjsky_node_get_textContent(JSContext *ctx, JSValueConst this_val)
{
    struct dom_node *node = JS_GetOpaque(this_val, qjsky_node_class_id);
    if (!node) return JS_NULL;

    dom_string *content;
    if (dom_node_get_text_content(node, &content) == DOM_NO_ERR && content) {
        JSValue res = JS_NewStringLen(ctx, dom_string_data(content), dom_string_length(content));
        dom_string_unref(content);
        return res;
    }
    return JS_NULL;
}

static JSValue qjsky_node_set_textContent(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
    struct dom_node *node = JS_GetOpaque(this_val, qjsky_node_class_id);
    if (!node) return JS_EXCEPTION;

    const char *str = JS_ToCString(ctx, val);
    if (str) {
        dom_string *ds;
        if (dom_string_create((const uint8_t *)str, strlen(str), &ds) == DOM_NO_ERR) {
            dom_node_set_text_content(node, ds);
            dom_string_unref(ds);
        }
        JS_FreeCString(ctx, str);
    }
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry qjsky_node_proto_funcs[] = {
    JS_CGETSET_DEF("textContent", qjsky_node_get_textContent, qjsky_node_set_textContent),
};

void qjsky_setup_node_prototype(JSContext *ctx, JSValue proto)
{
    JS_SetPropertyFunctionList(ctx, proto, qjsky_node_proto_funcs,
                               countof(qjsky_node_proto_funcs));
}

/* Task 17: Location API (stub) */
static JSValue qjsky_location_get_href(JSContext *ctx, JSValueConst this_val)
{
    return JS_NewString(ctx, "about:blank");
}

static const JSCFunctionListEntry qjsky_location_proto_funcs[] = {
    JS_CGETSET_DEF("href", qjsky_location_get_href, NULL),
};

void qjsky_init_location(JSContext *ctx)
{
    JSValue global_obj = JS_GetGlobalObject(ctx);
    JSValue location = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, location, qjsky_location_proto_funcs,
                               countof(qjsky_location_proto_funcs));
    JS_SetPropertyStr(ctx, global_obj, "location", location);
    JS_FreeValue(ctx, global_obj);
}

/* Task 16: History API (stub) */
static JSValue qjsky_history_back(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry qjsky_history_proto_funcs[] = {
    JS_CFUNC_DEF("back", 0, qjsky_history_back),
};

void qjsky_init_history(JSContext *ctx)
{
    JSValue global_obj = JS_GetGlobalObject(ctx);
    JSValue history = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, history, qjsky_history_proto_funcs,
                               countof(qjsky_history_proto_funcs));
    JS_SetPropertyStr(ctx, global_obj, "history", history);
    JS_FreeValue(ctx, global_obj);
}

/* Task 13: XMLHttpRequest Support (Skeleton) */
static JSClassID qjsky_xhr_class_id;

static void qjsky_xhr_finalizer(JSRuntime *rt, JSValue val)
{
}

static JSClassDef qjsky_xhr_class = {
    "XMLHttpRequest",
    .finalizer = qjsky_xhr_finalizer,
};

static JSValue qjsky_xhr_open(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv)
{
    return JS_UNDEFINED;
}

static JSValue qjsky_xhr_send(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv)
{
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry qjsky_xhr_proto_funcs[] = {
    JS_CFUNC_DEF("open", 2, qjsky_xhr_open),
    JS_CFUNC_DEF("send", 1, qjsky_xhr_send),
};

static JSValue qjsky_xhr_constructor(JSContext *ctx, JSValueConst new_target,
                                    int argc, JSValueConst *argv)
{
    JSValue obj = JS_NewObjectClass(ctx, qjsky_xhr_class_id);
    if (JS_IsException(obj)) return obj;
    return obj;
}

void qjsky_init_xhr(JSContext *ctx)
{
    JS_NewClassID(&qjsky_xhr_class_id);
    JS_NewClass(JS_GetRuntime(ctx), qjsky_xhr_class_id, &qjsky_xhr_class);

    JSValue proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, qjsky_xhr_proto_funcs,
                               countof(qjsky_xhr_proto_funcs));

    JSValue ctor = JS_NewCFunction2(ctx, qjsky_xhr_constructor, "XMLHttpRequest", 0, JS_CFUNC_constructor, 0);
    JS_SetConstructor(ctx, ctor, proto);
    JS_SetClassProto(ctx, qjsky_xhr_class_id, proto);

    JSValue global_obj = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global_obj, "XMLHttpRequest", ctor);
    JS_FreeValue(ctx, global_obj);
}

/* Task 14: Document.cookie */
static JSValue qjsky_document_get_cookie(JSContext *ctx, JSValueConst this_val)
{
    return JS_NewString(ctx, "");
}

static JSValue qjsky_document_set_cookie(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry qjsky_document_proto_funcs[] = {
    JS_CGETSET_DEF("cookie", qjsky_document_get_cookie, qjsky_document_set_cookie),
};

void qjsky_setup_document_prototype(JSContext *ctx, JSValue proto)
{
    JS_SetPropertyFunctionList(ctx, proto, qjsky_document_proto_funcs,
                               countof(qjsky_document_proto_funcs));
}

/* Task 23: Element.innerHTML (Skeleton) */
static JSValue qjsky_element_get_innerHTML(JSContext *ctx, JSValueConst this_val)
{
    return JS_NewString(ctx, "<!-- innerHTML not yet implemented -->");
}

static const JSCFunctionListEntry qjsky_element_proto_funcs[] = {
    JS_CGETSET_DEF("innerHTML", qjsky_element_get_innerHTML, NULL),
};

void qjsky_setup_element_prototype(JSContext *ctx, JSValue proto)
{
    JS_SetPropertyFunctionList(ctx, proto, qjsky_element_proto_funcs,
                               countof(qjsky_element_proto_funcs));
}

/* Task 8: Window Timer API (Skeleton) */
static JSValue qjsky_window_setTimeout(JSContext *ctx, JSValueConst this_val,
                                       int argc, JSValueConst *argv)
{
    /* To implement: schedule callback in NetSurf */
    return JS_NewInt32(ctx, 0);
}

static JSValue qjsky_window_clearTimeout(JSContext *ctx, JSValueConst this_val,
                                         int argc, JSValueConst *argv)
{
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry qjsky_timer_proto_funcs[] = {
    JS_CFUNC_DEF("setTimeout", 2, qjsky_window_setTimeout),
    JS_CFUNC_DEF("setInterval", 2, qjsky_window_setTimeout),
    JS_CFUNC_DEF("clearTimeout", 1, qjsky_window_clearTimeout),
    JS_CFUNC_DEF("clearInterval", 1, qjsky_window_clearTimeout),
};

void qjsky_init_timers(JSContext *ctx)
{
    JSValue global_obj = JS_GetGlobalObject(ctx);
    JS_SetPropertyFunctionList(ctx, global_obj, qjsky_timer_proto_funcs,
                               countof(qjsky_timer_proto_funcs));
    JS_FreeValue(ctx, global_obj);
}

/* Task 7: Event Target Support (Skeleton) */
static JSValue qjsky_event_target_addEventListener(JSContext *ctx, JSValueConst this_val,
                                                   int argc, JSValueConst *argv)
{
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry qjsky_event_target_proto_funcs[] = {
    JS_CFUNC_DEF("addEventListener", 3, qjsky_event_target_addEventListener),
};

void qjsky_setup_event_target_prototype(JSContext *ctx, JSValue proto)
{
    JS_SetPropertyFunctionList(ctx, proto, qjsky_event_target_proto_funcs,
                               countof(qjsky_event_target_proto_funcs));
}

/* Task 21: Attribute Node access (Stub) */
static JSValue qjsky_element_getAttributeNode(JSContext *ctx, JSValueConst this_val,
                                             int argc, JSValueConst *argv)
{
    return JS_NULL;
}

static const JSCFunctionListEntry qjsky_element_attr_funcs[] = {
    JS_CFUNC_DEF("getAttributeNode", 1, qjsky_element_getAttributeNode),
};

void qjsky_setup_element_attr_prototype(JSContext *ctx, JSValue proto)
{
    JS_SetPropertyFunctionList(ctx, proto, qjsky_element_attr_funcs,
                               countof(qjsky_element_attr_funcs));
}
