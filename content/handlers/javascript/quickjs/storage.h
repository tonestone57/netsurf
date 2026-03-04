#ifndef NETSURF_JAVASCRIPT_QUICKJS_STORAGE_H
#define NETSURF_JAVASCRIPT_QUICKJS_STORAGE_H

#include <quickjs.h>

extern JSClassDef qjsky_storage_class;

void qjsky_init_storage(JSContext *ctx);
JSValue qjsky_create_storage(JSContext *ctx, bool session);

#endif
