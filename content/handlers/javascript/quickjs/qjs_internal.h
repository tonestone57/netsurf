#ifndef NETSURF_QUICKJS_INTERNAL_H
#define NETSURF_QUICKJS_INTERNAL_H

#include <quickjs.h>

struct jsheap {
	JSRuntime *rt;
	JSClassID node_class_id;
	JSAtom node_map_atom;
	void *timer_ring;
	int next_timer_handle;
};

#endif
