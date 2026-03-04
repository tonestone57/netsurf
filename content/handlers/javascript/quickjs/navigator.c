#include <stdlib.h>
#include <quickjs.h>
#include <string.h>
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

static JSValue qjsky_navigator_get_appVersion(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewString(ctx, "3.4");
}

static JSValue qjsky_navigator_get_product(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewString(ctx, "Gecko");
}

static JSValue qjsky_navigator_get_productSub(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewString(ctx, "20100101");
}

static JSValue qjsky_navigator_get_vendor(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewString(ctx, "");
}

static JSValue qjsky_navigator_get_vendorSub(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewString(ctx, "");
}

static JSValue qjsky_navigator_get_cookieEnabled(JSContext *ctx, JSValueConst this_val)
{
	return JS_TRUE;
}

static JSValue qjsky_navigator_get_onLine(JSContext *ctx, JSValueConst this_val)
{
	return JS_TRUE;
}

static JSValue qjsky_navigator_get_hardwareConcurrency(JSContext *ctx, JSValueConst this_val)
{
	return JS_NewInt32(ctx, 1);
}

static JSValue qjsky_navigator_get_language(JSContext *ctx, JSValueConst this_val)
{
	const char *accept_language = nsoption_charp(accept_language);
	if (accept_language == NULL || accept_language[0] == '\0') {
		return JS_NewString(ctx, "en-GB");
	}

	const char *p = accept_language;
	while (*p == ' ') p++;
	const char *comma = strchr(p, ',');
	size_t len = comma ? (size_t)(comma - p) : strlen(p);
	const char *semi = memchr(p, ';', len);
	if (semi) len = semi - p;
	while (len > 0 && p[len-1] == ' ') len--;

	if (len == 0) return JS_NewString(ctx, "en-GB");
	return JS_NewStringLen(ctx, p, len);
}

static JSValue qjsky_navigator_get_languages(JSContext *ctx, JSValueConst this_val)
{
	const char *accept_language = nsoption_charp(accept_language);
	JSValue arr = JS_NewArray(ctx);
	if (accept_language == NULL || accept_language[0] == '\0') {
		JS_SetPropertyUint32(ctx, arr, 0, JS_NewString(ctx, "en-GB"));
		return arr;
	}

	const char *p = accept_language;
	uint32_t i = 0;
	while (p != NULL && *p != '\0') {
		while (*p == ' ') p++;
		const char *end = strchr(p, ',');
		size_t len = end ? (size_t)(end - p) : strlen(p);
		const char *semi = memchr(p, ';', len);
		if (semi) len = semi - p;
		while (len > 0 && p[len-1] == ' ') len--;

		if (len > 0) {
			JS_SetPropertyUint32(ctx, arr, i++, JS_NewStringLen(ctx, p, len));
		}
		if (end == NULL) break;
		p = end + 1;
	}
	return arr;
}

static JSValue qjsky_navigator_taintEnabled(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return JS_FALSE;
}

static JSValue qjsky_navigator_javaEnabled(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	return JS_FALSE;
}

static const JSCFunctionListEntry qjsky_navigator_proto_funcs[] = {
	JS_CGETSET_DEF("userAgent", qjsky_navigator_get_userAgent, NULL),
	JS_CGETSET_DEF("appName", qjsky_navigator_get_appName, NULL),
	JS_CGETSET_DEF("appCodeName", qjsky_navigator_get_appCodeName, NULL),
	JS_CGETSET_DEF("appVersion", qjsky_navigator_get_appVersion, NULL),
	JS_CGETSET_DEF("platform", qjsky_navigator_get_platform, NULL),
	JS_CGETSET_DEF("product", qjsky_navigator_get_product, NULL),
	JS_CGETSET_DEF("productSub", qjsky_navigator_get_productSub, NULL),
	JS_CGETSET_DEF("vendor", qjsky_navigator_get_vendor, NULL),
	JS_CGETSET_DEF("vendorSub", qjsky_navigator_get_vendorSub, NULL),
	JS_CGETSET_DEF("cookieEnabled", qjsky_navigator_get_cookieEnabled, NULL),
	JS_CGETSET_DEF("onLine", qjsky_navigator_get_onLine, NULL),
	JS_CGETSET_DEF("hardwareConcurrency", qjsky_navigator_get_hardwareConcurrency, NULL),
	JS_CGETSET_DEF("language", qjsky_navigator_get_language, NULL),
	JS_CGETSET_DEF("languages", qjsky_navigator_get_languages, NULL),
	JS_CFUNC_DEF("taintEnabled", 0, qjsky_navigator_taintEnabled),
	JS_CFUNC_DEF("javaEnabled", 0, qjsky_navigator_javaEnabled),
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
