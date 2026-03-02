#include <quickjs.h>
#include <stdlib.h>
#include <string.h>

#include "utils/log.h"
#include "utils/corestrings.h"
#include "content/handlers/javascript/js.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "content/handlers/javascript/quickjs/qjs_utils.h"
#include "content/handlers/javascript/quickjs/xhr.h"
#include "content/handlers/javascript/quickjs/location.h"
#include "content/handlers/javascript/quickjs/history.h"
#include "content/handlers/javascript/quickjs/navigator.h"
#include "content/content_protected.h"
#include "content/hlcache.h"

#include "quickjs/binding.h"
#include "quickjs/generics.js.inc"
#include "quickjs/polyfill.js.inc"

#include <dom/dom.h>

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
	if (heap == NULL) return NSERROR_NOMEM;

	heap->rt = JS_NewRuntime();
	if (heap->rt == NULL) {
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
	if (heap == NULL) return;

	if (heap->node_map_atom != JS_ATOM_NULL)
		JS_FreeAtomRT(heap->rt, heap->node_map_atom);
	if (heap->handler_map_atom != JS_ATOM_NULL)
		JS_FreeAtomRT(heap->rt, heap->handler_map_atom);
	if (heap->handler_listener_map_atom != JS_ATOM_NULL)
		JS_FreeAtomRT(heap->rt, heap->handler_listener_map_atom);
	if (heap->event_proto_atom != JS_ATOM_NULL)
		JS_FreeAtomRT(heap->rt, heap->event_proto_atom);

	JS_FreeRuntime(heap->rt);
	free(heap);
}

nserror js_newthread(struct jsheap *heap, void *win_priv, void *doc_priv, struct jsthread **thread_out)
{
	struct jsthread *thread = calloc(1, sizeof(*thread));
	if (thread == NULL) return NSERROR_NOMEM;

	thread->ctx = JS_NewContext(heap->rt);
	if (thread->ctx == NULL) {
		free(thread);
		return NSERROR_NOMEM;
	}
	thread->heap = heap;
	thread->win_priv = win_priv;
	thread->doc_priv = doc_priv;

	JS_SetContextOpaque(thread->ctx, thread);

	/* NetSurf-specific builtins */
	qjsky_init_context(thread->ctx);
	qjsky_init_console(thread->ctx);
	qjsky_init_window(thread->ctx);
	qjsky_init_location(thread->ctx);
	qjsky_init_history(thread->ctx);
	qjsky_init_navigator(thread->ctx);
	qjsky_timer_init(thread->ctx);
	qjsky_init_xhr(thread->ctx);

	/* Load polyfill.js */
	JSValue polyfill_val = JS_Eval(thread->ctx, (const char *)polyfill_js, polyfill_js_len, "polyfill.js", JS_EVAL_TYPE_GLOBAL);
	if (JS_IsException(polyfill_val)) {
		qjs_log_exception(thread->ctx, "Failed to load polyfill.js");
		JS_FreeValue(thread->ctx, polyfill_val);
		js_destroythread(thread);
		return NSERROR_INIT_FAILED;
	}
	JS_FreeValue(thread->ctx, polyfill_val);

	/* Load generics.js */
	JSValue generics_val = JS_Eval(thread->ctx, (const char *)generics_js, generics_js_len, "generics.js", JS_EVAL_TYPE_GLOBAL);
	if (JS_IsException(generics_val)) {
		qjs_log_exception(thread->ctx, "Failed to load generics.js");
		JS_FreeValue(thread->ctx, generics_val);
		js_destroythread(thread);
		return NSERROR_INIT_FAILED;
	}
	JS_FreeValue(thread->ctx, generics_val);

	/* Attach global instances */
	JSValue global = JS_GetGlobalObject(thread->ctx);

	/* window.location */
	nsurl *url = NULL;
	if (thread->doc_priv) {
		url = llcache_handle_get_url(((struct content *)thread->doc_priv)->llcache);
	}
	JSValue loc_obj = qjsky_create_location(thread->ctx, url);
	JS_SetPropertyStr(thread->ctx, global, "location", JS_DupValue(thread->ctx, loc_obj));
	/* TODO: document.location */
	JS_FreeValue(thread->ctx, loc_obj);

	/* window.history */
	JSValue hist_obj = qjsky_create_history(thread->ctx);
	JS_SetPropertyStr(thread->ctx, global, "history", hist_obj);

	/* window self-reference */
	JS_SetPropertyStr(thread->ctx, global, "window", JS_DupValue(thread->ctx, global));

	/* window.navigator */
	JSValue nav_obj = qjsky_create_navigator(thread->ctx);
	JS_SetPropertyStr(thread->ctx, global, "navigator", nav_obj);

	JS_FreeValue(thread->ctx, global);

	*thread_out = thread;
	return NSERROR_OK;
}

