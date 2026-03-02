#include <stdlib.h>
#include <string.h>
#include <quickjs.h>
#include "content/handlers/javascript/quickjs/xhr.h"
#include "content/handlers/javascript/quickjs/qjs_utils.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "content/fetch.h"
#include "content/content_protected.h"
#include "content/hlcache.h"
#include "utils/nsurl.h"
#include "utils/log.h"
#include "utils/messages.h"

typedef struct qjsky_xhr_s {
	JSContext *ctx;
	JSValue xhr_obj;
	int ready_state;
	int status;
	char *status_text;
	char *response_text;
	nsurl *url;
	char *method;
	struct fetch *fetch;
} qjsky_xhr_t;

static void qjsky_xhr_set_status(qjsky_xhr_t *xhr)
{
	xhr->status = fetch_http_code(xhr->fetch);
	char key[16];
	snprintf(key, sizeof(key), "HTTP%03d", xhr->status);
	const char *msg_str = messages_get(key);
	free(xhr->status_text);
	xhr->status_text = strdup(msg_str);
}

static void qjsky_xhr_on_state_change(qjsky_xhr_t *xhr, JSValueConst this_val)
{
	/* 1. Call onreadystatechange handler */
	JSValue onready = JS_GetPropertyStr(xhr->ctx, this_val, "__onreadystatechange");
	if (JS_IsFunction(xhr->ctx, onready)) {
		JSValue ret = JS_Call(xhr->ctx, onready, this_val, 0, NULL);
		if (JS_IsException(ret)) qjs_log_exception(xhr->ctx, "XHR onreadystatechange error");
		JS_FreeValue(xhr->ctx, ret);
	}
	JS_FreeValue(xhr->ctx, onready);

	/* 2. Dispatch readystatechange event */
	JSValue global = JS_GetGlobalObject(xhr->ctx);
	JSValue event_ctor = JS_GetPropertyStr(xhr->ctx, global, "Event");
	if (JS_IsFunction(xhr->ctx, event_ctor)) {
		JSValue type_val = JS_NewString(xhr->ctx, "readystatechange");
		JSValue event_obj = JS_CallConstructor(xhr->ctx, event_ctor, 1, &type_val);

		JSValue dispatch_fn = JS_GetPropertyStr(xhr->ctx, this_val, "dispatchEvent");
		if (JS_IsFunction(xhr->ctx, dispatch_fn)) {
			JSValue ret = JS_Call(xhr->ctx, dispatch_fn, this_val, 1, &event_obj);
			JS_FreeValue(xhr->ctx, ret);
		}

		JS_FreeValue(xhr->ctx, dispatch_fn);
		JS_FreeValue(xhr->ctx, event_obj);
		JS_FreeValue(xhr->ctx, type_val);
	}
	JS_FreeValue(xhr->ctx, event_ctor);
	JS_FreeValue(xhr->ctx, global);

	/* Run pending jobs */
	JSContext *ctx1;
	while (JS_ExecutePendingJob(JS_GetRuntime(xhr->ctx), &ctx1) > 0);
}

static void qjsky_xhr_fetch_callback(const fetch_msg *msg, void *p)
{
	qjsky_xhr_t *xhr = p;
	switch (msg->type) {
	case FETCH_HEADER:
		if (xhr->ready_state < 2) {
			qjsky_xhr_set_status(xhr);
			xhr->ready_state = 2; /* HEADERS_RECEIVED */
			qjsky_xhr_on_state_change(xhr, xhr->xhr_obj);
		}
		break;
	case FETCH_DATA:
		{
			size_t old_len = xhr->response_text ? strlen(xhr->response_text) : 0;
			char *new_text = realloc(xhr->response_text, old_len + msg->data.header_or_data.len + 1);
			if (new_text) {
				xhr->response_text = new_text;
				memcpy(xhr->response_text + old_len, msg->data.header_or_data.buf, msg->data.header_or_data.len);
				xhr->response_text[old_len + msg->data.header_or_data.len] = '\0';
			}

			if (xhr->ready_state < 3) {
				xhr->ready_state = 3; /* LOADING */
				qjsky_xhr_on_state_change(xhr, xhr->xhr_obj);
			}
		}
		break;
	case FETCH_FINISHED:
		qjsky_xhr_set_status(xhr);
		xhr->ready_state = 4; /* DONE */
		qjsky_xhr_on_state_change(xhr, xhr->xhr_obj);

		/* Cleanup flight state */
		JSValue obj = xhr->xhr_obj;
		xhr->xhr_obj = JS_UNDEFINED;
		JS_FreeValue(xhr->ctx, obj);
		break;
	case FETCH_ERROR:
		xhr->ready_state = 4; /* DONE */
		qjsky_xhr_on_state_change(xhr, xhr->xhr_obj);

		JSValue obj_err = xhr->xhr_obj;
		xhr->xhr_obj = JS_UNDEFINED;
		JS_FreeValue(xhr->ctx, obj_err);
		break;
	default:
		break;
	}
}

