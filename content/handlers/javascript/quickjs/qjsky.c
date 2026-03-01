#include <string.h>
#include <stdlib.h>
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "utils/log.h"
#include "utils/corestrings.h"

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
}

JSValue qjsky_push_node(JSContext *ctx, struct dom_node *node)
{
    /* Task 2: Node Memoization (Conceptual implementation) */
    /* In practice, we'd store a Map in the context opaque and use it here */
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

/* Task 1: String Conversion Helpers */

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
	return JS_NewStringLen(ctx, (const char *)dom_string_data(str), dom_string_length(str));
}

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

static const JSCFunctionListEntry qjsky_console_funcs[] = {
	JS_CFUNC_DEF("log", 1, qjsky_console_log),
	JS_CFUNC_DEF("info", 1, qjsky_console_log),
	JS_CFUNC_DEF("warn", 1, qjsky_console_log),
	JS_CFUNC_DEF("error", 1, qjsky_console_log),
};

void qjsky_init_console(JSContext *ctx)
{
	JSValue global = JS_GetGlobalObject(ctx);
	JSValue console = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, console, qjsky_console_funcs, sizeof(qjsky_console_funcs)/sizeof(qjsky_console_funcs[0]));
	JS_SetPropertyStr(ctx, global, "console", console);
	JS_FreeValue(ctx, global);
}
