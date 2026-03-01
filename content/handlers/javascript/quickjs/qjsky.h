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

/* Initialise the console object in a context */
void qjsky_init_console(JSContext *ctx);

/* Map a libdom node to a QuickJS object */
JSValue qjsky_push_node(JSContext *ctx, struct dom_node *node);

/* Retrieve a libdom node from a QuickJS object */
struct dom_node *qjsky_get_node(JSContext *ctx, JSValue val);

#endif
