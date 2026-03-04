#include <stdlib.h>
#include <string.h>
#include <quickjs.h>
#include "content/handlers/javascript/quickjs/storage.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "content/handlers/javascript/quickjs/qjs_utils.h"

JSClassDef qjsky_storage_class = {
	"Storage",
	.finalizer = NULL,
};

static JSValue qjsky_storage_get_length(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewInt32(ctx, 0);
}

static JSValue qjsky_storage_key(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return JS_NULL;
}

static JSValue qjsky_storage_getItem(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return JS_NULL;
}

static JSValue qjsky_storage_setItem(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return JS_UNDEFINED;
}

static JSValue qjsky_storage_removeItem(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return JS_UNDEFINED;
}

static JSValue qjsky_storage_clear(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return JS_UNDEFINED;
}

static const JSCFunctionListEntry qjsky_storage_proto_funcs[] = {
	JS_CGETSET_DEF("length", qjsky_storage_get_length, NULL),
	JS_CFUNC_DEF("key", 1, qjsky_storage_key),
	JS_CFUNC_DEF("getItem", 1, qjsky_storage_getItem),
	JS_CFUNC_DEF("setItem", 2, qjsky_storage_setItem),
	JS_CFUNC_DEF("removeItem", 1, qjsky_storage_removeItem),
	JS_CFUNC_DEF("clear", 0, qjsky_storage_clear),
};

void qjsky_init_storage(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_storage_proto_funcs, sizeof(qjsky_storage_proto_funcs)/sizeof(qjsky_storage_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->storage_class_id, proto);
}

JSValue qjsky_create_storage(JSContext *ctx, bool session)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	return JS_NewObjectClass(ctx, heap->storage_class_id);
}
