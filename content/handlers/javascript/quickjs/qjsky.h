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

/* Map a libdom node to a QuickJS object */
JSValue qjsky_push_node(JSContext *ctx, struct dom_node *node);

/* Retrieve a libdom node from a QuickJS object */
struct dom_node *qjsky_get_node(JSContext *ctx, JSValue val);

#endif
