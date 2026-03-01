#ifndef NETSURF_QUICKJS_QJSKY_H
#define NETSURF_QUICKJS_QJSKY_H

#include <quickjs.h>
#include <dom/dom.h>

/**
 * qjsky javascript heap
 */
typedef struct jsheap jsheap;

/**
 * qjsky javascript thread
 */
typedef struct jsthread jsthread;

/* Initialise a QuickJS runtime for NetSurf support */
void qjsky_init_runtime(JSRuntime *rt);

/* Map a libdom node to a QuickJS object */
JSValue qjsky_push_node(JSContext *ctx, struct dom_node *node);

/* Retrieve a libdom node from a QuickJS object */
struct dom_node *qjsky_get_node(JSContext *ctx, JSValue val);

/* Initialise Console API */
void qjsky_init_console(JSContext *ctx);

/* Initialise Window aliases (self, window, top) */
void qjsky_init_window_aliases(JSContext *ctx);

/* Initialise Performance API */
void qjsky_init_performance(JSContext *ctx);

/* Initialise Navigator API */
void qjsky_init_navigator(JSContext *ctx);

/* Setup Node prototype with common properties */
void qjsky_setup_node_prototype(JSContext *ctx, JSValue proto);

/* Initialise Location API */
void qjsky_init_location(JSContext *ctx);

/* Initialise History API */
void qjsky_init_history(JSContext *ctx);

/* Initialise XMLHttpRequest API */
void qjsky_init_xhr(JSContext *ctx);

/* Setup Document prototype with common properties */
void qjsky_setup_document_prototype(JSContext *ctx, JSValue proto);

/* Setup Element prototype with common properties */
void qjsky_setup_element_prototype(JSContext *ctx, JSValue proto);

/* Initialise Timer API */
void qjsky_init_timers(JSContext *ctx);

/* Setup EventTarget prototype with common properties */
void qjsky_setup_event_target_prototype(JSContext *ctx, JSValue proto);

/* Setup Element Attribute accessors */
void qjsky_setup_element_attr_prototype(JSContext *ctx, JSValue proto);

#endif
