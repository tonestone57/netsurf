#include "content/handlers/javascript/quickjs/qjsky.h"
#include "utils/log.h"

static JSClassID qjsky_node_class_id = 0;

JSValue qjsky_push_node(JSContext *ctx, struct dom_node *node)
{
    if (qjsky_node_class_id == 0) {
        JS_NewClassID(&qjsky_node_class_id);
        /* TODO: Define class and register it */
    }

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
