#ifndef NETSURF_QUICKJS_DUKTAPE_COMPAT_H
#define NETSURF_QUICKJS_DUKTAPE_COMPAT_H

#include <quickjs.h>

/*
 * This header is a placeholder for a compatibility layer.
 * Note: A direct 1-to-1 mapping from Duktape to QuickJS is not feasible
 * due to fundamentally different memory management (Value Stack vs Ref Counting).
 *
 * This file should eventually contain an abstraction layer (NSJS) that both
 * engines can implement.
 */

typedef JSContext duk_context;
typedef JSValue duk_ret_t; /* QuickJS functions return JSValue */

/* The following are stubs to satisfy structural requirements during analysis */
#define DUK_RET_TYPE_ERROR JS_EXCEPTION

#endif
