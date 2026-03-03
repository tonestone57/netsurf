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

static void qjsky_location_finalizer(JSRuntime *rt, JSValue val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(rt);
	qjsky_location_t *loc = JS_GetOpaque(val, heap->location_class_id);
	if (loc) {
		nsurl_unref(loc->url);
		free(loc);
	}
}

JSClassDef qjsky_location_class = {
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
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_location_t *loc = JS_GetOpaque2(ctx, this_val, heap->location_class_id);
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
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_location_t *loc = JS_GetOpaque2(ctx, this_val, heap->location_class_id);
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
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_location_t *loc = JS_GetOpaque2(ctx, this_val, heap->location_class_id);
	if (!loc) return JS_EXCEPTION;

	char *url_s;
	size_t url_l;
	nsurl_get(loc->url, component, &url_s, &url_l);
	JSValue val = JS_NewStringLen(ctx, url_s ? url_s : "", url_l);
	free(url_s);
	return val;
}

static JSValue qjsky_location_get_protocol(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_SCHEME); }
static JSValue qjsky_location_get_host(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_HOST | NSURL_PORT); }
static JSValue qjsky_location_get_hostname(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_HOST); }
static JSValue qjsky_location_get_port(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_PORT); }
static JSValue qjsky_location_get_pathname(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_PATH); }
static JSValue qjsky_location_get_search(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_QUERY); }
static JSValue qjsky_location_get_hash(JSContext *ctx, JSValueConst this_val) { return qjsky_location_get_component(ctx, this_val, NSURL_FRAGMENT); }

static JSValue qjsky_location_set_protocol(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_location_t *loc = JS_GetOpaque2(ctx, this_val, heap->location_class_id);
	if (!loc) return JS_EXCEPTION;

	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (!thread || !thread->win_priv) return JS_EXCEPTION;

	size_t slen;
	const char *s = JS_ToCStringLen(ctx, &slen, val);
	if (!s) return JS_EXCEPTION;

	if (slen > 0 && s[slen-1] == ':') slen--;

	lwc_string *scheme;
	if (lwc_intern_string(s, slen, &scheme) != lwc_error_ok) {
		JS_FreeCString(ctx, s);
		return JS_EXCEPTION;
	}
	JS_FreeCString(ctx, s);

	nsurl *new_url;
	nsurl_replace_scheme(loc->url, scheme, &new_url);
	lwc_string_unref(scheme);

	if (new_url) {
		browser_window_navigate(thread->win_priv, new_url, NULL, BW_NAVIGATE_HISTORY, NULL, NULL, NULL);
		nsurl_unref(new_url);
	}

	return JS_UNDEFINED;
}

static JSValue qjsky_location_set_search(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_location_t *loc = JS_GetOpaque2(ctx, this_val, heap->location_class_id);
	if (!loc) return JS_EXCEPTION;

	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (!thread || !thread->win_priv) return JS_EXCEPTION;

	const char *s = JS_ToCString(ctx, val);
	if (!s) return JS_EXCEPTION;
	const char *p = s;
	if (p[0] == '?') p++;

	nsurl *new_url;
	nsurl_replace_query(loc->url, p, &new_url);
	JS_FreeCString(ctx, s);

	if (new_url) {
		browser_window_navigate(thread->win_priv, new_url, NULL, BW_NAVIGATE_HISTORY, NULL, NULL, NULL);
		nsurl_unref(new_url);
	}

	return JS_UNDEFINED;
}

