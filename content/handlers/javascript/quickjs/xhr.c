#include <stdlib.h>
#include <string.h>
#include <quickjs.h>
#include "content/handlers/javascript/quickjs/xhr.h"
#include "content/handlers/javascript/quickjs/qjs_utils.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "content/fetch.h"
#include "utils/nsurl.h"
#include "utils/log.h"

typedef struct qjsky_xhr_s {
	JSContext *ctx;
	int ready_state;
	int status;
	char *status_text;
	char *response_text;
	struct fetch *fetch;
} qjsky_xhr_t;

static JSClassID qjsky_xhr_class_id = 0;

static void qjsky_xhr_finalizer(JSRuntime *rt, JSValue val)
{
	qjsky_xhr_t *xhr = JS_GetOpaque(val, qjsky_xhr_class_id);
	if (xhr) {
		if (xhr->fetch) fetch_free(xhr->fetch);
		free(xhr->status_text);
		free(xhr->response_text);
		free(xhr);
	}
}

static JSClassDef qjsky_xhr_class = {
	"XMLHttpRequest",
	.finalizer = qjsky_xhr_finalizer,
};

static JSValue qjsky_xhr_ctor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv)
{
	JSValue obj = JS_NewObjectProtoClass(ctx, new_target, qjsky_xhr_class_id);
	if (JS_IsException(obj)) return obj;

	qjsky_xhr_t *xhr = calloc(1, sizeof(*xhr));
	if (!xhr) {
		JS_FreeValue(ctx, obj);
		return JS_EXCEPTION;
	}
	xhr->ctx = ctx;
	xhr->ready_state = 0; /* UNSENT */

	JS_SetOpaque(obj, xhr);

	return obj;
}

static JSValue qjsky_xhr_open(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	qjsky_xhr_t *xhr = JS_GetOpaque2(ctx, this_val, qjsky_xhr_class_id);
	if (!xhr) return JS_EXCEPTION;

	if (argc < 2) return JS_EXCEPTION;

	/* TODO: Implementation */
	xhr->ready_state = 1; /* OPENED */

	return JS_UNDEFINED;
}

static JSValue qjsky_xhr_send(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
	qjsky_xhr_t *xhr = JS_GetOpaque2(ctx, this_val, qjsky_xhr_class_id);
	if (!xhr) return JS_EXCEPTION;

	/* TODO: Trigger asynchronous fetch */

	return JS_UNDEFINED;
}

static JSValue qjsky_xhr_get_readyState(JSContext *ctx, JSValueConst this_val)
{
	qjsky_xhr_t *xhr = JS_GetOpaque2(ctx, this_val, qjsky_xhr_class_id);
	if (!xhr) return JS_EXCEPTION;
	return JS_NewInt32(ctx, xhr->ready_state);
}

static const JSCFunctionListEntry qjsky_xhr_proto_funcs[] = {
	JS_CFUNC_DEF("open", 2, qjsky_xhr_open),
	JS_CFUNC_DEF("send", 1, qjsky_xhr_send),
	JS_CGETSET_DEF("readyState", qjsky_xhr_get_readyState, NULL),
};

void qjsky_init_xhr(JSContext *ctx)
{
	if (qjsky_xhr_class_id == 0) {
		JS_NewClassID(&qjsky_xhr_class_id);
	}
	/* Ensure class is registered for this runtime */
	JS_NewClass(JS_GetRuntime(ctx), qjsky_xhr_class_id, &qjsky_xhr_class);

	JSValue global = JS_GetGlobalObject(ctx);

	JSValue proto = JS_NewObject(ctx);
	JS_SetPropertyFunctionList(ctx, proto, qjsky_xhr_proto_funcs, sizeof(qjsky_xhr_proto_funcs)/sizeof(qjsky_xhr_proto_funcs[0]));
	JS_SetClassProto(ctx, qjsky_xhr_class_id, JS_DupValue(ctx, proto));

	JSValue ctor = JS_NewCFunction2(ctx, qjsky_xhr_ctor, "XMLHttpRequest", 0, JS_CFUNC_constructor, 0);
	JS_SetConstructor(ctx, ctor, proto);
	JS_SetPropertyStr(ctx, global, "XMLHttpRequest", ctor);

	JS_FreeValue(ctx, proto);
	JS_FreeValue(ctx, global);
}
