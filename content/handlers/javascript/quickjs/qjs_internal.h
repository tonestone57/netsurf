#ifndef NETSURF_QUICKJS_INTERNAL_H
#define NETSURF_QUICKJS_INTERNAL_H

#include <quickjs.h>

struct jsheap {
	JSRuntime *rt;
	JSClassID node_class_id;
	JSAtom node_map_atom;
	JSAtom handler_map_atom;
	JSAtom handler_listener_map_atom;
	JSAtom event_proto_atom;
	void *timer_ring;
	int next_timer_handle;
};

struct jsthread {
	JSContext *ctx;
	struct jsheap *heap;
	void *win_priv;
	void *doc_priv;
};

#endif
