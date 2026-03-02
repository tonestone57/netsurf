#ifndef NETSURF_QUICKJS_QJSKY_H
#define NETSURF_QUICKJS_QJSKY_H

#include <quickjs.h>
#include <dom/dom.h>

struct jsheap;

/* Per-runtime initialization */
void qjsky_init_runtime(struct jsheap *heap);

/* Per-context initialization */
void qjsky_init_context(JSContext *ctx);

/* Node conversion and memoization */
JSValue qjsky_push_node(JSContext *ctx, struct dom_node *node);
struct dom_node *qjsky_get_node(JSContext *ctx, JSValue val);

/* String Conversion Helpers */
dom_string *qjsky_js_value_to_dom_string(JSContext *ctx, JSValue val);
JSValue qjsky_dom_string_to_js_value(JSContext *ctx, dom_string *str);

/* Console Integration */
void qjsky_init_console(JSContext *ctx);

/* Window Integration */
void qjsky_init_window(JSContext *ctx);

/* Timer Support */
void qjsky_timer_init(JSContext *ctx);
void qjsky_timer_cleanup(JSContext *ctx);

/* Event Support */
void qjsky_register_event_listener_for(JSContext *ctx, struct dom_element *ele, dom_string *name, dom_string *value, bool capture);
JSValue qjsky_push_event(JSContext *ctx, dom_event *evt);

#endif