static JSValue qjsky_location_set_hash(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_location_t *loc = JS_GetOpaque2(ctx, this_val, heap->location_class_id);
	if (!loc) return JS_EXCEPTION;

	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (!thread || !thread->win_priv) return JS_EXCEPTION;

	size_t slen;
	const char *s = JS_ToCStringLen(ctx, &slen, val);
	if (!s) return JS_EXCEPTION;
	const char *p = s;
	if (slen > 0 && p[0] == '#') { p++; slen--; }

	lwc_string *frag;
	if (lwc_intern_string(p, slen, &frag) != lwc_error_ok) {
		JS_FreeCString(ctx, s);
		return JS_EXCEPTION;
	}
	JS_FreeCString(ctx, s);

	nsurl *new_url;
	nsurl_refragment(loc->url, frag, &new_url);
	lwc_string_unref(frag);

	if (new_url) {
		browser_window_navigate(thread->win_priv, new_url, NULL, BW_NAVIGATE_HISTORY, NULL, NULL, NULL);
		nsurl_unref(new_url);
	}

	return JS_UNDEFINED;
}

static JSValue qjsky_location_set_host(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_location_t *loc = JS_GetOpaque2(ctx, this_val, heap->location_class_id);
	if (!loc) return JS_EXCEPTION;

	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (!thread || !thread->win_priv) return JS_EXCEPTION;

	char *path_s = NULL;
	size_t path_l = 0;
	nsurl_get(loc->url, NSURL_PATH | NSURL_QUERY | NSURL_FRAGMENT, &path_s, &path_l);

	const char *new_host = JS_ToCString(ctx, val);
	if (!new_host) {
		free(path_s);
		return JS_EXCEPTION;
	}

	size_t rel_len = strlen(new_host) + (path_s ? path_l : 1) + 3;
	char *rel = malloc(rel_len);
	if (rel) {
		snprintf(rel, rel_len, "//%s%s", new_host, (path_s ? path_s : "/"));
		nsurl *new_url;
		nsurl_join(loc->url, rel, &new_url);
		if (new_url) {
			browser_window_navigate(thread->win_priv, new_url, NULL, BW_NAVIGATE_HISTORY, NULL, NULL, NULL);
			nsurl_unref(new_url);
		}
		free(rel);
	}

	JS_FreeCString(ctx, new_host);
	free(path_s);
	return JS_UNDEFINED;
}

static JSValue qjsky_location_set_hostname(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_location_t *loc = JS_GetOpaque2(ctx, this_val, heap->location_class_id);
	if (!loc) return JS_EXCEPTION;

	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (!thread || !thread->win_priv) return JS_EXCEPTION;

	char *port_s = NULL;
	size_t port_l = 0;
	nsurl_get(loc->url, NSURL_PORT, &port_s, &port_l);

	char *path_s = NULL;
	size_t path_l = 0;
	nsurl_get(loc->url, NSURL_PATH | NSURL_QUERY | NSURL_FRAGMENT, &path_s, &path_l);

	const char *new_hostname = JS_ToCString(ctx, val);
	if (!new_hostname) {
		free(port_s);
		free(path_s);
		return JS_EXCEPTION;
	}

	size_t rel_len = strlen(new_hostname) + (port_s ? port_l + 1 : 0) + (path_s ? path_l : 1) + 3;
	char *rel = malloc(rel_len);
	if (rel) {
		snprintf(rel, rel_len, "//%s%s%s%s", new_hostname, (port_s ? ":" : ""), (port_s ? port_s : ""), (path_s ? path_s : "/"));
		nsurl *new_url;
		nsurl_join(loc->url, rel, &new_url);
		if (new_url) {
			browser_window_navigate(thread->win_priv, new_url, NULL, BW_NAVIGATE_HISTORY, NULL, NULL, NULL);
			nsurl_unref(new_url);
		}
		free(rel);
	}

	JS_FreeCString(ctx, new_hostname);
	free(port_s);
	free(path_s);
	return JS_UNDEFINED;
}

