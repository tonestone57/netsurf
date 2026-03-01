#ifndef NETSURF_QUICKJS_DUKTAPE_COMPAT_H
#define NETSURF_QUICKJS_DUKTAPE_COMPAT_H

#include <quickjs.h>

/*
 * Engine-Neutral Abstraction Placeholder
 *
 * This header serves as a structural bridge for common types.
 *
 * NOTE: Direct mapping of stack-based Duktape APIs to reference-counted
 * QuickJS APIs via macros is not recommended. The migration strategy
 * relies on automated binding generation via nsgenbind.
 */

typedef JSContext duk_context;
typedef JSValue duk_ret_t;

#define DUK_RET_TYPE_ERROR JS_EXCEPTION

#endif
