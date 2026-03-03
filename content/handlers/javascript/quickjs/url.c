#include <stdlib.h>
#include <string.h>
#include <quickjs.h>
#include "content/handlers/javascript/quickjs/url.h"
#include "content/handlers/javascript/quickjs/qjs_utils.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "utils/nsurl.h"
#include "utils/log.h"
#include "utils/corestrings.h"
#include "utils/url.h"

/* --- URLSearchParams implementation --- */

typedef struct qjsky_usp_entry_s {
	char *key;
	char *value;
	struct qjsky_usp_entry_s *next;
} qjsky_usp_entry_t;

typedef struct qjsky_usp_s {
	qjsky_usp_entry_t *entries;
} qjsky_usp_t;

static void qjsky_usp_finalizer(JSRuntime *rt, JSValue val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(rt);
	qjsky_usp_t *usp = JS_GetOpaque(val, heap->urlsearchparams_class_id);
	if (usp) {
		qjsky_usp_entry_t *curr = usp->entries;
		while (curr) {
			qjsky_usp_entry_t *next = curr->next;
			free(curr->key);
			free(curr->value);
			free(curr);
			curr = next;
		}
		free(usp);
	}
}

JSClassDef qjsky_usp_class = {
	"URLSearchParams",
	.finalizer = qjsky_usp_finalizer,
};

static void qjsky_usp_parse(qjsky_usp_t *usp, const char *input)
{
	if (!input || !*input) return;
	if (*input == '?') input++;

	char *str = strdup(input);
	char *saveptr;
	char *pair = strtok_r(str, "&", &saveptr);
	while (pair) {
		char *sep = strchr(pair, '=');
		char *key, *val;
		if (sep) {
			*sep = '\0';
			key = pair;
			val = sep + 1;
		} else {
			key = pair;
			val = "";
		}

		/* Simple unescape for + as space */
		char *ke = strdup(key);
		char *ve = strdup(val);
		for (char *p = ke; *p; p++) if (*p == '+') *p = ' ';
		for (char *p = ve; *p; p++) if (*p == '+') *p = ' ';

		char *ukey, *uval;
		if (url_unescape(ke, 0, NULL, &ukey) != NSERROR_OK) ukey = strdup(ke);
		if (url_unescape(ve, 0, NULL, &uval) != NSERROR_OK) uval = strdup(ve);
		free(ke);
		free(ve);

		qjsky_usp_entry_t *entry = calloc(1, sizeof(*entry));
		entry->key = ukey;
		entry->value = uval;

		if (!usp->entries) {
			usp->entries = entry;
		} else {
			qjsky_usp_entry_t *last = usp->entries;
			while (last->next) last = last->next;
			last->next = entry;
		}

		pair = strtok_r(NULL, "&", &saveptr);
	}
	free(str);
}

static JSValue qjsky_usp_append(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_usp_t *usp = JS_GetOpaque2(ctx, this_val, heap->urlsearchparams_class_id);
	if (!usp || argc < 2) return JS_EXCEPTION;

	const char *key = JS_ToCString(ctx, argv[0]);
	const char *val = JS_ToCString(ctx, argv[1]);

	qjsky_usp_entry_t *entry = calloc(1, sizeof(*entry));
	entry->key = strdup(key);
	entry->value = strdup(val);

	JS_FreeCString(ctx, key);
	JS_FreeCString(ctx, val);

	if (!usp->entries) {
		usp->entries = entry;
	} else {
		qjsky_usp_entry_t *last = usp->entries;
		while (last->next) last = last->next;
		last->next = entry;
	}
	return JS_UNDEFINED;
}

static JSValue qjsky_usp_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_usp_t *usp = JS_GetOpaque2(ctx, this_val, heap->urlsearchparams_class_id);
	if (!usp || argc < 1) return JS_EXCEPTION;

	const char *key = JS_ToCString(ctx, argv[0]);
	qjsky_usp_entry_t *curr = usp->entries;
	while (curr) {
		if (strcmp(curr->key, key) == 0) {
			JS_FreeCString(ctx, key);
			return JS_NewString(ctx, curr->value);
		}
		curr = curr->next;
	}
	JS_FreeCString(ctx, key);
	return JS_NULL;
}

