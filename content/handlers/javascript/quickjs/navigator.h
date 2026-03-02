#ifndef NETSURF_QUICKJS_NAVIGATOR_H
#define NETSURF_QUICKJS_NAVIGATOR_H

#include <quickjs.h>

/* Initialise the Navigator class in a context */
void qjsky_init_navigator(JSContext *ctx);

/* Create a new Navigator object */
JSValue qjsky_create_navigator(JSContext *ctx);

#endif
