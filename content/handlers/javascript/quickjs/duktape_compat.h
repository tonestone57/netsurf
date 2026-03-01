#ifndef NETSURF_QUICKJS_DUKTAPE_COMPAT_H
#define NETSURF_QUICKJS_DUKTAPE_COMPAT_H

#include <quickjs.h>

/*
 * Engine-Neutral Abstraction Placeholder
 *
 * This header is a placeholder for a future engine-agnostic API.
 *
 * NOTE: Direct mapping of stack-based Duktape APIs to reference-counted
 * QuickJS APIs via macros is not supported as it leads to memory management
 * errors and scope issues (e.g. argc/argv dependencies).
 *
 * The migration strategy is:
 * 1. Implement a QuickJS backend for nsgenbind to generate native QJS wrappers.
 * 2. Refactor hand-written .bnd fragments to use an engine-neutral "NSJS" API.
 */

typedef JSContext duk_context;
typedef JSValue duk_ret_t;

/* Stub to allow structural compilation of generated headers */
#define DUK_RET_TYPE_ERROR JS_EXCEPTION

#endif
