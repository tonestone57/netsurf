#ifndef NETSURF_QUICKJS_DUKTAPE_COMPAT_H
#define NETSURF_QUICKJS_DUKTAPE_COMPAT_H

#include <quickjs.h>

/*
 * Engine-Neutral Abstraction Stubs
 *
 * This header acts as a placeholder for engine-agnostic macros to be used
 * during the migration from Duktape to QuickJS.
 *
 * NOTE: Direct mapping of stack-based Duktape APIs to reference-counted
 * QuickJS APIs via macros is not feasible. The long-term solution involves
 * using a dedicated QuickJS backend for the nsgenbind tool to generate
 * correct C wrappers.
 */

typedef JSContext duk_context;
typedef JSValue duk_ret_t;

/* Stub to allow structural compilation of generated headers */
#define DUK_RET_TYPE_ERROR JS_EXCEPTION

#endif