static JSValue qjsky_usp_getAll(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_usp_t *usp = JS_GetOpaque2(ctx, this_val, heap->urlsearchparams_class_id);
	if (!usp || argc < 1) return JS_EXCEPTION;

	const char *key = JS_ToCString(ctx, argv[0]);
	JSValue arr = JS_NewArray(ctx);
	int i = 0;
	qjsky_usp_entry_t *curr = usp->entries;
	while (curr) {
		if (strcmp(curr->key, key) == 0) {
			JS_SetPropertyUint32(ctx, arr, i++, JS_NewString(ctx, curr->value));
		}
		curr = curr->next;
	}
	JS_FreeCString(ctx, key);
	return arr;
}

static JSValue qjsky_usp_has(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_usp_t *usp = JS_GetOpaque2(ctx, this_val, heap->urlsearchparams_class_id);
	if (!usp || argc < 1) return JS_EXCEPTION;

	const char *key = JS_ToCString(ctx, argv[0]);
	qjsky_usp_entry_t *curr = usp->entries;
	while (curr) {
		if (strcmp(curr->key, key) == 0) {
			JS_FreeCString(ctx, key);
			return JS_TRUE;
		}
		curr = curr->next;
	}
	JS_FreeCString(ctx, key);
	return JS_FALSE;
}

static JSValue qjsky_usp_set(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_usp_t *usp = JS_GetOpaque2(ctx, this_val, heap->urlsearchparams_class_id);
	if (!usp || argc < 2) return JS_EXCEPTION;

	const char *key = JS_ToCString(ctx, argv[0]);
	const char *val = JS_ToCString(ctx, argv[1]);

	qjsky_usp_entry_t *curr = usp->entries;
	qjsky_usp_entry_t *prev = NULL;
	bool found = false;

	while (curr) {
		if (strcmp(curr->key, key) == 0) {
			if (!found) {
				free(curr->value);
				curr->value = strdup(val);
				found = true;
				prev = curr;
				curr = curr->next;
			} else {
				/* Remove subsequent entries with same key */
				qjsky_usp_entry_t *next = curr->next;
				if (prev) prev->next = next;
				else usp->entries = next;
				free(curr->key);
				free(curr->value);
				free(curr);
				curr = next;
			}
		} else {
			prev = curr;
			curr = curr->next;
		}
	}

	if (!found) {
		qjsky_usp_entry_t *entry = calloc(1, sizeof(*entry));
		entry->key = strdup(key);
		entry->value = strdup(val);
		if (prev) prev->next = entry;
		else usp->entries = entry;
	}

	JS_FreeCString(ctx, key);
	JS_FreeCString(ctx, val);
	return JS_UNDEFINED;
}

static JSValue qjsky_usp_delete(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_usp_t *usp = JS_GetOpaque2(ctx, this_val, heap->urlsearchparams_class_id);
	if (!usp || argc < 1) return JS_EXCEPTION;

	const char *key = JS_ToCString(ctx, argv[0]);
	qjsky_usp_entry_t *curr = usp->entries;
	qjsky_usp_entry_t *prev = NULL;

	while (curr) {
		if (strcmp(curr->key, key) == 0) {
			qjsky_usp_entry_t *next = curr->next;
			if (prev) prev->next = next;
			else usp->entries = next;
			free(curr->key);
			free(curr->value);
			free(curr);
			curr = next;
		} else {
			prev = curr;
			curr = curr->next;
		}
	}

	JS_FreeCString(ctx, key);
	return JS_UNDEFINED;
}

