#include <stdlib.h>
#include <quickjs.h>
#include "content/handlers/javascript/quickjs/barprop.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"

JSClassDef qjsky_barprop_class = {
	"BarProp",
	.finalizer = NULL,
};

static JSValue qjsky_barprop_get_visible(JSContext *ctx, JSValueConst this_val)
{
	return JS_TRUE;
}

static const JSCFunctionListEntry qjsky_barprop_proto_funcs[] = {
	JS_CGETSET_DEF("visible", qjsky_barprop_get_visible, NULL),
};

void qjsky_init_barprop(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));

	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_barprop_proto_funcs, sizeof(qjsky_barprop_proto_funcs)/sizeof(qjsky_barprop_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->barprop_class_id, proto);
}

JSValue qjsky_create_barprop(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	return JS_NewObjectClass(ctx, heap->barprop_class_id);
}
