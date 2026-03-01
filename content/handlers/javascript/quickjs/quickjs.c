/*
 * QuickJS implementation of javascript engine functions.
 */

#include <quickjs.h>
#include "utils/errors.h"
#include "utils/log.h"
#include "content/content.h"
#include "javascript/js.h"

struct jsheap {
    JSRuntime *rt;
};

struct jsthread {
    JSContext *ctx;
    jsheap *heap;
};

void js_initialise(void)
{
    NSLOG(netsurf, INFO, "QuickJS engine initialised");
}

void js_finalise(void)
{
}

nserror js_newheap(int timeout, jsheap **heap)
{
    jsheap *h = calloc(1, sizeof(*h));
    if (h == NULL) return NSERROR_NOMEM;

    h->rt = JS_NewRuntime();
    if (h->rt == NULL) {
        free(h);
        return NSERROR_NOMEM;
    }

    /* TODO: Set memory limit and timeout */

    *heap = h;
    return NSERROR_OK;
}

void js_destroyheap(jsheap *heap)
{
    if (heap) {
        JS_FreeRuntime(heap->rt);
        free(heap);
    }
}

nserror js_newthread(jsheap *heap, void *win_priv, void *doc_priv, jsthread **thread)
{
    jsthread *t = calloc(1, sizeof(*t));
    if (t == NULL) return NSERROR_NOMEM;

    t->heap = heap;
    t->ctx = JS_NewContext(heap->rt);
    if (t->ctx == NULL) {
        free(t);
        return NSERROR_NOMEM;
    }

    /* TODO: Initialize DOM objects and Window */

    *thread = t;
    return NSERROR_OK;
}

nserror js_closethread(jsthread *thread)
{
    return NSERROR_OK;
}

void js_destroythread(jsthread *thread)
{
    if (thread) {
        JS_FreeContext(thread->ctx);
        free(thread);
    }
}

bool js_exec(jsthread *thread, const uint8_t *txt, size_t txtlen, const char *name)
{
    JSValue val = JS_Eval(thread->ctx, (const char *)txt, txtlen, name, JS_EVAL_TYPE_GLOBAL);
    bool result = !JS_IsException(val);
    JS_FreeValue(thread->ctx, val);
    return result;
}

bool js_fire_event(jsthread *thread, const char *type, struct dom_document *doc, struct dom_node *target)
{
    /* TODO: Implementation */
    return true;
}

void js_handle_new_element(jsthread *thread, struct dom_element *node)
{
    /* TODO: Implementation */
}

void js_event_cleanup(jsthread *thread, struct dom_event *evt)
{
    /* TODO: Implementation */
}
