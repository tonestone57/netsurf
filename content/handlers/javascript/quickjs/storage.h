#ifndef NETSURF_QUICKJS_STORAGE_H
#define NETSURF_QUICKJS_STORAGE_H

#include <quickjs.h>

void qjsky_init_storage(JSContext *ctx);
JSValue qjsky_create_storage(JSContext *ctx);

extern JSClassDef qjsky_storage_class;

#endif
