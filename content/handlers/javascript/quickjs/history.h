#ifndef NETSURF_QUICKJS_HISTORY_H
#define NETSURF_QUICKJS_HISTORY_H

#include <quickjs.h>

/* Initialise the History class in a context */
void qjsky_init_history(JSContext *ctx);

/* Create a new History object */
JSValue qjsky_create_history(JSContext *ctx);

#endif