static JSValue qjsky_usp_toString(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_usp_t *usp = JS_GetOpaque2(ctx, this_val, heap->urlsearchparams_class_id);
	if (!usp) return JS_EXCEPTION;

	JSValue res = JS_NewString(ctx, "");
	qjsky_usp_entry_t *curr = usp->entries;
	while (curr) {
		if (JS_ToBool(ctx, JS_GetPropertyStr(ctx, res, "length"))) {
			res = JS_ConcatString(ctx, res, JS_NewString(ctx, "&"));
		}

		char *key_esc, *val_esc;
		url_escape(curr->key, true, NULL, &key_esc);
		url_escape(curr->value, true, NULL, &val_esc);

		char *pair;
		asprintf(&pair, "%s=%s", key_esc, val_esc);
		res = JS_ConcatString(ctx, res, JS_NewString(ctx, pair));

		free(pair);
		free(key_esc);
		free(val_esc);
		curr = curr->next;
	}
	return res;
}

static const JSCFunctionListEntry qjsky_usp_proto_funcs[] = {
	JS_CFUNC_DEF("append", 2, qjsky_usp_append),
	JS_CFUNC_DEF("get", 1, qjsky_usp_get),
	JS_CFUNC_DEF("getAll", 1, qjsky_usp_getAll),
	JS_CFUNC_DEF("has", 1, qjsky_usp_has),
	JS_CFUNC_DEF("set", 2, qjsky_usp_set),
	JS_CFUNC_DEF("delete", 1, qjsky_usp_delete),
	JS_CFUNC_DEF("toString", 0, qjsky_usp_toString),
};

void qjsky_init_urlsearchparams(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_usp_proto_funcs, sizeof(qjsky_usp_proto_funcs)/sizeof(qjsky_usp_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->urlsearchparams_class_id, proto);
}

JSValue qjsky_create_urlsearchparams(JSContext *ctx, const char *init)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue obj = JS_NewObjectClass(ctx, heap->urlsearchparams_class_id);
	if (JS_IsException(obj)) return obj;

	qjsky_usp_t *usp = calloc(1, sizeof(*usp));
	if (!usp) {
		JS_FreeValue(ctx, obj);
		return JS_EXCEPTION;
	}
	qjsky_usp_parse(usp, init);
	JS_SetOpaque(obj, usp);
	return obj;
}

JSValue qjsky_usp_ctor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv)
{
	const char *init = "";
	if (argc > 0) init = JS_ToCString(ctx, argv[0]);
	JSValue obj = qjsky_create_urlsearchparams(ctx, init);
	if (argc > 0) JS_FreeCString(ctx, init);
	return obj;
}

/* --- URL implementation --- */

typedef struct qjsky_url_s {
	nsurl *url;
	JSValue search_params;
} qjsky_url_t;

static void qjsky_url_finalizer(JSRuntime *rt, JSValue val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(rt);
	qjsky_url_t *url_priv = JS_GetOpaque(val, heap->url_class_id);
	if (url_priv) {
		if (url_priv->url) nsurl_unref(url_priv->url);
		JS_FreeValueRT(rt, url_priv->search_params);
		free(url_priv);
	}
}

JSClassDef qjsky_url_class = {
	"URL",
	.finalizer = qjsky_url_finalizer,
};

static JSValue qjsky_url_get_href(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_url_t *url_priv = JS_GetOpaque2(ctx, this_val, heap->url_class_id);
	if (!url_priv) return JS_EXCEPTION;

	char *url_s;
	size_t url_l;
	nsurl_get(url_priv->url, NSURL_COMPLETE, &url_s, &url_l);
	JSValue val = JS_NewStringLen(ctx, url_s ? url_s : "", url_l);
	free(url_s);
	return val;
}

static JSValue qjsky_url_get_component(JSContext *ctx, JSValueConst this_val, nsurl_component component)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_url_t *url_priv = JS_GetOpaque2(ctx, this_val, heap->url_class_id);
	if (!url_priv) return JS_EXCEPTION;

	char *url_s;
	size_t url_l;
	nsurl_get(url_priv->url, component, &url_s, &url_l);
	JSValue val = JS_NewStringLen(ctx, url_s ? url_s : "", url_l);
	free(url_s);
	return val;
}

