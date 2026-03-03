#ifndef NETSURF_QUICKJS_URL_H
#define NETSURF_QUICKJS_URL_H

#include <quickjs.h>
#include "utils/nsurl.h"

void qjsky_init_url(JSContext *ctx);
void qjsky_init_urlsearchparams(JSContext *ctx);

JSValue qjsky_create_url(JSContext *ctx, nsurl *url);
JSValue qjsky_create_urlsearchparams(JSContext *ctx, const char *init);

#endif
