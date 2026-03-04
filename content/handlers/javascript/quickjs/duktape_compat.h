#ifndef NETSURF_QUICKJS_DUKTAPE_COMPAT_H
#define NETSURF_QUICKJS_DUKTAPE_COMPAT_H

#include <quickjs.h>
#include <assert.h>
#include "utils/log.h"
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "content/handlers/javascript/quickjs/qjs_internal.h"

/*
 * Engine-Neutral Abstraction Placeholder
 *
 * This header serves as a structural bridge for common types.
 */

typedef JSContext duk_context;
typedef JSValue duk_ret_t;

#define DUK_RET_TYPE_ERROR JS_EXCEPTION

static inline void *duk_get_pointer(JSContext *ctx, int index)
{
	struct jsthread *thread = JS_GetContextOpaque(ctx);
	JSValue val;
	if (index < 0) {
		val = thread->stack[thread->stack_top + index];
	} else {
		val = thread->stack[index];
	}
	/* Note: this assumes we only store pointers in opaque objects for this shim */
	return JS_GetOpaque(val, 0); /* This is a very rough shim */
}

#define duk_get_top(ctx) (((struct jsthread *)JS_GetContextOpaque(ctx))->stack_top)
#define duk_pop(ctx) (((struct jsthread *)JS_GetContextOpaque(ctx))->stack_top--)
#define duk_pop_2(ctx) (((struct jsthread *)JS_GetContextOpaque(ctx))->stack_top -= 2)
#define duk_pop_n(ctx, n) (((struct jsthread *)JS_GetContextOpaque(ctx))->stack_top -= (n))

#define duk_push_boolean(ctx, b) do { \
	struct jsthread *t = JS_GetContextOpaque(ctx); \
	t->stack[t->stack_top++] = JS_NewBool(ctx, (b)); \
} while(0)

#define duk_push_int(ctx, i) do { \
	struct jsthread *t = JS_GetContextOpaque(ctx); \
	t->stack[t->stack_top++] = JS_NewInt32(ctx, (i)); \
} while(0)

#define duk_push_string(ctx, s) do { \
	struct jsthread *t = JS_GetContextOpaque(ctx); \
	t->stack[t->stack_top++] = JS_NewString(ctx, (s)); \
} while(0)

#define duk_push_lstring(ctx, s, l) do { \
	struct jsthread *t = JS_GetContextOpaque(ctx); \
	t->stack[t->stack_top++] = JS_NewStringLen(ctx, (s), (l)); \
} while(0)

#define duk_push_global_object(ctx) do { \
	struct jsthread *t = JS_GetContextOpaque(ctx); \
	t->stack[t->stack_top++] = JS_GetGlobalObject(ctx); \
} while(0)

#define duk_push_this(ctx) do { \
	struct jsthread *t = JS_GetContextOpaque(ctx); \
	t->stack[t->stack_top++] = JS_DupValue(ctx, t->this_val); \
} while(0)

#define duk_get_prop_string(ctx, idx, s) ({ \
	struct jsthread *t = JS_GetContextOpaque(ctx); \
	int real_idx = (idx < 0) ? t->stack_top + idx : idx; \
	JSValue val = JS_GetPropertyStr(ctx, t->stack[real_idx], (s)); \
	t->stack[t->stack_top++] = val; \
	!JS_IsUndefined(val); \
})

#define duk_safe_to_lstring(ctx, idx, out_len) ({ \
	struct jsthread *t = JS_GetContextOpaque(ctx); \
	int real_idx = (idx < 0) ? t->stack_top + idx : idx; \
	size_t len; \
	const char *str = JS_ToCStringLen(ctx, &len, t->stack[real_idx]); \
	if (out_len) *out_len = len; \
	str; \
})

#define DUK_VARARGS (-1)
#define DUK_EXEC_SUCCESS (0)

#define dukky_method_preamble() do { \
	struct jsthread *t = JS_GetContextOpaque(ctx); \
	t->stack_top = 0; \
	t->this_val = this_val; \
	for (int i=0; i<argc && i<QJS_STACK_SIZE; i++) t->stack[t->stack_top++] = argv[i]; \
} while(0)

#define dukky_method_epilogue() do { \
	struct jsthread *t = JS_GetContextOpaque(ctx); \
	return t->stack_top > 0 ? t->stack[t->stack_top - 1] : JS_UNDEFINED; \
} while(0)

#define dukky_getter_preamble() do { \
	struct jsthread *t = JS_GetContextOpaque(ctx); \
	t->stack_top = 0; \
	t->this_val = this_val; \
} while(0)

#define dukky_getter_epilogue() dukky_method_epilogue()

#define dukky_setter_preamble() do { \
	struct jsthread *t = JS_GetContextOpaque(ctx); \
	t->stack_top = 0; \
	t->this_val = this_val; \
	t->stack[t->stack_top++] = val; \
} while(0)

#define dukky_setter_epilogue() do { \
	return JS_UNDEFINED; \
} while(0)

#endif
