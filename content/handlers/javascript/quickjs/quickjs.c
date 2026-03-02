#include <quickjs.h>
#include <stdlib.h>
#include <string.h>

#include "utils/log.h"
#include "content/handlers/javascript/js.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "content/handlers/javascript/quickjs/qjs_utils.h"
#include "content/handlers/javascript/quickjs/xhr.h"

struct jsthread {
	JSContext *ctx;
	struct jsheap *heap;
};

void js_initialise(void)
{
	/* Global initialization if needed */
}

void js_finalise(void)
{
	/* Global cleanup if needed */
}

nserror js_newheap(int timeout, struct jsheap **heap_out)
{
	struct jsheap *heap = calloc(1, sizeof(*heap));
	if (!heap) return NSERROR_NOMEM;

	heap->rt = JS_NewRuntime();
	if (!heap->rt) {
		free(heap);
		return NSERROR_NOMEM;
	}

	/* Limit memory usage (e.g., 64MB) */
	JS_SetMemoryLimit(heap->rt, 64 * 1024 * 1024);
	JS_SetMaxStackSize(heap->rt, 1024 * 1024);

	heap->next_timer_handle = 1;

	qjsky_init_runtime(heap);

	*heap_out = heap;
	return NSERROR_OK;
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

nserror js_newthread(struct jsheap *heap, void *win_priv, void *doc_priv, struct jsthread **thread_out)
{
	struct jsthread *thread = calloc(1, sizeof(*thread));
	if (!thread) return NSERROR_NOMEM;

	thread->ctx = JS_NewContext(heap->rt);
	if (!thread->ctx) {
		free(thread);
		return NSERROR_NOMEM;
	}
	thread->heap = heap;

	/* NetSurf-specific builtins */
	qjsky_init_context(thread->ctx);
	qjsky_init_console(thread->ctx);
	qjsky_timer_init(thread->ctx);
	qjsky_init_xhr(thread->ctx);

	*thread_out = thread;
	return NSERROR_OK;
}

nserror js_closethread(struct jsthread *thread)
{
	if (!thread) return NSERROR_BAD_PARAMETER;
	qjsky_timer_cleanup(thread->ctx);
	return NSERROR_OK;
}

void js_destroythread(struct jsthread *thread)
{
	if (!thread) return;
	JS_FreeContext(thread->ctx);
	free(thread);
}

bool js_exec(struct jsthread *thread, const uint8_t *txt, size_t txtlen, const char *name)
{
	JSValue val = JS_Eval(thread->ctx, (const char *)txt, txtlen, name, JS_EVAL_TYPE_GLOBAL);
	if (JS_IsException(val)) {
		qjs_log_exception(thread->ctx, "JS Eval Error");
		return false;
	}
	JS_FreeValue(thread->ctx, val);
	return true;
}

/* Event Handlers */

bool js_fire_event(struct jsthread *thread, const char *type, struct dom_document *doc, struct dom_node *target)
{
	JSValue global = JS_GetGlobalObject(thread->ctx);
	JSValue event_ctor = JS_GetPropertyStr(thread->ctx, global, "Event");

	if (JS_IsException(event_ctor) || JS_IsUndefined(event_ctor)) {
		if (JS_IsException(event_ctor)) qjs_log_exception(thread->ctx, "Event ctor access error");
		JS_FreeValue(thread->ctx, event_ctor);
		JS_FreeValue(thread->ctx, global);
		return false;
	}

	/* Construct Event object */
	JSValue type_val = JS_NewString(thread->ctx, type);
	JSValue event_obj = JS_CallConstructor(thread->ctx, event_ctor, 1, &type_val);
	JS_FreeValue(thread->ctx, type_val);
	JS_FreeValue(thread->ctx, event_ctor);

	if (JS_IsException(event_obj)) {
		qjs_log_exception(thread->ctx, "Event construction error");
		JS_FreeValue(thread->ctx, event_obj);
		JS_FreeValue(thread->ctx, global);
		return false;
	}

	/* Wrap target node */
	JSValue target_val = qjsky_push_node(thread->ctx, target);
	if (JS_IsException(target_val)) {
		qjs_log_exception(thread->ctx, "Node wrapping error");
		JS_FreeValue(thread->ctx, target_val);
		JS_FreeValue(thread->ctx, event_obj);
		JS_FreeValue(thread->ctx, global);
		return false;
	}

	/* Dispatch event: target.dispatchEvent(event) */
	JSValue dispatch_fn = JS_GetPropertyStr(thread->ctx, target_val, "dispatchEvent");
	if (JS_IsFunction(thread->ctx, dispatch_fn)) {
		JSValue ret = JS_Call(thread->ctx, dispatch_fn, target_val, 1, &event_obj);
		if (JS_IsException(ret)) qjs_log_exception(thread->ctx, "Event dispatch error");
		JS_FreeValue(thread->ctx, ret);
	}

	JS_FreeValue(thread->ctx, dispatch_fn);
	JS_FreeValue(thread->ctx, target_val);
	JS_FreeValue(thread->ctx, event_obj);
	JS_FreeValue(thread->ctx, global);
	return true;
}

void js_handle_new_element(struct jsthread *thread, struct dom_element *node)
{
	/* Compile and attach inline event handlers (e.g., onclick) */
	/* Note: In a full implementation, we'd scan for all on* attributes */
}

void js_event_cleanup(struct jsthread *thread, struct dom_event *evt)
{
	/* Implementation of cleanup if needed */
}

bool
js_dom_event_add_listener(jsthread *thread,
			  struct dom_document *document,
			  struct dom_node *node,
			  struct dom_string *event_type_dom,
			  void *js_funcval)
{
	/* Bridge for manually adding event listeners from the core */
	return false;
}
