#include <stdlib.h>
#include <quickjs.h>
#include "content/handlers/javascript/quickjs/screen.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "desktop/browser_private.h"

JSClassDef qjsky_screen_class = {
	"Screen",
	.finalizer = NULL,
};

static JSValue qjsky_screen_get_width(JSContext *ctx, JSValueConst this_val)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	int width = 800;
	if (thread && thread->win_priv) {
		width = ((struct browser_window *)thread->win_priv)->width;
	}
	return JS_NewInt32(ctx, width);
}

static JSValue qjsky_screen_get_height(JSContext *ctx, JSValueConst this_val)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	int height = 600;
	if (thread && thread->win_priv) {
		height = ((struct browser_window *)thread->win_priv)->height;
	}
	return JS_NewInt32(ctx, height);
}

static JSValue qjsky_screen_get_availWidth(JSContext *ctx, JSValueConst this_val)
{
	return qjsky_screen_get_width(ctx, this_val);
}

static JSValue qjsky_screen_get_availHeight(JSContext *ctx, JSValueConst this_val)
{
	return qjsky_screen_get_height(ctx, this_val);
}

static JSValue qjsky_screen_get_colorDepth(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewInt32(ctx, 24);
}

static JSValue qjsky_screen_get_pixelDepth(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewInt32(ctx, 24);
}

static const JSCFunctionListEntry qjsky_screen_proto_funcs[] = {
	JS_CGETSET_DEF("width", qjsky_screen_get_width, NULL),
	JS_CGETSET_DEF("height", qjsky_screen_get_height, NULL),
	JS_CGETSET_DEF("availWidth", qjsky_screen_get_availWidth, NULL),
	JS_CGETSET_DEF("availHeight", qjsky_screen_get_availHeight, NULL),
	JS_CGETSET_DEF("colorDepth", qjsky_screen_get_colorDepth, NULL),
	JS_CGETSET_DEF("pixelDepth", qjsky_screen_get_pixelDepth, NULL),
};

void qjsky_init_screen(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));

	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_screen_proto_funcs, sizeof(qjsky_screen_proto_funcs)/sizeof(qjsky_screen_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->screen_class_id, proto);
}

JSValue qjsky_create_screen(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	return JS_NewObjectClass(ctx, heap->screen_class_id);
}