static JSValue qjsky_url_get_protocol(JSContext *ctx, JSValueConst this_val) { return qjsky_url_get_component(ctx, this_val, NSURL_SCHEME); }
static JSValue qjsky_url_get_host(JSContext *ctx, JSValueConst this_val) { return qjsky_url_get_component(ctx, this_val, NSURL_HOST | NSURL_PORT); }
static JSValue qjsky_url_get_hostname(JSContext *ctx, JSValueConst this_val) { return qjsky_url_get_component(ctx, this_val, NSURL_HOST); }
static JSValue qjsky_url_get_port(JSContext *ctx, JSValueConst this_val) { return qjsky_url_get_component(ctx, this_val, NSURL_PORT); }
static JSValue qjsky_url_get_pathname(JSContext *ctx, JSValueConst this_val) { return qjsky_url_get_component(ctx, this_val, NSURL_PATH); }
static JSValue qjsky_url_get_search(JSContext *ctx, JSValueConst this_val) { return qjsky_url_get_component(ctx, this_val, NSURL_QUERY); }
static JSValue qjsky_url_get_hash(JSContext *ctx, JSValueConst this_val) { return qjsky_url_get_component(ctx, this_val, NSURL_FRAGMENT); }

static JSValue qjsky_url_set_href(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_url_t *url_priv = JS_GetOpaque2(ctx, this_val, heap->url_class_id);
	if (!url_priv) return JS_EXCEPTION;

	const char *url_str = JS_ToCString(ctx, val);
	if (!url_str) return JS_EXCEPTION;

	nsurl *new_url;
	if (nsurl_create(url_str, &new_url) == NSERROR_OK) {
		nsurl_unref(url_priv->url);
		url_priv->url = new_url;
		/* Invalidate searchParams cache */
		if (!JS_IsUndefined(url_priv->search_params)) {
			JS_FreeValue(ctx, url_priv->search_params);
			url_priv->search_params = JS_UNDEFINED;
		}
	}
	JS_FreeCString(ctx, url_str);
	return JS_UNDEFINED;
}

static JSValue qjsky_url_set_protocol(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_url_t *url_priv = JS_GetOpaque2(ctx, this_val, heap->url_class_id);
	if (!url_priv) return JS_EXCEPTION;

	size_t slen;
	const char *s = JS_ToCStringLen(ctx, &slen, val);
	if (!s) return JS_EXCEPTION;
	if (slen > 0 && s[slen-1] == ':') slen--;

	lwc_string *scheme;
	if (lwc_intern_string(s, slen, &scheme) == lwc_error_ok) {
		nsurl *new_url;
		nsurl_replace_scheme(url_priv->url, scheme, &new_url);
		lwc_string_unref(scheme);
		nsurl_unref(url_priv->url);
		url_priv->url = new_url;
	}
	JS_FreeCString(ctx, s);
	return JS_UNDEFINED;
}

static JSValue qjsky_url_set_host(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_url_t *url_priv = JS_GetOpaque2(ctx, this_val, heap->url_class_id);
	if (!url_priv) return JS_EXCEPTION;

	char *path_s = NULL;
	size_t path_l = 0;
	nsurl_get(url_priv->url, NSURL_PATH | NSURL_QUERY | NSURL_FRAGMENT, &path_s, &path_l);

	const char *new_host = JS_ToCString(ctx, val);
	size_t rel_len = strlen(new_host) + (path_s ? path_l : 1) + 3;
	char *rel = malloc(rel_len);
	if (rel) {
		snprintf(rel, rel_len, "//%s%s", new_host, (path_s ? path_s : "/"));
		nsurl *new_url;
		nsurl_join(url_priv->url, rel, &new_url);
		if (new_url) {
			nsurl_unref(url_priv->url);
			url_priv->url = new_url;
		}
		free(rel);
	}
	JS_FreeCString(ctx, new_host);
	free(path_s);
	return JS_UNDEFINED;
}

