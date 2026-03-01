#include <quickjs.h>
#include <stdlib.h>
#include <string.h>

#include "utils/log.h"
#include "content/handlers/javascript/js.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "content/handlers/javascript/quickjs/xhr.h"

struct jsctx {
	JSContext *ctx;
	struct jsheap *heap;
};

struct jsheap *js_newheap(void)
{
	struct jsheap *heap = calloc(1, sizeof(*heap));
	if (!heap) return NULL;

	heap->rt = JS_NewRuntime();
	if (!heap->rt) {
		free(heap);
		return NULL;
	}

	/* Limit memory usage (e.g., 64MB) */
	JS_SetMemoryLimit(heap->rt, 64 * 1024 * 1024);
	JS_SetMaxStackSize(heap->rt, 1024 * 1024);

	heap->next_timer_handle = 1;

	qjsky_init_runtime(heap);

	return heap;
}

void js_destroyheap(struct jsheap *heap)
{
	if (!heap) return;
	/* Atoms are freed automatically when the runtime is destroyed,
	   but we should be clean if we were to reuse it. */
	if (heap->node_map_atom != JS_ATOM_NULL)
		JS_FreeAtomRT(heap->rt, heap->node_map_atom);
	JS_FreeRuntime(heap->rt);
	free(heap);
}

struct jsctx *js_newctx(struct jsheap *heap)
{
	struct jsctx *ctx = calloc(1, sizeof(*ctx));
	if (!ctx) return NULL;

	ctx->ctx = JS_NewContext(heap->rt);
	if (!ctx->ctx) {
		free(ctx);
		return NULL;
	}
	ctx->heap = heap;

	/* NetSurf-specific builtins */
	qjsky_init_context(ctx->ctx);
	qjsky_init_console(ctx->ctx);
	qjsky_timer_init(ctx->ctx);
	qjsky_init_xhr(ctx->ctx);

	return ctx;
}

void js_destroyctx(struct jsctx *ctx)
{
	if (!ctx) return;
	qjsky_timer_cleanup(ctx->ctx);
	JS_FreeContext(ctx->ctx);
	free(ctx);
}

static void qjs_log_exception(JSContext *ctx)
{
	JSValue exception = JS_GetException(ctx);
	const char *str = JS_ToCString(ctx, exception);
	if (str) {
		NSLOG(jserrors, ERROR, "JS Error: %s", str);
		JS_FreeCString(ctx, str);
	}
	JS_FreeValue(ctx, exception);
}

bool js_exec(struct jsctx *ctx, const char *script, size_t len, const char *filename)
{
	JSValue val = JS_Eval(ctx->ctx, script, len, filename, JS_EVAL_TYPE_GLOBAL);
	if (JS_IsException(val)) {
		qjs_log_exception(ctx->ctx);
		return false;
	}
	JS_FreeValue(ctx->ctx, val);
	return true;
}

/* Event Handlers */

bool js_fire_event(struct jsctx *ctx, const char *type, struct dom_node *target)
{
	JSValue global = JS_GetGlobalObject(ctx->ctx);
	JSValue event_ctor = JS_GetPropertyStr(ctx->ctx, global, "Event");

	if (JS_IsException(event_ctor) || JS_IsUndefined(event_ctor)) {
		if (JS_IsException(event_ctor)) qjs_log_exception(ctx->ctx);
		JS_FreeValue(ctx->ctx, event_ctor);
		JS_FreeValue(ctx->ctx, global);
		return false;
	}

	/* Construct Event object */
	JSValue type_val = JS_NewString(ctx->ctx, type);
	JSValue event_obj = JS_CallConstructor(ctx->ctx, event_ctor, 1, &type_val);
	JS_FreeValue(ctx->ctx, type_val);
	JS_FreeValue(ctx->ctx, event_ctor);

	if (JS_IsException(event_obj)) {
		qjs_log_exception(ctx->ctx);
		JS_FreeValue(ctx->ctx, event_obj);
		JS_FreeValue(ctx->ctx, global);
		return false;
	}

	/* Wrap target node */
	JSValue target_val = qjsky_push_node(ctx->ctx, target);
	if (JS_IsException(target_val)) {
		qjs_log_exception(ctx->ctx);
		JS_FreeValue(ctx->ctx, target_val);
		JS_FreeValue(ctx->ctx, event_obj);
		JS_FreeValue(ctx->ctx, global);
		return false;
	}

	/* Dispatch event: target.dispatchEvent(event) */
	JSValue dispatch_fn = JS_GetPropertyStr(ctx->ctx, target_val, "dispatchEvent");
	if (JS_IsFunction(ctx->ctx, dispatch_fn)) {
		JSValue ret = JS_Call(ctx->ctx, dispatch_fn, target_val, 1, &event_obj);
		if (JS_IsException(ret)) qjs_log_exception(ctx->ctx);
		JS_FreeValue(ctx->ctx, ret);
	}

	JS_FreeValue(ctx->ctx, dispatch_fn);
	JS_FreeValue(ctx->ctx, target_val);
	JS_FreeValue(ctx->ctx, event_obj);
	JS_FreeValue(ctx->ctx, global);
	return true;
}

void js_handle_new_element(struct jsctx *ctx, struct dom_node *node, const char *attr, const char *value)
{
	/* Compile and attach inline event handlers (e.g., onclick) */
	/* Skeleton implementation for future binding integration:
	   1. Check if 'attr' is an event handler (starts with 'on').
	   2. Create a JS function: "function(event) { " + value + " }"
	   3. Assign it to the corresponding property on the JS object for 'node'.
	*/
	if (strncmp(attr, "on", 2) == 0) {
		JSValue node_obj = qjsky_push_node(ctx->ctx, node);
		if (JS_IsException(node_obj)) {
			qjs_log_exception(ctx->ctx);
		} else {
			/* Implementation details pending full binding support */
		}
		JS_FreeValue(ctx->ctx, node_obj);
	}
}
