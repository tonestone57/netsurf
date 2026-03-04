#ifndef NETSURF_QUICKJS_SCREEN_H
#define NETSURF_QUICKJS_SCREEN_H

#include <quickjs.h>

/* Initialise the Screen class in a context */
void qjsky_init_screen(JSContext *ctx);

/* Create a new Screen object */
JSValue qjsky_create_screen(JSContext *ctx);

#endif