static JSValue qjsky_url_set_hostname(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_url_t *url_priv = JS_GetOpaque2(ctx, this_val, heap->url_class_id);
	if (!url_priv) return JS_EXCEPTION;

	char *port_s = NULL;
	size_t port_l = 0;
	nsurl_get(url_priv->url, NSURL_PORT, &port_s, &port_l);
	char *path_s = NULL;
	size_t path_l = 0;
	nsurl_get(url_priv->url, NSURL_PATH | NSURL_QUERY | NSURL_FRAGMENT, &path_s, &path_l);

	const char *new_hostname = JS_ToCString(ctx, val);
	size_t rel_len = strlen(new_hostname) + (port_s ? port_l + 1 : 0) + (path_s ? path_l : 1) + 3;
	char *rel = malloc(rel_len);
	if (rel) {
		snprintf(rel, rel_len, "//%s%s%s%s", new_hostname, (port_s ? ":" : ""), (port_s ? port_s : ""), (path_s ? path_s : "/"));
		nsurl *new_url;
		nsurl_join(url_priv->url, rel, &new_url);
		if (new_url) {
			nsurl_unref(url_priv->url);
			url_priv->url = new_url;
		}
		free(rel);
	}
	JS_FreeCString(ctx, new_hostname);
	free(port_s);
	free(path_s);
	return JS_UNDEFINED;
}

static JSValue qjsky_url_set_port(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_url_t *url_priv = JS_GetOpaque2(ctx, this_val, heap->url_class_id);
	if (!url_priv) return JS_EXCEPTION;

	char *host_s = NULL;
	size_t host_l = 0;
	nsurl_get(url_priv->url, NSURL_HOST, &host_s, &host_l);
	char *path_s = NULL;
	size_t path_l = 0;
	nsurl_get(url_priv->url, NSURL_PATH | NSURL_QUERY | NSURL_FRAGMENT, &path_s, &path_l);

	const char *new_port = JS_ToCString(ctx, val);
	size_t rel_len = (host_s ? host_l : 0) + strlen(new_port) + (path_s ? path_l : 1) + 4;
	char *rel = malloc(rel_len);
	if (rel) {
		snprintf(rel, rel_len, "//%s:%s%s", (host_s ? host_s : ""), new_port, (path_s ? path_s : "/"));
		nsurl *new_url;
		nsurl_join(url_priv->url, rel, &new_url);
		if (new_url) {
			nsurl_unref(url_priv->url);
			url_priv->url = new_url;
		}
		free(rel);
	}
	JS_FreeCString(ctx, new_port);
	free(host_s);
	free(path_s);
	return JS_UNDEFINED;
}

static JSValue qjsky_url_set_pathname(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_url_t *url_priv = JS_GetOpaque2(ctx, this_val, heap->url_class_id);
	if (!url_priv) return JS_EXCEPTION;

	char *query_s = NULL;
	size_t query_l = 0;
	nsurl_get(url_priv->url, NSURL_QUERY | NSURL_FRAGMENT, &query_s, &query_l);

	const char *new_path = JS_ToCString(ctx, val);
	size_t rel_len = strlen(new_path) + (query_s ? query_l : 0) + 2;
	char *rel = malloc(rel_len);
	if (rel) {
		if (new_path[0] == '/') {
			snprintf(rel, rel_len, "%s%s", new_path, (query_s ? query_s : ""));
		} else {
			snprintf(rel, rel_len, "/%s%s", new_path, (query_s ? query_s : ""));
		}
		nsurl *new_url;
		nsurl_join(url_priv->url, rel, &new_url);
		if (new_url) {
			nsurl_unref(url_priv->url);
			url_priv->url = new_url;
		}
		free(rel);
	}
	JS_FreeCString(ctx, new_path);
	free(query_s);
	return JS_UNDEFINED;
}

static JSValue qjsky_url_set_search(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_url_t *url_priv = JS_GetOpaque2(ctx, this_val, heap->url_class_id);
	if (!url_priv) return JS_EXCEPTION;

	const char *s = JS_ToCString(ctx, val);
	const char *p = s;
	if (p[0] == '?') p++;

	nsurl *new_url;
	nsurl_replace_query(url_priv->url, p, &new_url);
	if (new_url) {
		nsurl_unref(url_priv->url);
		url_priv->url = new_url;
		/* Invalidate searchParams cache */
		if (!JS_IsUndefined(url_priv->search_params)) {
			JS_FreeValue(ctx, url_priv->search_params);
			url_priv->search_params = JS_UNDEFINED;
		}
	}
	JS_FreeCString(ctx, s);
	return JS_UNDEFINED;
}

