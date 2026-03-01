#ifndef NETSURF_QUICKJS_DUKTAPE_COMPAT_H
#define NETSURF_QUICKJS_DUKTAPE_COMPAT_H

#include <quickjs.h>

/*
 * Engine-Neutral "NSJS" Abstraction (Conceptual)
 *
 * This header defines macros that will eventually be used in .bnd files
 * to allow cross-engine compatibility.
 */

#define NSJS_PUSH_STRING(ctx, s) JS_NewString(ctx, s)
#define NSJS_PUSH_INT(ctx, i) JS_NewInt32(ctx, i)
#define NSJS_GET_STRING(ctx, val) JS_ToCString(ctx, val)

typedef JSContext duk_context;
typedef JSValue duk_ret_t;

/* Stub to allow structural compilation of generated headers */
#define DUK_RET_TYPE_ERROR JS_EXCEPTION

#endif
