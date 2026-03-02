#include <stdlib.h>
#include <quickjs.h>
#include "content/handlers/javascript/quickjs/navigator.h"
#include "utils/useragent.h"

static JSClassID qjsky_navigator_class_id = 0;

static JSClassDef qjsky_navigator_class = {
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
	return JS_NULL;
}

static JSValue qjsky_navigator_get_cookieEnabled(JSContext *ctx, JSValueConst this_val)
{
	return JS_FALSE;
}

static const JSCFunctionListEntry qjsky_navigator_proto_funcs[] = {
	JS_CGETSET_DEF("userAgent", qjsky_navigator_get_userAgent, NULL),
	JS_CGETSET_DEF("appName", qjsky_navigator_get_appName, NULL),
	JS_CGETSET_DEF("appCodeName", qjsky_navigator_get_appCodeName, NULL),
	JS_CGETSET_DEF("platform", qjsky_navigator_get_platform, NULL),
	JS_CGETSET_DEF("cookieEnabled", qjsky_navigator_get_cookieEnabled, NULL),
};

void qjsky_init_navigator(JSContext *ctx)
{
	if (qjsky_navigator_class_id == 0) {
		JS_NewClassID(&qjsky_navigator_class_id);
	}
	JS_NewClass(JS_GetRuntime(ctx), qjsky_navigator_class_id, &qjsky_navigator_class);

	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_navigator_proto_funcs, sizeof(qjsky_navigator_proto_funcs)/sizeof(qjsky_navigator_proto_funcs[0]));
	JS_SetClassProto(ctx, qjsky_navigator_class_id, proto);
}

JSValue qjsky_create_navigator(JSContext *ctx)
{
	return JS_NewObjectClass(ctx, qjsky_navigator_class_id);
}
