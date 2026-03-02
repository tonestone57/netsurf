#include <stdlib.h>
#include <quickjs.h>
#include "content/handlers/javascript/quickjs/history.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "desktop/browser_history.h"
#include "netsurf/browser_window.h"

JSClassDef qjsky_history_class = {
	"History",
	.finalizer = NULL,
};

static JSValue qjsky_history_back(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (thread && thread->win_priv) {
		browser_window_history_back(thread->win_priv, false);
	}
	return JS_UNDEFINED;
}

static JSValue qjsky_history_forward(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (thread && thread->win_priv) {
		browser_window_history_forward(thread->win_priv, false);
	}
	return JS_UNDEFINED;
}

static JSValue qjsky_history_go(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (thread && thread->win_priv && argc > 0) {
		int delta;
		JS_ToInt32(ctx, &delta, argv[0]);
		/* Simple implementation for now */
		if (delta < 0) browser_window_history_back(thread->win_priv, false);
		else if (delta > 0) browser_window_history_forward(thread->win_priv, false);
	}
	return JS_UNDEFINED;
}

static const JSCFunctionListEntry qjsky_history_proto_funcs[] = {
	JS_CFUNC_DEF("back", 0, qjsky_history_back),
	JS_CFUNC_DEF("forward", 0, qjsky_history_forward),
	JS_CFUNC_DEF("go", 1, qjsky_history_go),
};

void qjsky_init_history(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));

	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_history_proto_funcs, sizeof(qjsky_history_proto_funcs)/sizeof(qjsky_history_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->history_class_id, proto);
}

JSValue qjsky_create_history(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	return JS_NewObjectClass(ctx, heap->history_class_id);
}
