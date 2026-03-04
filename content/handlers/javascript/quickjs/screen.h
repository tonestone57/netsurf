#ifndef NETSURF_QUICKJS_SCREEN_H
#define NETSURF_QUICKJS_SCREEN_H

#include <quickjs.h>

void qjsky_init_screen(JSContext *ctx);
JSValue qjsky_create_screen(JSContext *ctx);

#endif
