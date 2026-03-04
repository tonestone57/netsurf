#ifndef NETSURF_QUICKJS_INTERNAL_H
#define NETSURF_QUICKJS_INTERNAL_H

#include <quickjs.h>

struct jsheap {
	JSRuntime *rt;
	JSClassID node_class_id;
	JSClassID event_class_id;
	JSClassID xhr_class_id;
	JSClassID location_class_id;
	JSClassID history_class_id;
	JSClassID navigator_class_id;
	JSClassID screen_class_id;
	JSClassID barprop_class_id;
	JSClassID url_class_id;
	JSClassID urlsearchparams_class_id;
	JSClassID storage_class_id;
	JSClassID dom_implementation_class_id;
	JSAtom handler_map_atom;
	JSAtom handler_listener_map_atom;
	JSAtom libdom_registered_atom;
	JSAtom node_proto_atom;
	JSAtom document_proto_atom;
	JSAtom element_proto_atom;
	JSAtom html_element_proto_atom;
	JSAtom text_proto_atom;
	JSAtom comment_proto_atom;
	JSAtom dom_implementation_proto_atom;
	JSAtom event_proto_atom;
	JSAtom uievent_proto_atom;
	JSAtom mouseevent_proto_atom;
	JSAtom keyboardevent_proto_atom;
	JSAtom storage_proto_atom;
	JSAtom node_ctor_atom;
	JSAtom document_ctor_atom;
	JSAtom element_ctor_atom;
	JSAtom html_element_ctor_atom;
	JSAtom text_ctor_atom;
	JSAtom comment_ctor_atom;
	JSAtom event_ctor_atom;
	JSAtom uievent_ctor_atom;
	JSAtom mouseevent_ctor_atom;
	JSAtom keyboardevent_ctor_atom;
	JSAtom dom_implementation_ctor_atom;
	struct dom_string *node_key;
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
