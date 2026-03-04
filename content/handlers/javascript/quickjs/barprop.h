#ifndef NETSURF_QUICKJS_BARPROP_H
#define NETSURF_QUICKJS_BARPROP_H

#include <quickjs.h>

/* Initialise the BarProp class in a context */
void qjsky_init_barprop(JSContext *ctx);

/* Create a new BarProp object */
JSValue qjsky_create_barprop(JSContext *ctx);

#endif