static JSValue qjsky_location_set_port(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_location_t *loc = JS_GetOpaque2(ctx, this_val, heap->location_class_id);
	if (!loc) return JS_EXCEPTION;

	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (!thread || !thread->win_priv) return JS_EXCEPTION;

	char *host_s = NULL;
	size_t host_l = 0;
	nsurl_get(loc->url, NSURL_HOST, &host_s, &host_l);

	char *path_s = NULL;
	size_t path_l = 0;
	nsurl_get(loc->url, NSURL_PATH | NSURL_QUERY | NSURL_FRAGMENT, &path_s, &path_l);

	const char *new_port = JS_ToCString(ctx, val);
	if (!new_port) {
		free(host_s);
		free(path_s);
		return JS_EXCEPTION;
	}

	size_t rel_len = (host_s ? host_l : 0) + strlen(new_port) + (path_s ? path_l : 1) + 4;
	char *rel = malloc(rel_len);
	if (rel) {
		snprintf(rel, rel_len, "//%s:%s%s", (host_s ? host_s : ""), new_port, (path_s ? path_s : "/"));
		nsurl *new_url;
		nsurl_join(loc->url, rel, &new_url);
		if (new_url) {
			browser_window_navigate(thread->win_priv, new_url, NULL, BW_NAVIGATE_HISTORY, NULL, NULL, NULL);
			nsurl_unref(new_url);
		}
		free(rel);
	}

	JS_FreeCString(ctx, new_port);
	free(host_s);
	free(path_s);
	return JS_UNDEFINED;
}

static JSValue qjsky_location_set_pathname(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_location_t *loc = JS_GetOpaque2(ctx, this_val, heap->location_class_id);
	if (!loc) return JS_EXCEPTION;

	struct jsthread *thread = JS_GetContextOpaque(ctx);
	if (!thread || !thread->win_priv) return JS_EXCEPTION;

	char *query_s = NULL;
	size_t query_l = 0;
	nsurl_get(loc->url, NSURL_QUERY | NSURL_FRAGMENT, &query_s, &query_l);

	const char *new_path = JS_ToCString(ctx, val);
	if (!new_path) {
		free(query_s);
		return JS_EXCEPTION;
	}

	size_t rel_len = strlen(new_path) + (query_s ? query_l : 0) + 1;
	char *rel = malloc(rel_len);
	if (rel) {
		snprintf(rel, rel_len, "%s%s", new_path, (query_s ? query_s : ""));
		nsurl *new_url;
		nsurl_join(loc->url, rel, &new_url);
		if (new_url) {
			browser_window_navigate(thread->win_priv, new_url, NULL, BW_NAVIGATE_HISTORY, NULL, NULL, NULL);
			nsurl_unref(new_url);
		}
		free(rel);
	}

	JS_FreeCString(ctx, new_path);
	free(query_s);
	return JS_UNDEFINED;
}

static const JSCFunctionListEntry qjsky_location_proto_funcs[] = {
	JS_CFUNC_DEF("reload", 0, qjsky_location_reload),
	JS_CGETSET_DEF("href", qjsky_location_get_href, qjsky_location_set_href),
	JS_CGETSET_DEF("protocol", qjsky_location_get_protocol, qjsky_location_set_protocol),
	JS_CGETSET_DEF("host", qjsky_location_get_host, qjsky_location_set_host),
	JS_CGETSET_DEF("hostname", qjsky_location_get_hostname, qjsky_location_set_hostname),
	JS_CGETSET_DEF("port", qjsky_location_get_port, qjsky_location_set_port),
	JS_CGETSET_DEF("pathname", qjsky_location_get_pathname, qjsky_location_set_pathname),
	JS_CGETSET_DEF("search", qjsky_location_get_search, qjsky_location_set_search),
	JS_CGETSET_DEF("hash", qjsky_location_get_hash, qjsky_location_set_hash),
};

void qjsky_init_location(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));

	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_location_proto_funcs, sizeof(qjsky_location_proto_funcs)/sizeof(qjsky_location_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->location_class_id, proto);
}

JSValue qjsky_create_location(JSContext *ctx, nsurl *url)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue obj = JS_NewObjectClass(ctx, heap->location_class_id);
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
