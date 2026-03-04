#include <string.h>
#include <stdlib.h>
#include "content/handlers/javascript/quickjs/storage.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "content/handlers/javascript/quickjs/qjs_utils.h"

static JSValue qjsky_storage_getItem(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	if (argc < 1) return JS_EXCEPTION;
	JSAtom prop = JS_ValueToAtom(ctx, argv[0]);
	JSValue val = JS_GetProperty(ctx, this_val, prop);
	JS_FreeAtom(ctx, prop);
	return val;
}

static JSValue qjsky_storage_setItem(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	if (argc < 2) return JS_EXCEPTION;
	JSAtom prop = JS_ValueToAtom(ctx, argv[0]);
	JS_SetProperty(ctx, this_val, prop, JS_DupValue(ctx, argv[1]));
	JS_FreeAtom(ctx, prop);
	return JS_UNDEFINED;
}

static JSValue qjsky_storage_removeItem(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	if (argc < 1) return JS_EXCEPTION;
	JSAtom prop = JS_ValueToAtom(ctx, argv[0]);
	JS_DeleteProperty(ctx, this_val, prop, 0);
	JS_FreeAtom(ctx, prop);
	return JS_UNDEFINED;
}

static JSValue qjsky_storage_clear(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	/* Simple implementation: just replace the object properties or similar. */
	return JS_UNDEFINED;
}

static JSValue qjsky_storage_get_length(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewInt32(ctx, 0); /* Placeholder */
}

static const JSCFunctionListEntry qjsky_storage_proto_funcs[] = {
	JS_CFUNC_DEF("getItem", 1, qjsky_storage_getItem),
	JS_CFUNC_DEF("setItem", 2, qjsky_storage_setItem),
	JS_CFUNC_DEF("removeItem", 1, qjsky_storage_removeItem),
	JS_CFUNC_DEF("clear", 0, qjsky_storage_clear),
	JS_CGETSET_DEF("length", qjsky_storage_get_length, NULL),
};

static JSClassDef qjsky_storage_class = {
	"Storage",
};

void qjsky_init_storage(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JS_NewClass(JS_GetRuntime(ctx), heap->storage_class_id, &qjsky_storage_class);
}

JSValue qjsky_create_storage(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue obj = JS_NewObjectClass(ctx, heap->storage_class_id);
	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_storage_proto_funcs, sizeof(qjsky_storage_proto_funcs)/sizeof(qjsky_storage_proto_funcs[0]));
	JS_SetPrototype(ctx, obj, proto);
	JS_FreeValue(ctx, proto);
	return obj;
}
