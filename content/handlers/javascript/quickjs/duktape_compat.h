#ifndef NETSURF_QUICKJS_DUKTAPE_COMPAT_H
#define NETSURF_QUICKJS_DUKTAPE_COMPAT_H

#include <quickjs.h>

/*
 * This header is a placeholder for a future compatibility or abstraction layer.
 *
 * NOTE: A direct 1-to-1 macro mapping from Duktape to QuickJS is not possible
 * for most APIs because Duktape is stack-based and QuickJS uses
 * reference-counted JSValue handles.
 *
 * The long-term migration strategy involves:
 * 1. Implementing a QuickJS backend for nsgenbind.
 * 2. Introducing a engine-neutral "NSJS" API for NetSurf.
 */

typedef JSContext duk_context;
typedef JSValue duk_ret_t;

/* Stub to allow structural compilation of generated headers */
#define DUK_RET_TYPE_ERROR JS_EXCEPTION

#endif
