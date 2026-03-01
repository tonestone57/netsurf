#include "content/handlers/javascript/quickjs/xhr.h"

static JSClassID qjsky_xhr_class_id;

static void qjsky_xhr_finalizer(JSRuntime *rt, JSValue val)
{
    /* Cleanup XHR state */
}

static JSClassDef qjsky_xhr_class = {
    "XMLHttpRequest",
    .finalizer = qjsky_xhr_finalizer,
};

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
    JS_NewClassID(&qjsky_xhr_class_id);
    JS_NewClass(JS_GetRuntime(ctx), qjsky_xhr_class_id, &qjsky_xhr_class);

    JSValue proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, qjsky_xhr_proto_funcs, sizeof(qjsky_xhr_proto_funcs)/sizeof(qjsky_xhr_proto_funcs[0]));

    JSValue global = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global, "XMLHttpRequest", JS_NewCFunction2(ctx, NULL, "XMLHttpRequest", 0, JS_CFUNC_constructor, 0));
    JS_FreeValue(ctx, global);
}