static JSValue qjsky_url_set_hash(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_url_t *url_priv = JS_GetOpaque2(ctx, this_val, heap->url_class_id);
	if (!url_priv) return JS_EXCEPTION;

	size_t slen;
	const char *s = JS_ToCStringLen(ctx, &slen, val);
	const char *p = s;
	if (slen > 0 && p[0] == '#') { p++; slen--; }

	lwc_string *frag;
	if (lwc_intern_string(p, slen, &frag) == lwc_error_ok) {
		nsurl *new_url;
		nsurl_refragment(url_priv->url, frag, &new_url);
		lwc_string_unref(frag);
		if (new_url) {
			nsurl_unref(url_priv->url);
			url_priv->url = new_url;
		}
	}
	JS_FreeCString(ctx, s);
	return JS_UNDEFINED;
}

static JSValue qjsky_url_get_searchParams(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_url_t *url_priv = JS_GetOpaque2(ctx, this_val, heap->url_class_id);
	if (!url_priv) return JS_EXCEPTION;

	if (JS_IsUndefined(url_priv->search_params)) {
		char *url_s;
		size_t url_l;
		nsurl_get(url_priv->url, NSURL_QUERY, &url_s, &url_l);
		url_priv->search_params = qjsky_create_urlsearchparams(ctx, url_s ? url_s : "");
		free(url_s);
	}
	return JS_DupValue(ctx, url_priv->search_params);
}

static const JSCFunctionListEntry qjsky_url_proto_funcs[] = {
	JS_CGETSET_DEF("href", qjsky_url_get_href, qjsky_url_set_href),
	JS_CGETSET_DEF("protocol", qjsky_url_get_protocol, qjsky_url_set_protocol),
	JS_CGETSET_DEF("host", qjsky_url_get_host, qjsky_url_set_host),
	JS_CGETSET_DEF("hostname", qjsky_url_get_hostname, qjsky_url_set_hostname),
	JS_CGETSET_DEF("port", qjsky_url_get_port, qjsky_url_set_port),
	JS_CGETSET_DEF("pathname", qjsky_url_get_pathname, qjsky_url_set_pathname),
	JS_CGETSET_DEF("search", qjsky_url_get_search, qjsky_url_set_search),
	JS_CGETSET_DEF("hash", qjsky_url_get_hash, qjsky_url_set_hash),
	JS_CGETSET_DEF("searchParams", qjsky_url_get_searchParams, NULL),
};

void qjsky_init_url(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_url_proto_funcs, sizeof(qjsky_url_proto_funcs)/sizeof(qjsky_url_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->url_class_id, proto);
}

JSValue qjsky_create_url(JSContext *ctx, nsurl *url)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue obj = JS_NewObjectClass(ctx, heap->url_class_id);
	if (JS_IsException(obj)) return obj;

	qjsky_url_t *url_priv = calloc(1, sizeof(*url_priv));
	if (!url_priv) {
		JS_FreeValue(ctx, obj);
		return JS_EXCEPTION;
	}
	url_priv->url = url;
	nsurl_ref(url);
	url_priv->search_params = JS_UNDEFINED;

	JS_SetOpaque(obj, url_priv);
	return obj;
}

JSValue qjsky_url_ctor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv)
{
	if (argc < 1) return JS_EXCEPTION;

	const char *url_s = JS_ToCString(ctx, argv[0]);
	nsurl *base = NULL;
	if (argc > 1 && !JS_IsUndefined(argv[1])) {
		const char *base_s = JS_ToCString(ctx, argv[1]);
		nsurl_create(base_s, &base);
		JS_FreeCString(ctx, base_s);
	}

	nsurl *res_url;
	if (base) {
		nsurl_join(base, url_s, &res_url);
		nsurl_unref(base);
	} else {
		nsurl_create(url_s, &res_url);
	}
	JS_FreeCString(ctx, url_s);

	if (!res_url) return JS_EXCEPTION;

	JSValue obj = qjsky_create_url(ctx, res_url);
	nsurl_unref(res_url);
	return obj;
}