nserror js_closethread(struct jsthread *thread)
{
	if (thread == NULL) return NSERROR_BAD_PARAMETER;
	qjsky_timer_cleanup(thread->ctx);
	return NSERROR_OK;
}

void js_destroythread(struct jsthread *thread)
{
	if (thread == NULL) return;
	JS_FreeContext(thread->ctx);
	free(thread);
}

static void qjs_run_jobs(JSContext *ctx)
{
	JSContext *ctx1;
	int err;
	for (;;) {
		err = JS_ExecutePendingJob(JS_GetRuntime(ctx), &ctx1);
		if (err <= 0) {
			if (err < 0) qjs_log_exception(ctx1, "Pending job error");
			break;
		}
	}
}

bool js_exec(struct jsthread *thread, const uint8_t *txt, size_t txtlen, const char *name)
{
	JSValue val = JS_Eval(thread->ctx, (const char *)txt, txtlen, name, JS_EVAL_TYPE_GLOBAL);
	if (JS_IsException(val)) {
		qjs_log_exception(thread->ctx, "JS Eval Error");
		return false;
	}
	JS_FreeValue(thread->ctx, val);
	qjs_run_jobs(thread->ctx);
	return true;
}

/* Event Handlers */

bool js_fire_event(struct jsthread *thread, const char *type, struct dom_document *doc, struct dom_node *target)
{
	if (target != NULL) {
		/* General event firing logic */
		JSValue node_val = qjsky_push_node(thread->ctx, target);
		JSValue global = JS_GetGlobalObject(thread->ctx);
		JSValue event_ctor = JS_GetPropertyStr(thread->ctx, global, "Event");

		JSValue type_val = JS_NewString(thread->ctx, type);
		JSValue event_obj = JS_CallConstructor(thread->ctx, event_ctor, 1, &type_val);

		JSValue dispatch_fn = JS_GetPropertyStr(thread->ctx, node_val, "dispatchEvent");
		if (JS_IsFunction(thread->ctx, dispatch_fn)) {
			JSValue ret = JS_Call(thread->ctx, dispatch_fn, node_val, 1, &event_obj);
			JS_FreeValue(thread->ctx, ret);
		}

		JS_FreeValue(thread->ctx, dispatch_fn);
		JS_FreeValue(thread->ctx, event_obj);
		JS_FreeValue(thread->ctx, type_val);
		JS_FreeValue(thread->ctx, event_ctor);
		JS_FreeValue(thread->ctx, global);
		JS_FreeValue(thread->ctx, node_val);

		return true;
	}

	if (strcmp(type, "load") == 0) {
		/* Special case for Window.onload */
		/* Simple dispatch for now */
	}

	return true;
}

void js_handle_new_element(struct jsthread *thread, struct dom_element *node)
{
	dom_namednodemap *map;
	dom_exception exc;
	dom_ulong idx, siz;
	dom_attr *attr = NULL;
	dom_string *key = NULL;

	exc = dom_node_get_attributes(node, &map);
	if (exc != DOM_NO_ERR || map == NULL) return;

	exc = dom_namednodemap_get_length(map, &siz);
	if (exc != DOM_NO_ERR) {
		dom_namednodemap_unref(map);
		return;
	}

	for (idx = 0; idx < siz; idx++) {
		exc = dom_namednodemap_item(map, idx, &attr);
		if (exc != DOM_NO_ERR) break;

		exc = dom_attr_get_name(attr, &key);
		if (exc != DOM_NO_ERR) {
			dom_node_unref(attr);
			break;
		}

		if (dom_string_length(key) > 2) {
			const uint8_t *data = (const uint8_t *)dom_string_data(key);
			if (data[0] == 'o' && data[1] == 'n') {
				dom_string *sub = NULL;
				dom_string *val = NULL;
				dom_string_substr(key, 2, dom_string_length(key) - 2, &sub);
				dom_element_get_attribute(node, key, &val);
				if (sub && val) {
					qjsky_register_event_listener_for(thread->ctx, node, sub, val, false);
				}
				if (sub) dom_string_unref(sub);
				if (val) dom_string_unref(val);
			}
		}

		dom_string_unref(key);
		dom_node_unref(attr);
	}
	dom_namednodemap_unref(map);
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
