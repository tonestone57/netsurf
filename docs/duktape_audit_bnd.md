# Duktape API Usage Audit (NetSurf Specific)

| API Call | Occurrences | Files |
|----------|-------------|-------|
| duk_pop | 79 | CanvasRenderingContext2D.bnd, Console.bnd, Document.bnd, Element.bnd, EventTarget.bnd, HTMLCanvas... |
| duk_get_prop_string | 61 | CanvasRenderingContext2D.bnd, Console.bnd, Document.bnd, Element.bnd, EventTarget.bnd, HTMLCanvas... |
| duk_push_lstring | 52 | Console.bnd, DOMSettableTokenList.bnd, DOMTokenList.bnd, Element.bnd, Event.bnd, KeyboardEvent.bn... |
| duk_context | 46 | Console.bnd, EventTarget.bnd, Window.bnd, dukky.c, dukky.h |
| duk_size_t | 42 | Console.bnd, DOMImplementation.bnd, DOMSettableTokenList.bnd, DOMTokenList.bnd, Document.bnd, Ele... |
| duk_safe_to_lstring | 35 | Console.bnd, DOMImplementation.bnd, DOMTokenList.bnd, Document.bnd, Element.bnd, Event.bnd, Keybo... |
| duk_push_string | 33 | Console.bnd, Document.bnd, Navigator.bnd, Window.bnd, dukky.c |
| duk_dup | 30 | Console.bnd, Document.bnd, Element.bnd, EventTarget.bnd, HTMLCanvasElement.bnd, Node.bnd, Window.... |
| duk_get_top | 26 | CanvasRenderingContext2D.bnd, Console.bnd, DOMTokenList.bnd, Document.bnd, EventTarget.bnd, Node.... |
| duk_safe_to_string | 26 | Console.bnd, Document.bnd, dukky.c |
| duk_put_prop_string | 25 | Console.bnd, Document.bnd, Element.bnd, EventTarget.bnd, HTMLCanvasElement.bnd, ImageData.bnd, No... |
| duk_insert | 24 | Console.bnd, EventTarget.bnd, dukky.c |
| duk_push_boolean | 23 | DOMTokenList.bnd, Element.bnd, Event.bnd, EventTarget.bnd, KeyboardEvent.bnd, Navigator.bnd, Node... |
| duk_push_int | 20 | CanvasRenderingContext2D.bnd, EventTarget.bnd, ImageData.bnd, Window.bnd, dukky.c |
| duk_is_undefined | 19 | Console.bnd, Document.bnd, Element.bnd, EventTarget.bnd, HTMLCanvasElement.bnd, Node.bnd, Window.... |
| duk_int_t | 19 | EventTarget.bnd, Window.bnd, dukky.c, dukky.h |
| duk_get_pointer | 19 | CanvasRenderingContext2D.bnd, Console.bnd, EventTarget.bnd, Location.bnd, Node.bnd, Window.bnd |
| duk_push_object | 18 | Console.bnd, EventTarget.bnd, Window.bnd, dukky.c |
| duk_pop_2 | 16 | EventTarget.bnd, Window.bnd, dukky.c |
| duk_push_pointer | 16 | Document.bnd, Element.bnd, HTMLCanvasElement.bnd, Node.bnd, Window.bnd, dukky.c |
| duk_push_global_object | 15 | Console.bnd, Document.bnd, Location.bnd, Window.bnd, dukky.c |
| duk_push_this | 14 | Console.bnd, Document.bnd, Element.bnd, EventTarget.bnd, HTMLCanvasElement.bnd, ImageData.bnd, No... |
| duk_get_prop_index | 13 | EventTarget.bnd, dukky.c |
| duk_get_int | 12 | CanvasRenderingContext2D.bnd, EventTarget.bnd, Window.bnd, dukky.c |
| duk_get_global_string | 12 | Window.bnd, dukky.c |
| duk_get_prop | 11 | Console.bnd, Window.bnd, dukky.c |
| duk_bool_t | 11 | KeyboardEvent.bnd, dukky.c, dukky.h |
| duk_set_top | 10 | Console.bnd, Element.bnd, Node.bnd, dukky.c |
| duk_error | 10 | CanvasRenderingContext2D.bnd, Console.bnd, Document.bnd, HTMLCanvasElement.bnd, Window.bnd, dukky.c |
| duk_to_int | 10 | CanvasRenderingContext2D.bnd, EventTarget.bnd, dukky.c |
| duk_push_uint | 10 | Console.bnd, DOMTokenList.bnd, Element.bnd, Event.bnd, KeyboardEvent.bnd, NamedNodeMap.bnd, Node.... |
| duk_uint_t | 9 | Console.bnd, DOMTokenList.bnd, Element.bnd, KeyboardEvent.bnd, NamedNodeMap.bnd, Node.bnd, NodeLi... |
| duk_put_prop_index | 8 | EventTarget.bnd, Window.bnd, dukky.c |
| duk_idx_t | 8 | Console.bnd, DOMTokenList.bnd, Window.bnd |
| duk_uarridx_t | 7 | EventTarget.bnd, dukky.c, dukky.h |
| duk_put_prop | 7 | Console.bnd, Window.bnd, dukky.c |
| duk_pop_n | 7 | EventTarget.bnd, Window.bnd, dukky.c |
| duk_ret_t | 6 | Window.bnd, dukky.c, dukky.h |
| duk_pop_3 | 6 | EventTarget.bnd, dukky.c |
| duk_to_boolean | 6 | EventTarget.bnd, Node.bnd, dukky.c |
| duk_put_global_string | 5 | dukky.c |
| duk_push_null | 5 | CanvasRenderingContext2D.bnd, DOMTokenList.bnd, Element.bnd, HTMLCanvasElement.bnd, dukky.c |
| duk_get_boolean | 4 | Event.bnd, dukky.c |
| duk_concat | 4 | Console.bnd, Document.bnd, dukky.c |
| duk_push_array | 4 | EventTarget.bnd, Window.bnd, dukky.c |
| duk_del_prop | 4 | Console.bnd, Window.bnd, dukky.c |
| duk_remove | 3 | Window.bnd, dukky.c |
| duk_safe_call | 3 | dukky.c |
| duk_pcompile_lstring_filename | 3 | dukky.c |
| duk_pcall_method | 3 | dukky.c |
| duk_generic_error | 3 | CanvasRenderingContext2D.bnd |
| duk_is_string | 3 | Console.bnd, Window.bnd |
| duk_to_uint | 3 | DOMTokenList.bnd, NamedNodeMap.bnd, NodeList.bnd |
| duk_to_string | 3 | HTMLCanvasElement.bnd, Window.bnd |
| duk_push_c_function | 2 | Window.bnd, dukky.c |
| duk_del_prop_index | 2 | dukky.c |
| duk_gc | 2 | dukky.c |
| duk_pcall | 2 | dukky.c |
| duk_is_boolean | 2 | dukky.c |
| duk_push_number | 2 | CanvasRenderingContext2D.bnd |
| duk_double_t | 2 | CanvasRenderingContext2D.bnd |
| duk_get_uint | 2 | CanvasRenderingContext2D.bnd |
| duk_strict_equals | 2 | EventTarget.bnd |
| duk_get_type | 2 | EventTarget.bnd |
| duk_to_lstring | 2 | Element.bnd, EventTarget.bnd |
| duk_require_lstring | 2 | DOMSettableTokenList.bnd, DOMTokenList.bnd |
| duk_get_string | 1 | dukky.c |
| duk_set_prototype | 1 | dukky.c |
| duk_call | 1 | dukky.c |
| duk_create_heap | 1 | dukky.c |
| duk_destroy_heap | 1 | dukky.c |
| duk_push_thread | 1 | dukky.c |
| duk_require_context | 1 | dukky.c |
| duk_set_global_object | 1 | dukky.c |
| duk_del_prop_string | 1 | dukky.c |
| duk_dup_top | 1 | dukky.c |
| duk_safe_to_stacktrace | 1 | dukky.c |
| duk_memory_functions | 1 | dukky.c |
| duk_get_memory_functions | 1 | dukky.c |
| duk_push_context_dump | 1 | dukky.c |
| duk_replace | 1 | dukky.c |
| duk_pcompile | 1 | dukky.c |
| duk_has_prop | 1 | dukky.c |
| duk_push_undefined | 1 | dukky.c |
| duk_push_bool | 1 | DOMImplementation.bnd |
| duk_to_uint32 | 1 | Console.bnd |
| duk_push_sprintf | 1 | Console.bnd |
| duk_push_error_object | 1 | Console.bnd |
| duk_opt_boolean | 1 | DOMTokenList.bnd |
| duk_libdom | 1 | netsurf.bnd |
| duk_get_length | 1 | Window.bnd |
| duk_def_prop | 1 | Window.bnd |
| duk_push_buffer | 1 | ImageData.bnd |
| duk_push_buffer_object | 1 | ImageData.bnd |
