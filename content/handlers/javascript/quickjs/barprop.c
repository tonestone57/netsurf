#include <quickjs.h>
#include "content/handlers/javascript/quickjs/barprop.h"

static JSValue qjsky_barprop_get_visible(JSContext *ctx, JSValueConst this_val)
{
	return JS_TRUE;
}

static const JSCFunctionListEntry qjsky_barprop_proto_funcs[] = {
	JS_CGETSET_DEF("visible", qjsky_barprop_get_visible, NULL),
};

void qjsky_init_barprop(JSContext *ctx)
{
	JSValue global = JS_GetGlobalObject(ctx);
	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_barprop_proto_funcs, sizeof(qjsky_barprop_proto_funcs)/sizeof(qjsky_barprop_proto_funcs[0]));
	JS_SetPropertyStr(ctx, global, "BarProp", proto);
	JS_FreeValue(ctx, global);
}

JSValue qjsky_create_barprop(JSContext *ctx)
{
	JSValue global = JS_GetGlobalObject(ctx);
	JSValue proto = JS_GetPropertyStr(ctx, global, "BarProp");
	JSValue obj = JS_NewObjectProto(ctx, proto);
	JS_FreeValue(ctx, proto);
	JS_FreeValue(ctx, global);
	return obj;
}