static void qjsky_xhr_finalizer(JSRuntime *rt, JSValue val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(rt);
	qjsky_xhr_t *xhr = JS_GetOpaque(val, heap->xhr_class_id);
	if (xhr) {
		if (xhr->fetch) fetch_free(xhr->fetch);
		free(xhr->status_text);
		free(xhr->response_text);
		if (xhr->url) nsurl_unref(xhr->url);
		free(xhr->method);
		free(xhr);
	}
}

JSClassDef qjsky_xhr_class = {
	"XMLHttpRequest",
	.finalizer = qjsky_xhr_finalizer,
};

static JSValue qjsky_xhr_ctor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	JSValue obj = JS_NewObjectProtoClass(ctx, new_target, heap->xhr_class_id);
	if (JS_IsException(obj)) return obj;

	qjsky_xhr_t *xhr = calloc(1, sizeof(*xhr));
	if (!xhr) {
		JS_FreeValue(ctx, obj);
		return JS_EXCEPTION;
	}
	xhr->ctx = ctx;
	xhr->xhr_obj = JS_UNDEFINED;
	xhr->ready_state = 0; /* UNSENT */

	JS_SetOpaque(obj, xhr);

	return obj;
}

static JSValue qjsky_xhr_open(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_xhr_t *xhr = JS_GetOpaque2(ctx, this_val, heap->xhr_class_id);
	if (!xhr) return JS_EXCEPTION;

	if (argc < 2) return JS_EXCEPTION;

	const char *method = JS_ToCString(ctx, argv[0]);
	const char *url_str = JS_ToCString(ctx, argv[1]);

	if (!method || !url_str) {
		JS_FreeCString(ctx, method);
		JS_FreeCString(ctx, url_str);
		return JS_EXCEPTION;
	}

	free(xhr->method);
	xhr->method = strdup(method);

	if (xhr->url) nsurl_unref(xhr->url);

	struct jsthread *thread = JS_GetContextOpaque(ctx);
	nsurl *base = NULL;
	if (thread && thread->doc_priv) {
		base = llcache_handle_get_url(((struct content *)thread->doc_priv)->llcache);
	}

	nsurl_join(base, url_str, &xhr->url);

	JS_FreeCString(ctx, method);
	JS_FreeCString(ctx, url_str);

	xhr->ready_state = 1; /* OPENED */
	qjsky_xhr_on_state_change(xhr, this_val);

	return JS_UNDEFINED;
}

static JSValue qjsky_xhr_send(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_xhr_t *xhr = JS_GetOpaque2(ctx, this_val, heap->xhr_class_id);
	if (!xhr) return JS_EXCEPTION;

	if (xhr->ready_state != 1 || xhr->url == NULL) return JS_EXCEPTION;

	xhr->xhr_obj = JS_DupValue(ctx, this_val);

	nserror err = fetch_start(xhr->url, NULL, qjsky_xhr_fetch_callback, xhr,
				  false, NULL, NULL, false, false, NULL, &xhr->fetch);

	if (err != NSERROR_OK) {
		JS_FreeValue(ctx, xhr->xhr_obj);
		xhr->xhr_obj = JS_UNDEFINED;
		return JS_EXCEPTION;
	}

	return JS_UNDEFINED;
}

