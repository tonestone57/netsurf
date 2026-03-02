#include <stdlib.h>
#include <string.h>
#include <quickjs.h>
#include "content/handlers/javascript/quickjs/location.h"
#include "content/handlers/javascript/quickjs/qjs_utils.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "netsurf/browser_window.h"
#include "utils/nsurl.h"
#include "utils/log.h"

typedef struct qjsky_location_s {
	nsurl *url;
} qjsky_location_t;

static JSClassID qjsky_location_class_id = 0;

static void qjsky_location_finalizer(JSRuntime *rt, JSValue val)
{
	qjsky_location_t *loc = JS_GetOpaque(val, qjsky_location_class_id);
	if (loc) {
		nsurl_unref(loc->url);
		free(loc);
	}
}

static JSClassDef qjsky_location_class = {
	"Location",
	.finalizer = qjsky_location_finalizer,
};

static JSValue qjsky_location_reload(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (thread && thread->win_priv) {
		browser_window_reload(thread->win_priv, false);
	}
	return JS_UNDEFINED;
}

static JSValue qjsky_location_get_href(JSContext *ctx, JSValueConst this_val)
{
	qjsky_location_t *loc = JS_GetOpaque2(ctx, this_val, qjsky_location_class_id);
	if (!loc) return JS_EXCEPTION;

	char *url_s;
	size_t url_l;
	nsurl_get(loc->url, NSURL_COMPLETE, &url_s, &url_l);
	if (!url_s) return JS_NULL;

	JSValue val = JS_NewStringLen(ctx, url_s, url_l);
	free(url_s);
	return val;
}

static JSValue qjsky_location_set_href(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	qjsky_location_t *loc = JS_GetOpaque2(ctx, this_val, qjsky_location_class_id);
	if (!loc) return JS_EXCEPTION;

	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (!thread || !thread->win_priv) return JS_EXCEPTION;

	const char *url_str = JS_ToCString(ctx, val);
	if (!url_str) return JS_EXCEPTION;

	nsurl *joined;
	nsurl_join(loc->url, url_str, &joined);
	JS_FreeCString(ctx, url_str);

	if (joined) {
		browser_window_navigate(thread->win_priv, joined, NULL, BW_NAVIGATE_HISTORY, NULL, NULL, NULL);
		nsurl_unref(joined);
	}

	return JS_UNDEFINED;
}

static JSValue qjsky_location_get_component(JSContext *ctx, JSValueConst this_val, nsurl_component component)
{
	qjsky_location_t *loc = JS_GetOpaque2(ctx, this_val, qjsky_location_class_id);
	if (!loc) return JS_EXCEPTION;

	char *url_s;
	size_t url_l;
	nsurl_get(loc->url, component, &url_s, &url_l);
	JSValue val = JS_NewStringLen(ctx, url_s ? url_s : "", url_l);
	free(url_s);
	return val;
}

static JSValue qjsky_location_get_protocol(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_SCHEME); }
static JSValue qjsky_location_get_host(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_HOST); }
static JSValue qjsky_location_get_hostname(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_HOST); }
static JSValue qjsky_location_get_port(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_PORT); }
static JSValue qjsky_location_get_pathname(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_PATH); }
static JSValue qjsky_location_get_search(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_QUERY); }
static JSValue qjsky_location_get_hash(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_FRAGMENT); }

static const JSCFunctionListEntry qjsky_location_proto_funcs[] = {
	JS_CFUNC_DEF("reload", 0, qjsky_location_reload),
	JS_CGETSET_DEF("href", qjsky_location_get_href, qjsky_location_set_href),
	JS_CGETSET_DEF("protocol", qjsky_location_get_protocol, NULL),
	JS_CGETSET_DEF("host", qjsky_location_get_host, NULL),
	JS_CGETSET_DEF("hostname", qjsky_location_get_hostname, NULL),
	JS_CGETSET_DEF("port", qjsky_location_get_port, NULL),
	JS_CGETSET_DEF("pathname", qjsky_location_get_pathname, NULL),
	JS_CGETSET_DEF("search", qjsky_location_get_search, NULL),
	JS_CGETSET_DEF("hash", qjsky_location_get_hash, NULL),
};

void qjsky_init_location(JSContext *ctx)
{
	if (qjsky_location_class_id == 0) {
		JS_NewClassID(&qjsky_location_class_id);
	}
	JS_NewClass(JS_GetRuntime(ctx), qjsky_location_class_id, &qjsky_location_class);

	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_location_proto_funcs, sizeof(qjsky_location_proto_funcs)/sizeof(qjsky_location_proto_funcs[0]));
	JS_SetClassProto(ctx, qjsky_location_class_id, proto);
}

JSValue qjsky_create_location(JSContext *ctx, nsurl *url)
{
	JSValue obj = JS_NewObjectClass(ctx, qjsky_location_class_id);
	if (JS_IsException(obj)) return obj;

	qjsky_location_t *loc = calloc(1, sizeof(*loc));
	if (!loc) {
		JS_FreeValue(ctx, obj);
		return JS_EXCEPTION;
	}
	loc->url = url;
	nsurl_ref(url);

	JS_SetOpaque(obj, loc);
	return obj;
}
