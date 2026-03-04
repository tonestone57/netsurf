#include <stdlib.h>
#include <quickjs.h>
#include "content/handlers/javascript/quickjs/navigator.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "utils/useragent.h"
#include "utils/nsoption.h"

JSClassDef qjsky_navigator_class = {
	"Navigator",
	.finalizer = NULL,
};

static JSValue qjsky_navigator_get_userAgent(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewString(ctx, user_agent_string());
}

static JSValue qjsky_navigator_get_appVersion(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewString(ctx, user_agent_string());
}

static JSValue qjsky_navigator_get_appName(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewString(ctx, "Netscape");
}

static JSValue qjsky_navigator_get_appCodeName(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewString(ctx, "Mozilla");
}

static JSValue qjsky_navigator_get_platform(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewString(ctx, "");
}

static JSValue qjsky_navigator_get_product(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewString(ctx, "Gecko");
}

static JSValue qjsky_navigator_get_language(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewString(ctx, nsoption_charp(language));
}

static JSValue qjsky_navigator_get_languages(JSContext *ctx, JSValueConst this_val)
{
	JSValue arr = JS_NewArray(ctx);
	JS_SetPropertyUint32(ctx, arr, 0, JS_NewString(ctx, nsoption_charp(language)));
	return arr;
}

static JSValue qjsky_navigator_get_onLine(JSContext *ctx, JSValueConst this_val)
{
	return JS_TRUE;
}

static JSValue qjsky_navigator_get_cookieEnabled(JSContext *ctx, JSValueConst this_val)
{
	return JS_FALSE;
}

static JSValue qjsky_navigator_get_hardwareConcurrency(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewInt32(ctx, 1);
}

static const JSCFunctionListEntry qjsky_navigator_proto_funcs[] = {
	JS_CGETSET_DEF("userAgent", qjsky_navigator_get_userAgent, NULL),
	JS_CGETSET_DEF("appVersion", qjsky_navigator_get_appVersion, NULL),
	JS_CGETSET_DEF("appName", qjsky_navigator_get_appName, NULL),
	JS_CGETSET_DEF("appCodeName", qjsky_navigator_get_appCodeName, NULL),
	JS_CGETSET_DEF("platform", qjsky_navigator_get_platform, NULL),
	JS_CGETSET_DEF("product", qjsky_navigator_get_product, NULL),
	JS_CGETSET_DEF("language", qjsky_navigator_get_language, NULL),
	JS_CGETSET_DEF("languages", qjsky_navigator_get_languages, NULL),
	JS_CGETSET_DEF("onLine", qjsky_navigator_get_onLine, NULL),
	JS_CGETSET_DEF("cookieEnabled", qjsky_navigator_get_cookieEnabled, NULL),
	JS_CGETSET_DEF("hardwareConcurrency", qjsky_navigator_get_hardwareConcurrency, NULL),
};

void qjsky_init_navigator(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));

	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_navigator_proto_funcs, sizeof(qjsky_navigator_proto_funcs)/sizeof(qjsky_navigator_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->navigator_class_id, proto);
}

JSValue qjsky_create_navigator(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	return JS_NewObjectClass(ctx, heap->navigator_class_id);
}
