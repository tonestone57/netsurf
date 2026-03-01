#include <quickjs.h>
#include <stdlib.h>
#include <string.h>

#include "utils/log.h"
#include "content/handlers/javascript/js.h"
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "content/handlers/javascript/quickjs/xhr.h"

struct jsheap {
	JSRuntime *rt;
	JSClassID node_class_id;
	JSAtom node_map_atom;
    void *timer_ring; /* Managed in qjsky.c but logically part of heap */
};

struct jsctx {
	JSContext *ctx;
	struct jsheap *heap;
};

/* Global for simple class ID management if needed, but jsheap is better */
static JSClassID global_node_class_id = 0;

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

	qjsky_init_runtime(heap);

	return heap;
}

void js_destroyheap(struct jsheap *heap)
{
	if (!heap) return;
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

bool js_exec(struct jsctx *ctx, const char *script, size_t len, const char *filename)
{
	JSValue val = JS_Eval(ctx->ctx, script, len, filename, JS_EVAL_TYPE_GLOBAL);
	if (JS_IsException(val)) {
		JSValue exception = JS_GetException(ctx->ctx);
		const char *str = JS_ToCString(ctx->ctx, exception);
		if (str) {
			NSLOG(jserrors, ERROR, "JS Error: %s", str);
			JS_FreeCString(ctx->ctx, str);
		}
		JS_FreeValue(ctx->ctx, exception);
		return false;
	}
	JS_FreeValue(ctx->ctx, val);
	return true;
}

/* Event Handlers (Skeletons) */

bool js_fire_event(struct jsctx *ctx, const char *type, struct dom_node *target)
{
	/* TODO: Implement event object construction and dispatch */
	return true;
}

void js_handle_new_element(struct jsctx *ctx, struct dom_node *node, const char *attr, const char *value)
{
	/* TODO: Compile and attach inline event handlers */
}
