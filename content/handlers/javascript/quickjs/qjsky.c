#include <string.h>
#include <stdlib.h>
#include "content/handlers/javascript/quickjs/qjsky.h"
#include "utils/log.h"

static JSClassID qjsky_node_class_id = 0;

static void qjsky_node_finalizer(JSRuntime *rt, JSValue val)
{
	struct dom_node *node = JS_GetOpaque(val, qjsky_node_class_id);
	if (node) {
		dom_node_unref(node);
	}
}

static JSClassDef qjsky_node_class = {
	"Node",
	.finalizer = qjsky_node_finalizer,
};

void qjsky_init_runtime(JSRuntime *rt)
{
	if (qjsky_node_class_id == 0) {
		JS_NewClassID(&qjsky_node_class_id);
	}
	JS_NewClass(rt, qjsky_node_class_id, &qjsky_node_class);
}

JSValue qjsky_push_node(JSContext *ctx, struct dom_node *node)
{
	JSValue obj = JS_NewObjectProtoClass(ctx, JS_NULL, qjsky_node_class_id);
	if (JS_IsException(obj)) return obj;

	JS_SetOpaque(obj, node);
	dom_node_ref(node);

	return obj;
}

struct dom_node *qjsky_get_node(JSContext *ctx, JSValue val)
{
	return JS_GetOpaque(val, qjsky_node_class_id);
}
