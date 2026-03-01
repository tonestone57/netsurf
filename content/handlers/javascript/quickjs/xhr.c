#include <stdlib.h>
#include <quickjs.h>
#include "content/handlers/javascript/quickjs/xhr.h"
#include "content/handlers/javascript/quickjs/qjs_utils.h"

static JSClassID qjsky_xhr_class_id = 0;

static void qjsky_xhr_finalizer(JSRuntime *rt, JSValue val)
{
	/* TODO: Cleanup XHR state */
}

static JSClassDef qjsky_xhr_class = {
	"XMLHttpRequest",
	.finalizer = qjsky_xhr_finalizer,
};

static JSValue qjsky_xhr_ctor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv)
{
	JSValue obj = JS_NewObjectProtoClass(ctx, new_target, qjsky_xhr_class_id);
	if (JS_IsException(obj)) return obj;

	/* TODO: Initialize private XHR state */

	return obj;
}

static JSValue qjsky_xhr_open(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return JS_UNDEFINED;
}

static JSValue qjsky_xhr_send(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return JS_UNDEFINED;
}

static const JSCFunctionListEntry qjsky_xhr_proto_funcs[] = {
	JS_CFUNC_DEF("open", 2, qjsky_xhr_open),
	JS_CFUNC_DEF("send", 1, qjsky_xhr_send),
};

void qjsky_init_xhr(JSContext *ctx)
{
	if (qjsky_xhr_class_id == 0) {
		JS_NewClassID(&qjsky_xhr_class_id);
	}
	/* Ensure class is registered for this runtime */
	JS_NewClass(JS_GetRuntime(ctx), qjsky_xhr_class_id, &qjsky_xhr_class);

	JSValue global = JS_GetGlobalObject(ctx);

	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_xhr_proto_funcs, sizeof(qjsky_xhr_proto_funcs)/sizeof(qjsky_xhr_proto_funcs[0]));
	JS_SetClassProto(ctx, qjsky_xhr_class_id, JS_DupValue(ctx, proto));

	JSValue ctor = JS_NewCFunction2(ctx, qjsky_xhr_ctor, "XMLHttpRequest", 0, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, ctor, proto);
	JS_SetPropertyStr(ctx, global, "XMLHttpRequest", ctor);

	JS_FreeValue(ctx, proto);
	JS_FreeValue(ctx, global);
}