static JSValue qjsky_xhr_get_readyState(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_xhr_t *xhr = JS_GetOpaque2(ctx, this_val, heap->xhr_class_id);
	if (!xhr) return JS_EXCEPTION;
	return JS_NewInt32(ctx, xhr->ready_state);
}

static JSValue qjsky_xhr_get_status(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_xhr_t *xhr = JS_GetOpaque2(ctx, this_val, heap->xhr_class_id);
	if (!xhr) return JS_EXCEPTION;
	return JS_NewInt32(ctx, xhr->status);
}

static JSValue qjsky_xhr_get_statusText(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_xhr_t *xhr = JS_GetOpaque2(ctx, this_val, heap->xhr_class_id);
	if (!xhr) return JS_EXCEPTION;
	return JS_NewString(ctx, xhr->status_text ? xhr->status_text : "");
}

static JSValue qjsky_xhr_get_responseText(JSContext *ctx, JSValueConst this_val)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));
	qjsky_xhr_t *xhr = JS_GetOpaque2(ctx, this_val, heap->xhr_class_id);
	if (!xhr) return JS_EXCEPTION;
	return JS_NewString(ctx, xhr->response_text ? xhr->response_text : "");
}

static JSValue qjsky_xhr_get_onreadystatechange(JSContext *ctx, JSValueConst this_val)
{
	return JS_GetPropertyStr(ctx, this_val, "__onreadystatechange");
}

static JSValue qjsky_xhr_set_onreadystatechange(JSContext *ctx, JSValueConst this_val, JSValueConst val)
{
	JS_SetPropertyStr(ctx, this_val, "__onreadystatechange", JS_DupValue(ctx, val));
	return JS_UNDEFINED;
}

static const JSCFunctionListEntry qjsky_xhr_proto_funcs[] = {
	JS_CFUNC_DEF("open", 2, qjsky_xhr_open),
	JS_CFUNC_DEF("send", 1, qjsky_xhr_send),
	JS_CGETSET_DEF("readyState", qjsky_xhr_get_readyState, NULL),
	JS_CGETSET_DEF("status", qjsky_xhr_get_status, NULL),
	JS_CGETSET_DEF("statusText", qjsky_xhr_get_statusText, NULL),
	JS_CGETSET_DEF("responseText", qjsky_xhr_get_responseText, NULL),
	JS_CGETSET_DEF("onreadystatechange", qjsky_xhr_get_onreadystatechange, qjsky_xhr_set_onreadystatechange),
};

void qjsky_init_xhr(JSContext *ctx)
{
	struct jsheap *heap = JS_GetRuntimeOpaque(JS_GetRuntime(ctx));

	JSValue global = JS_GetGlobalObject(ctx);

	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_xhr_proto_funcs, sizeof(qjsky_xhr_proto_funcs)/sizeof(qjsky_xhr_proto_funcs[0]));
	JS_SetClassProto(ctx, heap->xhr_class_id, JS_DupValue(ctx, proto));

	JSValue ctor = JS_NewCFunction2(ctx, qjsky_xhr_ctor, "XMLHttpRequest", 0, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, ctor, proto);

	/* Constants */
	JS_DefinePropertyValueStr(ctx, ctor, "UNSENT", JS_NewInt32(ctx, 0), JS_PROP_ENUMERABLE);
	JS_DefinePropertyValueStr(ctx, ctor, "OPENED", JS_NewInt32(ctx, 1), JS_PROP_ENUMERABLE);
	JS_DefinePropertyValueStr(ctx, ctor, "HEADERS_RECEIVED", JS_NewInt32(ctx, 2), JS_PROP_ENUMERABLE);
	JS_DefinePropertyValueStr(ctx, ctor, "LOADING", JS_NewInt32(ctx, 3), JS_PROP_ENUMERABLE);
	JS_DefinePropertyValueStr(ctx, ctor, "DONE", JS_NewInt32(ctx, 4), JS_PROP_ENUMERABLE);

	JS_SetPropertyStr(ctx, global, "XMLHttpRequest", ctor);

	JS_FreeValue(ctx, proto);
	JS_FreeValue(ctx, global);
}
