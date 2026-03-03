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

static inline void *duk_get_pointer(JSContext *ctx, int index)
{
	/* This is a simplified shim for .bnd compatibility.
	 * In a real migration, nsgenbind should handle this via JS_GetOpaque.
	 */
	return NULL;
}

#define duk_get_top(ctx) (0)
#define duk_pop(ctx) ((void)0)
#define duk_pop_2(ctx) ((void)0)
#define duk_pop_n(ctx, n) ((void)0)
#define duk_push_boolean(ctx, b) ((void)(b))
#define duk_get_prop_string(ctx, idx, s) (1)
#define duk_put_prop_string(ctx, idx, s) (1)
#define duk_insert(ctx, idx) ((void)0)
#define duk_push_int(ctx, i) ((void)0)
#define duk_push_uint(ctx, u) ((void)0)
#define duk_push_string(ctx, s) ((void)0)
#define duk_push_object(ctx) ((void)0)
#define duk_push_array(ctx) ((void)0)
#define duk_push_null(ctx) ((void)0)
#define duk_push_undefined(ctx) ((void)0)
#define duk_is_undefined(ctx, idx) (0)
#define duk_error(ctx, err, msg) (JS_EXCEPTION)

#define DUK_VARARGS (-1)
#define DUK_EXEC_SUCCESS (0)

#endif
