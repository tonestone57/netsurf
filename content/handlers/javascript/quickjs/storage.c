#include <string.h>
#include <stdlib.h>
#include "content/handlers/javascript/quickjs/storage.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "content/handlers/javascript/quickjs/qjs_utils.h"

static JSValue qjsky_storage_getItem(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	if (argc < 1) return JS_EXCEPTION;
	JSAtom prop = JS_ValueToAtom(ctx, argv[0]);
	/* Use JS_GetPropertyInternal to avoid potential prototype pollution or methods shadowing */
	JSValue val = JS_GetProperty(ctx, this_val, prop);
	JS_FreeAtom(ctx, prop);
	if (JS_IsFunction(ctx, val) || JS_IsUndefined(val)) {
		JS_FreeValue(ctx, val);
		return JS_NULL;
	}
	return val;
}

static JSValue qjsky_storage_setItem(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	if (argc < 2) return JS_EXCEPTION;
	JSAtom prop = JS_ValueToAtom(ctx, argv[0]);
	/* Spec says values must be stringified */
	JSValue str = JS_ToString(ctx, argv[1]);
	if (JS_IsException(str)) {
		JS_FreeAtom(ctx, prop);
		return str;
	}
	JS_SetProperty(ctx, this_val, prop, str);
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
	JSPropertyEnum *tab;
	uint32_t len, i;
	if (JS_GetOwnPropertyNames(ctx, &tab, &len, this_val, JS_GPN_STRING_MASK) < 0)
		return JS_EXCEPTION;
	for (i = 0; i < len; i++) {
		JS_DeleteProperty(ctx, this_val, tab[i].atom, 0);
		JS_FreeAtom(ctx, tab[i].atom);
	}
	js_free(ctx, tab);
	return JS_UNDEFINED;
}

static JSValue qjsky_storage_get_length(JSContext *ctx, JSValueConst this_val)
{
	JSPropertyEnum *tab;
	uint32_t len;
	if (JS_GetOwnPropertyNames(ctx, &tab, &len, this_val, JS_GPN_STRING_MASK) < 0)
		return JS_NewInt32(ctx, 0);
	for (uint32_t i = 0; i < len; i++) {
		JS_FreeAtom(ctx, tab[i].atom);
	}
	js_free(ctx, tab);
	return JS_NewUint32(ctx, len);
}

static JSValue qjsky_storage_key(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	if (argc < 1) return JS_EXCEPTION;
	uint32_t index;
	if (JS_ToUint32(ctx, &index, argv[0])) return JS_EXCEPTION;

	JSPropertyEnum *tab;
	uint32_t len;
	if (JS_GetOwnPropertyNames(ctx, &tab, &len, this_val, JS_GPN_STRING_MASK) < 0)
		return JS_EXCEPTION;

	JSValue res = JS_NULL;
	if (index < len) {
		res = JS_AtomToValue(ctx, tab[index].atom);
	}

	for (uint32_t i = 0; i < len; i++) {
		JS_FreeAtom(ctx, tab[i].atom);
	}
	js_free(ctx, tab);
	return res;
}

static const JSCFunctionListEntry qjsky_storage_proto_funcs[] = {
	JS_CFUNC_DEF("getItem", 1, qjsky_storage_getItem),
	JS_CFUNC_DEF("setItem", 2, qjsky_storage_setItem),
	JS_CFUNC_DEF("removeItem", 1, qjsky_storage_removeItem),
	JS_CFUNC_DEF("clear", 0, qjsky_storage_clear),
	JS_CFUNC_DEF("key", 1, qjsky_storage_key),
	JS_CGETSET_DEF("length", qjsky_storage_get_length, NULL),
};

JSClassDef qjsky_storage_class = {
	"Storage",
};

void qjsky_init_storage(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue global = JS_GetGlobalObject(ctx);

	if (heap->storage_proto_atom == JS_ATOM_NULL) {
		JSValue sym_ctor = JS_GetPropertyStr(ctx, global, "Symbol");
		JSValue arg = JS_NewString(ctx, "storageProto");
		JSValue sym = JS_Call(ctx, sym_ctor, JS_UNDEFINED, 1, &arg);
		heap->storage_proto_atom = JS_ValueToAtom(ctx, sym);
		JS_FreeValue(ctx, sym);
		JS_FreeValue(ctx, arg);
		JS_FreeValue(ctx, sym_ctor);
	}

	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_storage_proto_funcs, sizeof(qjsky_storage_proto_funcs)/sizeof(qjsky_storage_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->storage_class_id, JS_DupValue(ctx, proto));
	JS_SetProperty(ctx, global, heap->storage_proto_atom, proto);

	JS_FreeValue(ctx, global);
}

JSValue qjsky_create_storage(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue global = JS_GetGlobalObject(ctx);
	JSValue proto = JS_GetProperty(ctx, global, heap->storage_proto_atom);
	JSValue obj = JS_NewObjectProtoClass(ctx, proto, heap->storage_class_id);
	JS_FreeValue(ctx, proto);
	JS_FreeValue(ctx, global);
	return obj;
}
