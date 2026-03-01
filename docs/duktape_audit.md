# Duktape API Usage Audit

| API Call | Occurrences | Files |
|----------|-------------|-------|
| duk_hthread | 1750 | duk_config.h, duktape.c |
| duk_uint8_t | 1194 | duk_config.h, duktape.c |
| duk_tval | 1103 | duk_config.h, duktape.c |
| duk_idx_t | 1032 | Console.bnd, DOMTokenList.bnd, Window.bnd, duk_config.h, duktape.c, duktape.h |
| duk_size_t | 947 | Console.bnd, DOMImplementation.bnd, DOMSettableTokenList.bnd, DOMTokenList.bnd, Document.bnd, Ele... |
| duk_int_t | 848 | EventTarget.bnd, Window.bnd, duk_config.h, dukky.c, dukky.h, duktape.c, duktape.h |
| duk_uint32_t | 816 | duk_config.h, duktape.c, duktape.h |
| duk_hobject | 785 | duktape.c |
| duk_bool_t | 748 | KeyboardEvent.bnd, duk_config.h, duk_custom.h, dukky.c, dukky.h, duktape.c, duktape.h |
| duk_small_uint_t | 646 | duk_config.h, duktape.c, duktape.h |
| duk_uint_t | 552 | Console.bnd, DOMTokenList.bnd, Element.bnd, KeyboardEvent.bnd, NamedNodeMap.bnd, Node.bnd, NodeLi... |
| duk_context | 531 | Console.bnd, EventTarget.bnd, Window.bnd, duk_config.h, dukky.c, dukky.h, duktape.c, duktape.h |
| duk_hstring | 529 | duktape.c |
| duk_double_t | 479 | CanvasRenderingContext2D.bnd, duk_config.h, duktape.c, duktape.h |
| duk_ret_t | 469 | Window.bnd, duk_config.h, dukky.c, dukky.h, duktape.c, duktape.h |
| duk_heaphdr | 464 | duktape.c |
| duk_small_int_t | 381 | duk_config.h, duktape.c |
| duk_heap | 256 | duk_config.h, duktape.c |
| duk_regconst_t | 251 | duktape.c |
| duk_get_tval | 236 | duktape.c |
| duk_uint16_t | 224 | duk_config.h, duktape.c, duktape.h |
| duk_compiler_ctx | 216 | duktape.c |
| duk_pop | 182 | CanvasRenderingContext2D.bnd, Console.bnd, Document.bnd, Element.bnd, EventTarget.bnd, HTMLCanvas... |
| duk_get_top | 177 | CanvasRenderingContext2D.bnd, Console.bnd, DOMTokenList.bnd, Document.bnd, EventTarget.bnd, Node.... |
| duk_codepoint_t | 170 | duk_config.h, duktape.c, duktape.h |
| duk_int32_t | 168 | duk_config.h, duktape.c, duktape.h |
| duk_uint_fast32_t | 153 | duk_config.h, duktape.c |
| duk_activation | 146 | duktape.c |
| duk_uarridx_t | 143 | EventTarget.bnd, duk_config.h, dukky.c, dukky.h, duktape.c, duktape.h |
| duk_hbuffer | 137 | duk_config.h, duktape.c |
| duk_fb_sprintf | 121 | duktape.c |
| duk_hbufobj | 120 | duktape.c |
| duk_ivalue | 107 | duktape.c |
| duk_hcompfunc | 106 | duktape.c |
| duk__tv | 106 | duktape.c |
| duk_internal | 104 | duktape.c |
| duk_uint64_t | 100 | duk_config.h, duktape.c |
| duk_dup | 99 | Console.bnd, Document.bnd, Element.bnd, EventTarget.bnd, HTMLCanvasElement.bnd, Node.bnd, Window.... |
| duk_harray | 89 | duktape.c |
| duk_push_tval | 88 | duktape.c |
| duk_push_lstring | 83 | Console.bnd, DOMSettableTokenList.bnd, DOMTokenList.bnd, Element.bnd, Event.bnd, KeyboardEvent.bn... |
| duk_replace | 73 | dukky.c, duktape.c, duktape.h |
| duk_instr_t | 70 | duktape.c |
| duk_push_int | 68 | CanvasRenderingContext2D.bnd, EventTarget.bnd, ImageData.bnd, Window.bnd, dukky.c, duktape.c, duk... |
| duk_require_tval | 68 | duktape.c |
| duk_pop_unsafe | 67 | duktape.c |
| duk_get_prop_string | 66 | CanvasRenderingContext2D.bnd, Console.bnd, Document.bnd, Element.bnd, EventTarget.bnd, HTMLCanvas... |
| duk_is_undefined | 63 | Console.bnd, Document.bnd, Element.bnd, EventTarget.bnd, HTMLCanvasElement.bnd, Node.bnd, Window.... |
| duk_insert | 63 | Console.bnd, EventTarget.bnd, dukky.c, duktape.c, duktape.h |
| duk_get_prop_index | 63 | EventTarget.bnd, dukky.c, duktape.c, duktape.h |
| duk_push_hobject | 63 | duktape.c |
| duk_push_uint | 62 | Console.bnd, DOMTokenList.bnd, Element.bnd, Event.bnd, KeyboardEvent.bnd, NamedNodeMap.bnd, Node.... |
| duk_double_union | 61 | duktape.c |
| duk_hnatfunc | 60 | duktape.c |
| duk_bufwriter_ctx | 60 | duktape.c |
| duk_ucodepoint_t | 60 | duk_config.h, duktape.c |
| duk_memcpy | 60 | duktape.c |
| duk_memzero | 60 | duktape.c |
| duk_int64_t | 58 | duk_config.h, duktape.c |
| duk_push_boolean | 57 | DOMTokenList.bnd, Element.bnd, Event.bnd, EventTarget.bnd, KeyboardEvent.bnd, Navigator.bnd, Node... |
| duk_push_number | 57 | CanvasRenderingContext2D.bnd, duktape.c, duktape.h |
| duk_double_equals | 57 | duktape.c |
| duk_xdef_prop_stridx_short | 57 | duktape.c |
| duk_errcode_t | 56 | duk_config.h, duktape.c, duktape.h |
| duk__bigint | 55 | duktape.c |
| duk__advance | 54 | duktape.c |
| duk_require_normalize_index | 52 | duktape.c, duktape.h |
| duk_to_string | 52 | HTMLCanvasElement.bnd, Window.bnd, duktape.c, duktape.h |
| duk_push_string | 51 | Console.bnd, Document.bnd, Navigator.bnd, Window.bnd, dukky.c, duktape.c, duktape.h |
| duk_c_function | 50 | duktape.c, duktape.h |
| duk_json_enc_ctx | 50 | duktape.c |
| duk_uidx_t | 50 | duk_config.h, duktape.c |
| duk_push_this | 48 | Console.bnd, Document.bnd, Element.bnd, EventTarget.bnd, HTMLCanvasElement.bnd, ImageData.bnd, No... |
| duk__dst | 47 | duktape.c |
| duk_set_top | 45 | Console.bnd, Element.bnd, Node.bnd, dukky.c, duktape.c, duktape.h |
| duk__val_len | 45 | duktape.c |
| duk_catcher | 44 | duktape.c |
| duk_put_prop_index | 43 | EventTarget.bnd, Window.bnd, dukky.c, duktape.c, duktape.h |
| duk_push_undefined | 43 | dukky.c, duktape.c, duktape.h |
| duk_require_hobject | 43 | duktape.c |
| duk__emit_a_bc | 43 | duktape.c |
| duk_get_hobject | 42 | duktape.c |
| duk_to_int | 40 | CanvasRenderingContext2D.bnd, EventTarget.bnd, dukky.c, duktape.c, duktape.h |
| duk_get_current_magic | 40 | duktape.c, duktape.h |
| duk_bd_decode | 40 | duktape.c |
| duk__len | 40 | duktape.c |
| duk_get_prop_stridx_short | 40 | duktape.c |
| duk__err | 40 | duktape.c |
| duk__line | 40 | duktape.c |
| duk_json_dec_ctx | 40 | duktape.c |
| duk_hdecenv | 39 | duktape.c |
| duk_safe_to_lstring | 38 | Console.bnd, DOMImplementation.bnd, DOMTokenList.bnd, Document.bnd, Element.bnd, Event.bnd, Keybo... |
| duk_hbuffer_dynamic | 38 | duktape.c |
| duk__advance_expect | 38 | duktape.c |
| duk_pop_2 | 37 | EventTarget.bnd, Window.bnd, dukky.c, duktape.c, duktape.h |
| duk__p | 37 | duktape.c |
| duk_push_hstring | 37 | duktape.c |
| duk_lexer_ctx | 36 | duktape.c |
| duk_compiler_instr | 36 | duktape.c |
| duk_memcpy_unsafe | 36 | duktape.c |
| duk_hboundfunc | 35 | duktape.c |
| duk__get_current_pc | 35 | duktape.c |
| duk_remove_m2 | 34 | duktape.c |
| duk_get_tval_or_unused | 34 | duktape.c |
| duk_push_object | 33 | Console.bnd, EventTarget.bnd, Window.bnd, dukky.c, duktape.c, duktape.h |
| duk_propdesc | 33 | duktape.c |
| duk_int16_t | 33 | duk_config.h, duktape.c |
| duk_push_hstring_stridx | 33 | duktape.c |
| duk_put_prop_stridx_short | 33 | duktape.c |
| duk_to_number | 32 | duktape.c, duktape.h |
| duk_hobjenv | 32 | duktape.c |
| duk_debug_write_uint | 32 | duktape.c |
| duk_put_prop_string | 31 | Console.bnd, Document.bnd, Element.bnd, EventTarget.bnd, HTMLCanvasElement.bnd, ImageData.bnd, No... |
| duk_push_literal | 31 | duktape.c, duktape.h |
| duk_bitdecoder_ctx | 31 | duktape.c |
| duk__bi_is_valid | 31 | duktape.c |
| duk_safe_call | 30 | dukky.c, duktape.c, duktape.h |
| duk_safe_to_string | 30 | Console.bnd, Document.bnd, dukky.c, duktape.c, duktape.h |
| duk_dup_top | 30 | dukky.c, duktape.c, duktape.h |
| duk_get_length | 30 | Window.bnd, duktape.c, duktape.h |
| duk_propvalue | 30 | duktape.c |
| duk_fixedbuffer | 30 | duktape.c |
| duk__h | 30 | duktape.c |
| duk_debug_write_int | 30 | duktape.c |
| duk_known_hstring | 29 | duktape.c |
| duk_known_hobject | 29 | duktape.c |
| duk_cbor_decode_context | 29 | duktape.c |
| duk_require_stack | 28 | duktape.c, duktape.h |
| duk_hproxy | 28 | duktape.c |
| duk_float_t | 28 | duk_config.h, duktape.c |
| duk_get_prop | 27 | Console.bnd, Window.bnd, dukky.c, duktape.c, duktape.h |
| duk_is_string | 27 | Console.bnd, Window.bnd, duktape.c, duktape.h |
| duk_call_method | 27 | duktape.c, duktape.h |
| duk_debug_write_eom | 27 | duktape.c |
| duk_uint_fast8_t | 27 | duk_config.h, duktape.c |
| duk__emit_bc | 27 | duktape.c |
| duk_check_type_mask | 26 | duktape.c, duktape.h |
| duk_dup_0 | 26 | duktape.c |
| duk__vm_bitwise_binary_op | 26 | duktape.c |
| duk_is_number | 25 | duktape.c, duktape.h |
| duk_hbuffer_fixed | 25 | duktape.c |
| duk_uint_fast_t | 25 | duk_config.h, duktape.c |
| duk_small_uint_fast_t | 25 | duk_config.h, duktape.c |
| duk__mark_heaphdr | 25 | duktape.c |
| duk__emit_a_b_c | 25 | duktape.c |
| duk__ivalue_regconst | 25 | duktape.c |
| duk_put_prop | 24 | Console.bnd, Window.bnd, dukky.c, duktape.c, duktape.h |
| duk_push_null | 24 | CanvasRenderingContext2D.bnd, DOMTokenList.bnd, Element.bnd, HTMLCanvasElement.bnd, dukky.c, dukt... |
| duk_re_compiler_ctx | 24 | duktape.c |
| duk_require_hstring | 24 | duktape.c |
| duk_get_prop_stridx | 24 | duktape.c |
| duk_get_int | 23 | CanvasRenderingContext2D.bnd, EventTarget.bnd, Window.bnd, dukky.c, duktape.c, duktape.h |
| duk_push_pointer | 23 | Document.bnd, Element.bnd, HTMLCanvasElement.bnd, Node.bnd, Window.bnd, dukky.c, duktape.c, dukta... |
| duk_push_sprintf | 23 | Console.bnd, duktape.c, duktape.h |
| duk_compiler_func | 23 | duktape.c |
| duk__dprint_state | 23 | duktape.c |
| duk__expr | 23 | duktape.c |
| duk_to_boolean | 22 | EventTarget.bnd, Node.bnd, dukky.c, duktape.c, duktape.h |
| duk_is_valid_index | 22 | duktape.c, duktape.h |
| duk_int8_t | 22 | duk_config.h, duktape.c |
| duk__src | 22 | duktape.c |
| duk_fb_put_byte | 22 | duktape.c |
| duk_fb_put_cstring | 22 | duktape.c |
| duk__emit | 22 | duktape.c |
| duk__patch_jump_here | 22 | duktape.c |
| duk__vm_arith_binary_op | 22 | duktape.c |
| duk_concat | 21 | Console.bnd, Document.bnd, dukky.c, duktape.c, duktape.h |
| duk_get_pointer | 21 | CanvasRenderingContext2D.bnd, Console.bnd, EventTarget.bnd, Location.bnd, Node.bnd, Window.bnd, d... |
| duk_to_uint32 | 21 | Console.bnd, duktape.c, duktape.h |
| duk_lexer_codepoint | 21 | duktape.c |
| duk_to_number_m1 | 21 | duktape.c |
| duk_to_int_clamped | 21 | duktape.c |
| duk_debug_write_reply | 21 | duktape.c |
| duk_cbor_encode_context | 21 | duktape.c |
| duk__debug_getinfo_prop_uint | 21 | duktape.c |
| duk__append_reop | 21 | duktape.c |
| duk_push_array | 20 | EventTarget.bnd, Window.bnd, dukky.c, duktape.c, duktape.h |
| duk_push_fixed_buffer_nozero | 20 | duktape.c |
| duk__ispec_toregconst_raw | 20 | duktape.c |
| duk_push_global_object | 19 | Console.bnd, Document.bnd, Location.bnd, Window.bnd, dukky.c, duktape.c, duktape.h |
| duk_to_object | 19 | duktape.c, duktape.h |
| duk_push_this_coercible_to_string | 19 | duktape.c |
| duk_push_u32 | 19 | duktape.c |
| duk_to_hstring | 19 | duktape.c |
| duk_pop_2_unsafe | 19 | duktape.c |
| duk__debug_getinfo_flags_key | 19 | duktape.c |
| duk__patch_jump | 19 | duktape.c |
| duk__insert_u32 | 19 | duktape.c |
| duk_remove | 18 | Window.bnd, dukky.c, duktape.c, duktape.h |
| duk_error | 18 | CanvasRenderingContext2D.bnd, Console.bnd, Document.bnd, HTMLCanvasElement.bnd, Window.bnd, dukky... |
| duk_pop_n | 18 | EventTarget.bnd, Window.bnd, dukky.c, duktape.c, duktape.h |
| duk_def_prop | 18 | Window.bnd, duktape.c, duktape.h |
| duk_bd_decode_varuint | 18 | duktape.c |
| duk_debug_read_byte | 18 | duktape.c |
| duk__get_tagged_heaphdr_raw | 18 | duktape.c |
| duk_push_true | 17 | duktape.c, duktape.h |
| duk_to_primitive | 17 | duktape.c, duktape.h |
| duk_buffer_to_string | 17 | duktape.c, duktape.h |
| duk_memmove | 17 | duktape.c |
| duk_get_hstring | 17 | duktape.c |
| duk_push_hstring_empty | 17 | duktape.c |
| duk_push_object_helper | 17 | duktape.c |
| duk_int_fast32_t | 17 | duk_config.h, duktape.c |
| duk__push_tval_to_property_key | 17 | duktape.c |
| duk__ivalue_toforcedreg | 17 | duktape.c |
| duk__bc_get_u32 | 17 | duktape.c |
| duk_del_prop_index | 16 | dukky.c, duktape.c, duktape.h |
| duk_hbuffer_external | 16 | duktape.c |
| duk_breakpoint | 16 | duktape.c |
| duk_token | 16 | duktape.c |
| duk__dblval | 16 | duktape.c |
| duk__val | 16 | duktape.c |
| duk_set_top_unsafe | 16 | duktape.c |
| duk_pop_undefined | 16 | duktape.c |
| duk_debug_write_byte | 16 | duktape.c |
| duk_debug_write_error_eom | 16 | duktape.c |
| duk__cbor_decode_error | 16 | duktape.c |
| duk__emit_jump_empty | 16 | duktape.c |
| duk__parse_stmt | 16 | duktape.c |
| duk__get_identifier_reference | 16 | duktape.c |
| duk_get_global_string | 15 | Window.bnd, dukky.c, duktape.c, duktape.h |
| duk_has_prop | 15 | dukky.c, duktape.c, duktape.h |
| duk_del_prop | 15 | Console.bnd, Window.bnd, dukky.c, duktape.c, duktape.h |
| duk_api_global_filename | 15 | duktape.c, duktape.h |
| duk_api_global_line | 15 | duktape.c, duktape.h |
| duk_push_heapptr | 15 | duktape.c, duktape.h |
| duk_is_object | 15 | duktape.c, duktape.h |
| duk__bswaptmp1 | 15 | duktape.c |
| duk__bswaptmp2 | 15 | duktape.c |
| duk_jmpbuf | 15 | duktape.c |
| duk_lexer_point | 15 | duktape.c |
| duk_lc_digits | 15 | duktape.c |
| duk_hex_dectab | 15 | duktape.c |
| duk_labelinfo | 15 | duktape.c |
| duk_xget_owndataprop_stridx_short | 15 | duktape.c |
| duk_unicode_is_line_terminator | 15 | duktape.c |
| duk__get_own_propdesc_raw | 15 | duktape.c |
| duk__exprtop | 15 | duktape.c |
| duk__parse_stmts | 15 | duktape.c |
| duk_push_false | 14 | duktape.c, duktape.h |
| duk_eval_raw | 14 | duktape.c, duktape.h |
| duk__thr | 14 | duktape.c |
| duk_dup_m2 | 14 | duktape.c |
| duk_push_string_tval_readable | 14 | duktape.c |
| duk_xdef_prop_index_wec | 14 | duktape.c |
| duk_hobject_find_entry_tval_ptr_stridx | 14 | duktape.c |
| duk_debug_write_bytes | 14 | duktape.c |
| duk_debug_write_heapptr | 14 | duktape.c |
| duk_debug_is_attached | 14 | duktape.c |
| duk__base64_enctab_fast | 14 | duktape.c |
| duk__print_hobject | 14 | duktape.c |
| duk__emit_op_only | 14 | duktape.c |
| duk__append_u32 | 14 | duktape.c |
| duk_is_callable | 13 | duktape.c, duktape.h |
| duk_compile_raw | 13 | duktape.c, duktape.h |
| duk_memcmp | 13 | duktape.c |
| duk_ispec | 13 | duktape.c |
| duk_is_bare_object | 13 | duktape.c |
| duk_known_hbuffer | 13 | duktape.c |
| duk_xdef_prop_stridx | 13 | duktape.c |
| duk_debug_write_tval | 13 | duktape.c |
| duk__decode_context | 13 | duktape.c |
| duk__json_enc_newline_indent | 13 | duktape.c |
| duk__bi_set_small | 13 | duktape.c |
| duk__bi_mul_small | 13 | duktape.c |
| duk__match_regexp | 13 | duktape.c |
| duk_pop_3 | 12 | EventTarget.bnd, dukky.c, duktape.c, duktape.h |
| duk_is_constructor_call | 12 | duktape.c, duktape.h |
| duk_push_nan | 12 | duktape.c, duktape.h |
| duk_is_buffer | 12 | duktape.c, duktape.h |
| duk_get_number | 12 | duktape.c, duktape.h |
| duk_hstring_external | 12 | duktape.c |
| duk_strcache_entry | 12 | duktape.c |
| duk_re_matcher_ctx | 12 | duktape.c |
| duk__d | 12 | duktape.c |
| duk_to_boolean_top_pop | 12 | duktape.c |
| duk_push_bufobj_raw | 12 | duktape.c |
| duk_hobject_get_own_propdesc | 12 | duktape.c |
| duk_hthread_activation_unwind_norz | 12 | duktape.c |
| duk_heap_strcache_offset_char2byte | 12 | duktape.c |
| duk_debug_set_paused | 12 | duktape.c |
| duk_handle_call_unprotected | 12 | duktape.c |
| duk__uni_range_match | 12 | duktape.c |
| duk__json_dec_syntax_error | 12 | duktape.c |
| duk_uintptr_t | 12 | duk_config.h, duktape.c |
| duk__mark_heaphdr_nonnull | 12 | duktape.c |
| duk__numconv_stringify_ctx | 12 | duktape.c |
| duk_get_uint | 11 | CanvasRenderingContext2D.bnd, duktape.c, duktape.h |
| duk_enum | 11 | duktape.c, duktape.h |
| duk_error_raw | 11 | duktape.c, duktape.h |
| duk_push_bare_object | 11 | duktape.c, duktape.h |
| duk_substring | 11 | duktape.c, duktape.h |
| duk_heaphdr_string | 11 | duktape.c |
| duk_bitencoder_ctx | 11 | duktape.c |
| duk_hex_enctab | 11 | duktape.c |
| duk_require_hobject_promote_mask | 11 | duktape.c |
| duk_pop_n_unsafe | 11 | duktape.c |
| duk_hobject_getprop | 11 | duktape.c |
| duk_hobject_putprop | 11 | duktape.c |
| duk_hbuffer_resize | 11 | duktape.c |
| duk_heap_mark_and_sweep | 11 | duktape.c |
| duk_debug_read_bytes | 11 | duktape.c |
| duk_err_range | 11 | duktape.c |
| duk_unicode_decode_xutf8_checked | 11 | duktape.c |
| duk_unicode_is_identifier_part | 11 | duktape.c |
| duk_native_stack_check | 11 | duktape.c |
| duk__base64_dectab_fast | 11 | duktape.c |
| duk_ptrdiff_t | 11 | duk_config.h, duktape.c |
| duk__require_bufobj_this | 11 | duktape.c |
| duk__mark_tval | 11 | duktape.c |
| duk__assert_walk_list | 11 | duktape.c |
| duk__sort_key_t | 11 | duktape.c |
| duk__getconst | 11 | duktape.c |
| duk__lookup_lhs | 11 | duktape.c |
| duk__inp_get_cp | 11 | duktape.c |
| duk_get_string | 10 | dukky.c, duktape.c, duktape.h |
| duk_get_type | 10 | EventTarget.bnd, duktape.c, duktape.h |
| duk_config | 10 | duk_config.h, duktape.c, duktape.h |
| duk_alloc_function | 10 | duktape.c, duktape.h |
| duk_realloc_function | 10 | duktape.c, duktape.h |
| duk_free_function | 10 | duktape.c, duktape.h |
| duk_get_error_code | 10 | duktape.c, duktape.h |
| duk_error_va_raw | 10 | duktape.c, duktape.h |
| duk_normalize_index | 10 | duktape.c, duktape.h |
| duk_push_bare_array | 10 | duktape.c, duktape.h |
| duk_push_fixed_buffer | 10 | duktape.c, duktape.h |
| duk_float_union | 10 | duktape.c |
| duk_ljstate | 10 | duktape.c |
| duk_uc_nybbles | 10 | duktape.c |
| duk_dup_1 | 10 | duktape.c |
| duk_push_this_coercible_to_object | 10 | duktape.c |
| duk_to_hobject | 10 | duktape.c |
| duk_pop_nodecref_unsafe | 10 | duktape.c |
| duk_hobject_delprop_raw | 10 | duktape.c |
| duk_hobject_define_property_internal | 10 | duktape.c |
| duk_heap_mem_alloc | 10 | duktape.c |
| duk_debug_skip_bytes | 10 | duktape.c |
| duk_debug_write_cstring | 10 | duktape.c |
| duk_debug_write_hstring | 10 | duktape.c |
| duk_err_handle_error_fmt | 10 | duktape.c |
| duk_err_longjmp | 10 | duktape.c |
| duk_unicode_is_identifier_start | 10 | duktape.c |
| duk_js_toboolean | 10 | duktape.c |
| duk__push_this_obj_len_u32 | 10 | duktape.c |
| duk__cbor_get_reserve | 10 | duktape.c |
| duk__json_dec_req_stridx | 10 | duktape.c |
| duk__abandon_array_part | 10 | duktape.c |
| duk__ivalue_toplain_ignore | 10 | duktape.c |
| duk__expr_toforcedreg | 10 | duktape.c |
| duk__bval | 10 | duktape.c |
| duk__insert_jump_offset | 10 | duktape.c |
| duk_get_type_mask | 9 | duktape.c, duktape.h |
| duk_copy | 9 | duktape.c, duktape.h |
| duk_push_dynamic_buffer | 9 | duktape.c, duktape.h |
| duk_require_lstring | 9 | DOMSettableTokenList.bnd, DOMTokenList.bnd, duktape.c, duktape.h |
| duk_require_constructor_call | 9 | duktape.c, duktape.h |
| duk_to_undefined | 9 | duktape.c, duktape.h |
| duk_to_int32 | 9 | duktape.c, duktape.h |
| duk_get_prop_literal | 9 | duktape.c, duktape.h |
| duk_internal_exception | 9 | duktape.c |
| duk_litcache_entry | 9 | duktape.c |
| duk_unicode_encode_xutf8 | 9 | duktape.c |
| duk__valsz | 9 | duktape.c |
| duk_regexp_match | 9 | duktape.c |
| duk_dup_m4 | 9 | duktape.c |
| duk_push_uarridx | 9 | duktape.c |
| duk_has_prop_stridx | 9 | duktape.c |
| duk_xdef_prop | 9 | duktape.c |
| duk_hobject_realloc_props | 9 | duktape.c |
| duk_hobject_compact_props | 9 | duktape.c |
| duk_hthread_catcher_unwind_norz | 9 | duktape.c |
| duk_heap_process_finalize_list | 9 | duktape.c |
| duk_debug_read_int | 9 | duktape.c |
| duk_debug_write_hobject | 9 | duktape.c |
| duk_fb_is_full | 9 | duktape.c |
| duk_err_error | 9 | duktape.c |
| duk_err_type | 9 | duktape.c |
| duk_err_create_and_throw | 9 | duktape.c |
| duk_unicode_ids_noa | 9 | duktape.c |
| duk_unicode_ids_noabmp | 9 | duktape.c |
| duk_unicode_decode_xutf8 | 9 | duktape.c |
| duk_unicode_re_canonicalize_char | 9 | duktape.c |
| duk_js_compare_helper | 9 | duktape.c |
| duk_js_isarray_hobject | 9 | duktape.c |
| duk__err_shared | 9 | duktape.c |
| duk__tag_check | 9 | duktape.c |
| duk__require_bufobj_value | 9 | duktape.c |
| duk__cbor_decode_value | 9 | duktape.c |
| duk__cbor_encode_uint32 | 9 | duktape.c |
| duk__hobject_alloc_init | 9 | duktape.c |
| duk__emit_jump | 9 | duktape.c |
| duk__emit_invalid | 9 | duktape.c |
| duk__ivalue_toregconst_raw | 9 | duktape.c |
| duk__expr_led | 9 | duktape.c |
| duk__id_lookup_result | 9 | duktape.c |
| duk__bi_compare | 9 | duktape.c |
| duk__test_double_union | 9 | duktape.c |
| duk_call | 8 | dukky.c, duktape.c, duktape.h |
| duk_push_c_function | 8 | Window.bnd, dukky.c, duktape.c, duktape.h |
| duk_put_global_string | 8 | dukky.c, duktape.c, duktape.h |
| duk_strict_equals | 8 | EventTarget.bnd, duktape.c, duktape.h |
| duk_to_lstring | 8 | Element.bnd, EventTarget.bnd, duktape.c, duktape.h |
| duk_thread_state | 8 | duktape.c, duktape.h |
| duk_time_components | 8 | duktape.c, duktape.h |
| duk_push_literal_raw | 8 | duktape.c, duktape.h |
| duk_push_error_object_va_raw | 8 | duktape.c, duktape.h |
| duk_push_buffer_raw | 8 | duktape.c, duktape.h |
| duk_require_buffer | 8 | duktape.c, duktape.h |
| duk_set_length | 8 | duktape.c, duktape.h |
| duk_next | 8 | duktape.c, duktape.h |
| duk_join | 8 | duktape.c, duktape.h |
| duk_fatal_exception | 8 | duktape.c |
| duk_re_token | 8 | duktape.c |
| duk__cp | 8 | duktape.c |
| duk_unicode_encode_cesu8 | 8 | duktape.c |
| duk_be_encode | 8 | duktape.c |
| duk_memmove_unsafe | 8 | duktape.c |
| duk_memset | 8 | duktape.c |
| duk_heaphdr_decref | 8 | duktape.c |
| duk_hobject_refzero | 8 | duktape.c |
| duk_hobject_refzero_norz | 8 | duktape.c |
| duk_dup_2 | 8 | duktape.c |
| duk_get_type_name | 8 | duktape.c |
| duk_is_string_notsymbol | 8 | duktape.c |
| duk_to_number_m2 | 8 | duktape.c |
| duk_to_property_key_hstring | 8 | duktape.c |
| duk_require_hstring_notsymbol | 8 | duktape.c |
| duk_require_hbuffer | 8 | duktape.c |
| duk_push_hobject_bidx | 8 | duktape.c |
| duk_hobject_find_entry | 8 | duktape.c |
| duk_hobject_pc2line_query | 8 | duktape.c |
| duk_handle_safe_call | 8 | duktape.c |
| duk_debug_write_null | 8 | duktape.c |
| duk_debug_log | 8 | duktape.c |
| duk_err_handle_error | 8 | duktape.c |
| duk_unicode_unvalidated_utf8_length | 8 | duktape.c |
| duk_js_tonumber | 8 | duktape.c |
| duk_js_getvar_activation | 8 | duktape.c |
| duk_js_push_closure | 8 | duktape.c |
| duk_numconv_stringify | 8 | duktape.c |
| duk_bi_date_get_timeval_from_dparts | 8 | duktape.c |
| duk__put_prop_shared | 8 | duktape.c |
| duk__convert_systime_to_ularge | 8 | duktape.c |
| duk__transform_context | 8 | duktape.c |
| duk__debug_read_uint32_raw | 8 | duktape.c |
| duk__debug_write_hstring_safe_top | 8 | duktape.c |
| duk__tval_number_to_arr_idx | 8 | duktape.c |
| duk__tval_fastint_to_arr_idx | 8 | duktape.c |
| duk__get_instr_ptr | 8 | duktape.c |
| duk__expr_nud | 8 | duktape.c |
| duk__exprtop_toregconst | 8 | duktape.c |
| duk__parse_func_like_fnum | 8 | duktape.c |
| duk__bi_copy | 8 | duktape.c |
| duk__bi_mul_small_copy | 8 | duktape.c |
| duk_memory_functions | 7 | dukky.c, duktape.c, duktape.h |
| duk_pcall | 7 | dukky.c, duktape.c, duktape.h |
| duk_get_boolean | 7 | Event.bnd, dukky.c, duktape.c, duktape.h |
| duk_pcall_method | 7 | dukky.c, duktape.c, duktape.h |
| duk_function_list_entry | 7 | duktape.c, duktape.h |
| duk_number_list_entry | 7 | duktape.c, duktape.h |
| duk_throw | 7 | duktape.c, duktape.h |
| duk_push_buffer_object | 7 | ImageData.bnd, duktape.c, duktape.h |
| duk_require_type_mask | 7 | duktape.c, duktape.h |
| duk_require_buffer_data | 7 | duktape.c, duktape.h |
| duk_require_callable | 7 | duktape.c, duktape.h |
| duk_resize_buffer | 7 | duktape.c, duktape.h |
| duk_new | 7 | duktape.c, duktape.h |
| duk_debugger_attach | 7 | duktape.c, duktape.h |
| duk_bi_global_object_eval | 7 | duktape.c |
| duk_bi_thread_resume | 7 | duktape.c |
| duk_builtins_data | 7 | duktape.c |
| duk_unicode_get_xutf8_length | 7 | duktape.c |
| duk_bd_decode_flag | 7 | duktape.c |
| duk_bd_decode_flagged | 7 | duktape.c |
| duk_memcmp_unsafe | 7 | duktape.c |
| duk_double_is_nan | 7 | duktape.c |
| duk_heaphdr_refzero | 7 | duktape.c |
| duk_heaphdr_refzero_norz | 7 | duktape.c |
| duk_heaphdr_decref_norz | 7 | duktape.c |
| duk_push_uint_to_hstring | 7 | duktape.c |
| duk_get_borrowed_this_tval | 7 | duktape.c |
| duk_to_hstring_m1 | 7 | duktape.c |
| duk_push_hcompfunc | 7 | duktape.c |
| duk_push_fixed_buffer_zero | 7 | duktape.c |
| duk_get_prop_stridx_boolean | 7 | duktape.c |
| duk_xdef_prop_stridx_thrower | 7 | duktape.c |
| duk_pop_n_nodecref_unsafe | 7 | duktape.c |
| duk_compact_m1 | 7 | duktape.c |
| duk_hstring_get_charlen | 7 | duktape.c |
| duk_hdecenv_alloc | 7 | duktape.c |
| duk_hobject_hasprop | 7 | duktape.c |
| duk_hobject_hasprop_raw | 7 | duktape.c |
| duk_hbufobj_validated_write | 7 | duktape.c |
| duk_hthread_get_activation_for_level | 7 | duktape.c |
| duk_handle_call | 7 | duktape.c |
| duk_debug_read_hstring | 7 | duktape.c |
| duk_debug_process_messages | 7 | duktape.c |
| duk_err_augment_error_throw | 7 | duktape.c |
| duk_err_setup_ljstate1 | 7 | duktape.c |
| duk_js_equals_helper | 7 | duktape.c |
| duk_js_samevalue | 7 | duktape.c |
| duk_js_putvar_activation | 7 | duktape.c |
| duk_js_init_activation_environment_records_delayed | 7 | duktape.c |
| duk_numconv_parse | 7 | duktape.c |
| duk_bi_date_timeval_to_parts | 7 | duktape.c |
| duk_bi_json_stringify_helper | 7 | duktape.c |
| duk__compile_raw_args | 7 | duktape.c |
| duk_internal_thread_state | 7 | duktape.c |
| duk__cbor_decode_readbyte | 7 | duktape.c |
| duk__cbor_decode_read_u32 | 7 | duktape.c |
| duk__cbor_decode_consume | 7 | duktape.c |
| duk__push_this_get_timeval | 7 | duktape.c |
| duk__transform_helper | 7 | duktape.c |
| duk__json_dec_get | 7 | duktape.c |
| duk__json_dec_get_nonwhite | 7 | duktape.c |
| duk__json_dec_value | 7 | duktape.c |
| duk__emit_esc_auto_fast | 7 | duktape.c |
| duk__print_hbuffer | 7 | duktape.c |
| duk__hobject_alloc_entry_checked | 7 | duktape.c |
| duk__ivalue_toplain_raw | 7 | duktape.c |
| duk__ivalue_totempconst | 7 | duktape.c |
| duk__expr_lbp | 7 | duktape.c |
| duk__expr_toregconst | 7 | duktape.c |
| duk__tvdst | 7 | duktape.c |
| duk__advance_bytes | 7 | duktape.c |
| duk__emit_u16_direct_ranges | 7 | duktape.c |
| duk__bi_add | 7 | duktape.c |
| duk__bc_get_i32 | 7 | duktape.c |
| duk_fatal_function | 6 | duktape.c, duktape.h |
| duk_safe_call_function | 6 | duktape.c, duktape.h |
| duk_debug_detached_function | 6 | duktape.c, duktape.h |
| duk_get_top_index | 6 | duktape.c, duktape.h |
| duk_check_stack | 6 | duktape.c, duktape.h |
| duk_is_null | 6 | duktape.c, duktape.h |
| duk_is_null_or_undefined | 6 | duktape.c, duktape.h |
| duk_is_nan | 6 | duktape.c, duktape.h |
| duk_to_uint | 6 | DOMTokenList.bnd, NamedNodeMap.bnd, NodeList.bnd, duktape.c, duktape.h |
| duk_hex_decode | 6 | duktape.c, duktape.h |
| duk_set_finalizer | 6 | duktape.c, duktape.h |
| duk_compact | 6 | duktape.c, duktape.h |
| duk_propaccessor | 6 | duktape.c |
| duk_bi_function_prototype_apply | 6 | duktape.c |
| duk_bi_function_prototype_call | 6 | duktape.c |
| duk_bi_thread_yield | 6 | duktape.c |
| duk_bi_reflect_apply | 6 | duktape.c |
| duk_bi_reflect_construct | 6 | duktape.c |
| duk_bi_native_functions | 6 | duktape.c |
| duk__enc_len | 6 | duktape.c |
| duk__valptr | 6 | duktape.c |
| duk_hex_dectab_shift4 | 6 | duktape.c |
| duk_util_hashbytes | 6 | duktape.c |
| duk_heaphdr_incref | 6 | duktape.c |
| duk_set_top_and_wipe | 6 | duktape.c |
| duk_dup_m3 | 6 | duktape.c |
| duk_get_hstring_notsymbol | 6 | duktape.c |
| duk_get_buffer_data_raw | 6 | duktape.c |
| duk_require_hobject_accept_mask | 6 | duktape.c |
| duk_to_int_check_range | 6 | duktape.c |
| duk_push_harray_with_size_outptr | 6 | duktape.c |
| duk_push_string_readable | 6 | duktape.c |
| duk_put_prop_stridx | 6 | duktape.c |
| duk_hobject_has_finalizer_fast_raw | 6 | duktape.c |
| duk_hobjenv_alloc | 6 | duktape.c |
| duk_hobject_find_entry_tval_ptr | 6 | duktape.c |
| duk_hobject_get_length | 6 | duktape.c |
| duk_hobject_get_varmap | 6 | duktape.c |
| duk_hobject_prototype_chain_contains | 6 | duktape.c |
| duk_hbufobj_push_validated_read | 6 | duktape.c |
| duk_hthread_get_act_prev_pc | 6 | duktape.c |
| duk_heap_mem_realloc | 6 | duktape.c |
| duk_heap_mem_realloc_indirect | 6 | duktape.c |
| duk_free_hstring | 6 | duktape.c |
| duk_heap_free_heaphdr_raw | 6 | duktape.c |
| duk_heap_hashstring | 6 | duktape.c |
| duk_debug_skip_byte | 6 | duktape.c |
| duk_debug_write_notify | 6 | duktape.c |
| duk_debug_is_paused | 6 | duktape.c |
| duk_debug_vsnprintf | 6 | duktape.c |
| duk_err_require_type_index | 6 | duktape.c |
| duk_unicode_re_ranges_digit | 6 | duktape.c |
| duk_unicode_re_ranges_white | 6 | duktape.c |
| duk_unicode_re_ranges_wordchar | 6 | duktape.c |
| duk_js_tointeger_number | 6 | duktape.c |
| duk_js_tointeger | 6 | duktape.c |
| duk_js_arith_pow | 6 | duktape.c |
| duk_call_construct_postprocess | 6 | duktape.c |
| duk_bi_json_parse_helper | 6 | duktape.c |
| duk_uint_fast16_t | 6 | duk_config.h, duktape.c |
| duk__load_string_raw | 6 | duktape.c |
| duk__pcall_prop_args | 6 | duktape.c |
| duk__pcall_method_args | 6 | duktape.c |
| duk__pcall_args | 6 | duktape.c |
| duk__base64_encode_fast_3 | 6 | duktape.c |
| duk__push_c_function_raw | 6 | duktape.c |
| duk__known_heaphdr | 6 | duktape.c |
| duk__getrequire_bufobj_this | 6 | duktape.c |
| duk__set_bufobj_buffer | 6 | duktape.c |
| duk__cbor_encode_value | 6 | duktape.c |
| duk__cbor_encode_ensure | 6 | duktape.c |
| duk__timeclip | 6 | duktape.c |
| duk__json_enc_key_autoquote | 6 | duktape.c |
| duk__json_enc_quote_string | 6 | duktape.c |
| duk__json_enc_value | 6 | duktape.c |
| duk__push_this_number_plain | 6 | duktape.c |
| duk__debug_read_pointer_raw | 6 | duktape.c |
| duk__debug_getinfo_prop_bool | 6 | duktape.c |
| duk__handle_temproot | 6 | duktape.c |
| duk__compact_object_list | 6 | duktape.c |
| duk__refcount_free_pending | 6 | duktape.c |
| duk__to_property_key | 6 | duktape.c |
| duk__emit_abc | 6 | duktape.c |
| duk__emit_load_int32 | 6 | duktape.c |
| duk__emit_if_true_skip | 6 | duktape.c |
| duk__ivalue_toreg | 6 | duktape.c |
| duk__update_label_flags | 6 | duktape.c |
| duk__expr_is_empty | 6 | duktape.c |
| duk__vm_arith_add | 6 | duktape.c |
| duk__prepost_incdec_reg_helper | 6 | duktape.c |
| duk__prepost_incdec_var_helper | 6 | duktape.c |
| duk__act | 6 | duktape.c |
| duk__internbuffer | 6 | duktape.c |
| duk__bi_mul | 6 | duktape.c |
| duk__encode_i32 | 6 | duktape.c |
| duk__parse_disjunction | 6 | duktape.c |
| duk_gc | 5 | dukky.c, duktape.c, duktape.h |
| duk_is_boolean | 5 | dukky.c, duktape.c, duktape.h |
| duk_generic_error | 5 | CanvasRenderingContext2D.bnd, duktape.h |
| duk_debug_read_function | 5 | duktape.c, duktape.h |
| duk_debug_write_function | 5 | duktape.c, duktape.h |
| duk_debug_peek_function | 5 | duktape.c, duktape.h |
| duk_debug_read_flush_function | 5 | duktape.c, duktape.h |
| duk_debug_write_flush_function | 5 | duktape.c, duktape.h |
| duk_debug_request_function | 5 | duktape.c, duktape.h |
| duk_push_thread_raw | 5 | duktape.c, duktape.h |
| duk_type_error | 5 | duktape.c, duktape.h |
| duk_require_valid_index | 5 | duktape.c, duktape.h |
| duk_swap | 5 | duktape.c, duktape.h |
| duk_push_error_object_raw | 5 | duktape.c, duktape.h |
| duk_push_error_object | 5 | Console.bnd, duktape.c, duktape.h |
| duk_is_array | 5 | duktape.c, duktape.h |
| duk_is_function | 5 | duktape.c, duktape.h |
| duk_get_lstring | 5 | duktape.c, duktape.h |
| duk_to_buffer_raw | 5 | duktape.c, duktape.h |
| duk_hex_encode | 5 | duktape.c, duktape.h |
| duk_put_prop_literal | 5 | duktape.c, duktape.h |
| duk_get_prop_desc | 5 | duktape.c, duktape.h |
| duk_decode_string | 5 | duktape.c, duktape.h |
| duk_debugger_pause | 5 | duktape.c, duktape.h |
| duk_computed_nan | 5 | duk_config.h, duktape.c |
| duk_tval_set_number_chkfast_fast | 5 | duktape.c |
| duk_bi_object_prototype_to_string | 5 | duktape.c |
| duk_util_tinyrandom_get_double | 5 | duktape.c |
| duk_util_tinyrandom_prepare_seed | 5 | duktape.c |
| duk_byteswap_bytes | 5 | duktape.c |
| duk_util_get_random_double | 5 | duktape.c |
| duk_double_is_nan_or_zero | 5 | duktape.c |
| duk_double_is_nan_or_inf | 5 | duktape.c |
| duk_double_trunc_towards_zero | 5 | duktape.c |
| duk_double_fmax | 5 | duktape.c |
| duk_re_range_callback | 5 | duktape.c |
| duk_js_compile | 5 | duktape.c |
| duk_regexp_compile | 5 | duktape.c |
| duk_hstring_refzero | 5 | duktape.c |
| duk_hbuffer_refzero | 5 | duktape.c |
| duk_hobject_refcount_finalize_norz | 5 | duktape.c |
| duk_valstack_grow_check_throw | 5 | duktape.c |
| duk_copy_tvals_incref | 5 | duktape.c |
| duk_remove_n | 5 | duktape.c |
| duk_get_type_mask_tval | 5 | duktape.c |
| duk_get_hbuffer | 5 | duktape.c |
| duk_get_hobject_with_class | 5 | duktape.c |
| duk_get_hobject_promote_lfunc | 5 | duktape.c |
| duk_to_number_tval | 5 | duktape.c |
| duk_to_int_clamped_raw | 5 | duktape.c |
| duk_push_hbuffer | 5 | duktape.c |
| duk_push_c_function_builtin_noconstruct | 5 | duktape.c |
| duk_push_lightfunc_name_raw | 5 | duktape.c |
| duk_pack | 5 | duktape.c |
| duk_unpack_array_like | 5 | duktape.c |
| duk_get_top_require_min | 5 | duktape.c |
| duk_get_top_index_unsafe | 5 | duktape.c |
| duk_pop_3_unsafe | 5 | duktape.c |
| duk_seal_freeze_raw | 5 | duktape.c |
| duk_concat_2 | 5 | duktape.c |
| duk_time_get_ecmascript_time | 5 | duktape.c |
| duk_time_get_monotonic_time | 5 | duktape.c |
| duk_hobject_delprop | 5 | duktape.c |
| duk_hobject_define_property_helper | 5 | duktape.c |
| duk_hobject_get_internal_value_tval_ptr | 5 | duktape.c |
| duk_hobject_get_internal_value_string | 5 | duktape.c |
| duk_hobject_get_formals | 5 | duktape.c |
| duk_hobject_proxy_check | 5 | duktape.c |
| duk_hobject_enumerator_create | 5 | duktape.c |
| duk_hbufobj_promote_plain | 5 | duktape.c |
| duk_hthread_create_builtin_objects | 5 | duktape.c |
| duk_hthread_catcher_alloc | 5 | duktape.c |
| duk_hthread_get_act_curr_pc | 5 | duktape.c |
| duk_hthread_sync_and_null_currpc | 5 | duktape.c |
| duk_free_hobject | 5 | duktape.c |
| duk_heap_strtable_intern | 5 | duktape.c |
| duk_heap_strtable_intern_checked | 5 | duktape.c |
| duk_heap_strtable_dump | 5 | duktape.c |
| duk_heap_run_finalizer | 5 | duktape.c |
| duk_debug_read_any_ptr | 5 | duktape.c |
| duk_debug_read_tval | 5 | duktape.c |
| duk_debug_write_unused | 5 | duktape.c |
| duk_debug_write_string | 5 | duktape.c |
| duk_debug_write_buffer | 5 | duktape.c |
| duk_debug_write_pointer | 5 | duktape.c |
| duk_debug_curr_line | 5 | duktape.c |
| duk_debug_halt_execution | 5 | duktape.c |
| duk_debug_clear_paused | 5 | duktape.c |
| duk_debug_file_stash | 5 | duktape.c |
| duk_debug_func_stash | 5 | duktape.c |
| duk_err_augment_error_create | 5 | duktape.c |
| duk_err_check_debugger_integration | 5 | duktape.c |
| duk_unicode_ids_m_let_noa | 5 | duktape.c |
| duk_unicode_ids_m_let_noabmp | 5 | duktape.c |
| duk_unicode_idp_m_ids_noa | 5 | duktape.c |
| duk_unicode_idp_m_ids_noabmp | 5 | duktape.c |
| duk_unicode_caseconv_uc | 5 | duktape.c |
| duk_unicode_caseconv_lc | 5 | duktape.c |
| duk_unicode_re_canon_bitmap | 5 | duktape.c |
| duk_unicode_is_whitespace | 5 | duktape.c |
| duk_js_data_compare | 5 | duktape.c |
| duk_js_typeof_stridx | 5 | duktape.c |
| duk_handle_call_unprotected_nargs | 5 | duktape.c |
| duk_bi_date_is_leap_year | 5 | duktape.c |
| duk_bi_date_timeval_in_leeway_range | 5 | duktape.c |
| duk__u16_union | 5 | duktape.c |
| duk__u32_union | 5 | duktape.c |
| duk__dump_hstring_raw | 5 | duktape.c |
| duk__prep_codec_arg | 5 | duktape.c |
| duk__api_coerce_d2i | 5 | duktape.c |
| duk__api_coerce_d2ui | 5 | duktape.c |
| duk__push_this_helper | 5 | duktape.c |
| duk__push_this_obj_len_u32_limited | 5 | duktape.c |
| duk__array_sort_compare | 5 | duktape.c |
| duk__array_qsort | 5 | duktape.c |
| duk__buffer_elemtype_copy_compatible | 5 | duktape.c |
| duk__cbor_encode_error | 5 | duktape.c |
| duk__cbor_encode_sizet_uint32_check | 5 | duktape.c |
| duk__cbor_decode_push_aival_int | 5 | duktape.c |
| duk__cbor_decode_aival_uint32 | 5 | duktape.c |
| duk__cbor_decode_buffer | 5 | duktape.c |
| duk__cbor_decode_string | 5 | duktape.c |
| duk__cbor_decode_array | 5 | duktape.c |
| duk__cbor_decode_map | 5 | duktape.c |
| duk__set_systime_jan1970 | 5 | duktape.c |
| duk__decode_hex_escape | 5 | duktape.c |
| duk__json_dec_decode_hex_escape | 5 | duktape.c |
| duk__json_dec_objarr_entry | 5 | duktape.c |
| duk__json_dec_objarr_exit | 5 | duktape.c |
| duk__json_dec_reviver_walk | 5 | duktape.c |
| duk__json_enc_object | 5 | duktape.c |
| duk__two_arg_func | 5 | duktape.c |
| duk__object_keys_enum_flags | 5 | duktape.c |
| duk__print_hstring | 5 | duktape.c |
| duk__print_tval | 5 | duktape.c |
| duk__debug_read_uint16_raw | 5 | duktape.c |
| duk__debug_read_hstring_raw | 5 | duktape.c |
| duk__debug_getinfo_prop_int | 5 | duktape.c |
| duk__debug_getinfo_bitmask | 5 | duktape.c |
| duk__init_object_parts | 5 | duktape.c |
| duk__add_enum_key | 5 | duktape.c |
| duk__get_propdesc | 5 | duktape.c |
| duk__get_default_h_size | 5 | duktape.c |
| duk__obtain_arridx_slot | 5 | duktape.c |
| duk__proxy_check_prop | 5 | duktape.c |
| duk__emit_load_int32_noshuffle | 5 | duktape.c |
| duk__ivalue_plain_fromstack | 5 | duktape.c |
| duk__copy_ispec | 5 | duktape.c |
| duk__copy_ivalue | 5 | duktape.c |
| duk__ivalue_toplain | 5 | duktape.c |
| duk__ivalue_toregconst | 5 | duktape.c |
| duk__expr_toplain | 5 | duktape.c |
| duk__parse_var_decl | 5 | duktape.c |
| duk__parse_func_body | 5 | duktape.c |
| duk__token_lbp | 5 | duktape.c |
| duk__hstring_is_eval_or_arguments_in_strict_mode | 5 | duktape.c |
| duk__objlit_load_key | 5 | duktape.c |
| duk__tvsrc | 5 | duktape.c |
| duk__lexer_parse_escape | 5 | duktape.c |
| duk__lexer_skip_to_endofline | 5 | duktape.c |
| duk__str2num_digits_for_radix | 5 | duktape.c |
| duk__bi_normalize | 5 | duktape.c |
| duk__bi_exp_small | 5 | duktape.c |
| duk__append_7bit | 5 | duktape.c |
| duk__bw_update_ptrs | 5 | duktape.c |
| duk_set_prototype | 4 | dukky.c, duktape.c, duktape.h |
| duk_create_heap | 4 | dukky.c, duktape.c, duktape.h |
| duk_require_context | 4 | dukky.c, duktape.c, duktape.h |
| duk_pcompile_lstring_filename | 4 | dukky.c, duktape.h |
| duk_del_prop_string | 4 | dukky.c, duktape.c, duktape.h |
| duk_push_context_dump | 4 | dukky.c, duktape.c, duktape.h |
| duk_suspend | 4 | duktape.c, duktape.h |
| duk_fatal | 4 | duktape.c, duktape.h |
| duk_is_strict_call | 4 | duktape.c, duktape.h |
| duk_xcopymove_raw | 4 | duktape.c, duktape.h |
| duk_push_vsprintf | 4 | duktape.c, duktape.h |
| duk_push_proxy | 4 | duktape.c, duktape.h |
| duk_is_buffer_data | 4 | duktape.c, duktape.h |
| duk_is_symbol | 4 | duktape.c, duktape.h |
| duk_is_constructable | 4 | duktape.c, duktape.h |
| duk_get_buffer_data | 4 | duktape.c, duktape.h |
| duk_get_c_function | 4 | duktape.c, duktape.h |
| duk_require_number | 4 | duktape.c, duktape.h |
| duk_require_int | 4 | duktape.c, duktape.h |
| duk_require_uint | 4 | duktape.c, duktape.h |
| duk_require_string | 4 | duktape.c, duktape.h |
| duk_base64_encode | 4 | duktape.c, duktape.h |
| duk_base64_decode | 4 | duktape.c, duktape.h |
| duk_get_prop_literal_raw | 4 | duktape.c, duktape.h |
| duk_put_prop_literal_raw | 4 | duktape.c, duktape.h |
| duk_has_prop_index | 4 | duktape.c, duktape.h |
| duk_get_finalizer | 4 | duktape.c, duktape.h |
| duk_set_magic | 4 | duktape.c, duktape.h |
| duk_trim | 4 | duktape.c, duktape.h |
| duk_debugger_detach | 4 | duktape.c, duktape.h |
| duk_debugger_cooperate | 4 | duktape.c, duktape.h |
| duk_debugger_notify | 4 | duktape.c, duktape.h |
| duk__set_part_helper | 4 | duktape.c, duktape.h |
| duk_computed_infinity | 4 | duk_config.h, duktape.c |
| duk_tval_unused | 4 | duktape.c |
| duk_tval_set_number_chkfast_slow | 4 | duktape.c |
| duk_bi_function_prototype | 4 | duktape.c |
| duk_bi_typedarray_constructor | 4 | duktape.c |
| duk_bi_object_constructor_define_properties | 4 | duktape.c |
| duk_bi_number_prototype_to_string | 4 | duktape.c |
| duk_bi_date_prototype_value_of | 4 | duktape.c |
| duk_bi_error_prototype_stack_getter | 4 | duktape.c |
| duk_bi_error_prototype_filename_getter | 4 | duktape.c |
| duk_bi_error_prototype_linenumber_getter | 4 | duktape.c |
| duk_bi_error_prototype_to_string | 4 | duktape.c |
| duk_bi_typedarray_bytelength_getter | 4 | duktape.c |
| duk_bi_typedarray_byteoffset_getter | 4 | duktape.c |
| duk_bi_typedarray_buffer_getter | 4 | duktape.c |
| duk_bi_cbor_encode | 4 | duktape.c |
| duk_bi_cbor_decode | 4 | duktape.c |
| duk_raw_write_u16_be | 4 | duktape.c |
| duk_raw_write_u32_be | 4 | duktape.c |
| duk_raw_write_float_be | 4 | duktape.c |
| duk_raw_write_double_be | 4 | duktape.c |
| duk_raw_read_u16_be | 4 | duktape.c |
| duk_raw_read_u32_be | 4 | duktape.c |
| duk_raw_read_double_be | 4 | duktape.c |
| duk_dblunion_host_to_little | 4 | duktape.c |
| duk_dblunion_little_to_host | 4 | duktape.c |
| duk_dblunion_host_to_big | 4 | duktape.c |
| duk_dblunion_big_to_host | 4 | duktape.c |
| duk_fltunion_host_to_big | 4 | duktape.c |
| duk_fltunion_big_to_host | 4 | duktape.c |
| duk_bw_compact | 4 | duktape.c |
| duk_bw_assert_valid | 4 | duktape.c |
| duk__sz | 4 | duktape.c |
| duk_bw_resize | 4 | duktape.c |
| duk_bw_write_raw_slice | 4 | duktape.c |
| duk_bw_insert_raw_bytes | 4 | duktape.c |
| duk_bw_insert_raw_slice | 4 | duktape.c |
| duk_bw_insert_raw_area | 4 | duktape.c |
| duk_bd_decode_flagged_signed | 4 | duktape.c |
| duk_bd_decode_bitpacked_string | 4 | duktape.c |
| duk_memset_unsafe | 4 | duktape.c |
| duk_double_fmin | 4 | duktape.c |
| duk_double_is_integer | 4 | duktape.c |
| duk_double_div | 4 | duktape.c |
| duk_double_to_int32_t | 4 | duktape.c |
| duk_double_to_uint32_t | 4 | duktape.c |
| duk_lexer | 4 | duktape.c |
| duk_regexp_create_instance | 4 | duktape.c |
| duk_tvals | 4 | duktape.c |
| duk_heaphdr_assert_valid_subclassed | 4 | duktape.c |
| duk_heaphdr_assert_links | 4 | duktape.c |
| duk_valstack_grow_check_nothrow | 4 | duktape.c |
| duk_reserve_gap | 4 | duktape.c |
| duk_remove_unsafe | 4 | duktape.c |
| duk_remove_n_unsafe | 4 | duktape.c |
| duk_get_type_tval | 4 | duktape.c |
| duk_get_class_number | 4 | duktape.c |
| duk_get_string_notsymbol | 4 | duktape.c |
| duk_get_hobject_promote_mask | 4 | duktape.c |
| duk_known_hnatfunc | 4 | duktape.c |
| duk_to_hstring_acceptsymbol | 4 | duktape.c |
| duk_push_class_string_tval | 4 | duktape.c |
| duk_require_hthread | 4 | duktape.c |
| duk_require_hobject_with_class | 4 | duktape.c |
| duk_push_object_helper_proto | 4 | duktape.c |
| duk_push_harray | 4 | duktape.c |
| duk_push_harray_with_size | 4 | duktape.c |
| duk_push_string_funcptr | 4 | duktape.c |
| duk_push_lightfunc_tostring | 4 | duktape.c |
| duk_del_prop_stridx | 4 | duktape.c |
| duk_xdef_prop_index | 4 | duktape.c |
| duk_xdef_prop_stridx_short_wec | 4 | duktape.c |
| duk_get_method_stridx | 4 | duktape.c |
| duk_push_symbol_descriptive_string | 4 | duktape.c |
| duk_pop_2_nodecref_unsafe | 4 | duktape.c |
| duk_insert_undefined | 4 | duktape.c |
| duk_pcall_method_flags | 4 | duktape.c |
| duk_time_get_ecmascript_time_nofrac | 4 | duktape.c |
| duk_js_to_arrayindex_hstring_fast_known | 4 | duktape.c |
| duk_hstring_char_code_at_raw | 4 | duktape.c |
| duk_harray_alloc | 4 | duktape.c |
| duk_hthread_alloc_unchecked | 4 | duktape.c |
| duk_hthread_alloc | 4 | duktape.c |
| duk_hobject_resize_entrypart | 4 | duktape.c |
| duk_hobject_find_array_entry_tval_ptr | 4 | duktape.c |
| duk_hobject_prepare_property_descriptor | 4 | duktape.c |
| duk_hobject_object_get_own_property_descriptor | 4 | duktape.c |
| duk_hobject_object_ownprop_helper | 4 | duktape.c |
| duk_hobject_resolve_proxy_target | 4 | duktape.c |
| duk_hobject_get_enumerated_keys | 4 | duktape.c |
| duk_hobject_enumerator_next | 4 | duktape.c |
| duk_hthread_init_stacks | 4 | duktape.c |
| duk_hthread_terminate | 4 | duktape.c |
| duk_hthread_activation_alloc | 4 | duktape.c |
| duk_hthread_catcher_free | 4 | duktape.c |
| duk_hthread_valstack_torture_realloc | 4 | duktape.c |
| duk__v | 4 | duktape.c |
| duk_mem_getptr | 4 | duktape.c |
| duk_heap_mem_free | 4 | duktape.c |
| duk_heap_free | 4 | duktape.c |
| duk_free_hbuffer | 4 | duktape.c |
| duk_heap_strtable_intern_u32 | 4 | duktape.c |
| duk_heap_strtable_intern_u32_checked | 4 | duktape.c |
| duk_heap_strcache_string_remove | 4 | duktape.c |
| duk_heap_free_freelists | 4 | duktape.c |
| duk_debug_do_detach | 4 | duktape.c |
| duk_debug_write_flush | 4 | duktape.c |
| duk_debug_write_undefined | 4 | duktape.c |
| duk_debug_write_boolean | 4 | duktape.c |
| duk_debug_write_hbuffer | 4 | duktape.c |
| duk_debug_clear_pause_state | 4 | duktape.c |
| duk_debug_line_stash | 4 | duktape.c |
| duk_debug_level_stash | 4 | duktape.c |
| duk_fb_put_bytes | 4 | duktape.c |
| duk_fb_put_funcptr | 4 | duktape.c |
| duk_default_fatal_handler | 4 | duktape.c |
| duk_error_throw_from_negative_rc | 4 | duktape.c |
| duk_unicode_re_ranges_not_digit | 4 | duktape.c |
| duk_unicode_re_ranges_not_white | 4 | duktape.c |
| duk_unicode_re_ranges_not_wordchar | 4 | duktape.c |
| duk_is_idchar_tab | 4 | duktape.c |
| duk_unicode_is_letter | 4 | duktape.c |
| duk_unicode_re_is_wordchar | 4 | duktape.c |
| duk_js_touint32 | 4 | duktape.c |
| duk_js_to_arrayindex_string | 4 | duktape.c |
| duk_js_string_compare | 4 | duktape.c |
| duk_js_instanceof | 4 | duktape.c |
| duk_js_isarray | 4 | duktape.c |
| duk_js_equals | 4 | duktape.c |
| duk_js_strict_equals | 4 | duktape.c |
| duk_js_putvar_envrec | 4 | duktape.c |
| duk_create_activation_environment_record | 4 | duktape.c |
| duk_call_setup_propcall_error | 4 | duktape.c |
| duk_bi_date_get_local_tzoffset_windows | 4 | duk_config.h, duktape.c |
| duk_proxy_ownkeys_postprocess | 4 | duktape.c |
| duk__call_get_idx_func | 4 | duktape.c |
| duk__call_prop_prep_stack | 4 | duktape.c |
| duk__resize_valstack | 4 | duktape.c |
| duk__get_buffer_helper | 4 | duktape.c |
| duk__get_hobject_promote_mask_raw | 4 | duktape.c |
| duk__obj_flag_any_default_false | 4 | duktape.c |
| duk__push_stash | 4 | duktape.c |
| duk__push_hstring_readable_unicode | 4 | duktape.c |
| duk__array_sort_swap | 4 | duktape.c |
| duk__cbor_decode_checkbreak | 4 | duktape.c |
| duk__cbor_decode_and_join_strbuf | 4 | duktape.c |
| duk__cbor_encode | 4 | duktape.c |
| duk__cbor_decode | 4 | duktape.c |
| duk__push_this_get_timeval_tzoffset | 4 | duktape.c |
| duk__twodigit_year_fixup | 4 | duktape.c |
| duk__set_this_timeval_from_dparts | 4 | duktape.c |
| duk__parse_iso8601_seps | 4 | duktape.c |
| duk__parse_iso8601_control | 4 | duktape.c |
| duk__div_floor | 4 | duktape.c |
| duk__day_from_year | 4 | duktape.c |
| duk__date_get_indirect_magic | 4 | duktape.c |
| duk__encode_context | 4 | duktape.c |
| duk__utf8_decode_init | 4 | duktape.c |
| duk__error_getter_helper | 4 | duktape.c |
| duk__error_setter_helper | 4 | duktape.c |
| duk__json_dec_eat_white | 4 | duktape.c |
| duk__json_dec_string | 4 | duktape.c |
| duk__json_enc_objarr_entry | 4 | duktape.c |
| duk__json_enc_objarr_exit | 4 | duktape.c |
| duk__json_enc_array | 4 | duktape.c |
| duk__json_enc_double | 4 | duktape.c |
| duk__json_enc_fastint_tval | 4 | duktape.c |
| duk__json_enc_buffer_jx_jc | 4 | duktape.c |
| duk__json_enc_pointer | 4 | duktape.c |
| duk__json_enc_bufobj | 4 | duktape.c |
| duk__json_enc_buffer_data_hex | 4 | duktape.c |
| duk__json_stringify_fast_value | 4 | duktape.c |
| duk__one_arg_func | 4 | duktape.c |
| duk__ptr_union | 4 | duktape.c |
| duk_debug_peek_byte | 4 | duktape.c |
| duk__debug_read_level_get_activation | 4 | duktape.c |
| duk__check_resend_status | 4 | duktape.c |
| duk_rom_strings_lookup | 4 | duktape.c |
| duk_int_fast16_t | 4 | duk_config.h, duktape.c |
| duk_intptr_t | 4 | duk_config.h, duktape.c |
| duk_uintmax_t | 4 | duk_config.h, duktape.c |
| duk_intmax_t | 4 | duk_config.h, duktape.c |
| duk_int_fast_t | 4 | duk_config.h, duktape.c |
| duk__mark_temproots_by_heap_scan | 4 | duktape.c |
| duk__sweep_heap | 4 | duktape.c |
| duk__assert_walk_strtable | 4 | duktape.c |
| duk__assert_validity_cb1 | 4 | duktape.c |
| duk__clear_assert_refcounts_cb1 | 4 | duktape.c |
| duk__strtable_assert_checks | 4 | duktape.c |
| duk__sort_enum_keys_es6 | 4 | duktape.c |
| duk__check_arguments_map_for_get | 4 | duktape.c |
| duk__handle_put_array_length_smaller | 4 | duktape.c |
| duk__count_used_e_keys | 4 | duktape.c |
| duk__compute_a_stats | 4 | duktape.c |
| duk__lookup_arguments_map | 4 | duktape.c |
| duk__to_new_array_length_checked | 4 | duktape.c |
| duk__hstring_get_charlen_slowpath | 4 | duktape.c |
| duk__push_stridx_or_string | 4 | duktape.c |
| duk__handle_safe_call_error | 4 | duktape.c |
| duk__alloctemps | 4 | duktape.c |
| duk__compiler_stkstate | 4 | duktape.c |
| duk__advance_helper | 4 | duktape.c |
| duk__init_func_valstack_slots | 4 | duktape.c |
| duk__convert_to_func_template | 4 | duktape.c |
| duk__emit_a_b | 4 | duktape.c |
| duk__emit_b_c | 4 | duktape.c |
| duk__patch_trycatch | 4 | duktape.c |
| duk__emit_if_false_skip | 4 | duktape.c |
| duk__ivalue_var_hstring | 4 | duktape.c |
| duk__ivalue_totemp | 4 | duktape.c |
| duk__add_label | 4 | duktape.c |
| duk__lookup_active_label | 4 | duktape.c |
| duk__exprtop_toreg | 4 | duktape.c |
| duk__parse_arguments | 4 | duktape.c |
| duk__parse_var_stmt | 4 | duktape.c |
| duk__stmt_label_site | 4 | duktape.c |
| duk__parse_func_like_raw | 4 | duktape.c |
| duk__update_lineinfo_currtoken | 4 | duktape.c |
| duk__objlit_flush_keys | 4 | duktape.c |
| duk__vm_arith_unary_op | 4 | duktape.c |
| duk__reconfig_valstack_ecma_return | 4 | duktape.c |
| duk__handle_finally | 4 | duktape.c |
| duk__handle_break_or_continue | 4 | duktape.c |
| duk__handle_return | 4 | duktape.c |
| duk__handle_executor_error | 4 | duktape.c |
| duk__js_equals_number | 4 | duktape.c |
| duk__compare_number | 4 | duktape.c |
| duk__bi_sub | 4 | duktape.c |
| duk__re_disjunction_info | 4 | duktape.c |
| duk__regexp_emit_range | 4 | duktape.c |
| duk_destroy_heap | 3 | dukky.c, duktape.c, duktape.h |
| duk_push_thread | 3 | dukky.c, duktape.c, duktape.h |
| duk_set_global_object | 3 | dukky.c, duktape.c, duktape.h |
| duk_safe_to_stacktrace | 3 | dukky.c, duktape.c, duktape.h |
| duk_get_memory_functions | 3 | dukky.c, duktape.c, duktape.h |
| duk_decode_char_function | 3 | duktape.c, duktape.h |
| duk_map_char_function | 3 | duktape.c, duktape.h |
| duk_compile | 3 | duktape.c, duktape.h |
| duk_resume | 3 | duktape.c, duktape.h |
| duk_throw_raw | 3 | duktape.c, duktape.h |
| duk_fatal_raw | 3 | duktape.c, duktape.h |
| duk_error_stash | 3 | duktape.c, duktape.h |
| duk_generic_error_stash | 3 | duktape.c, duktape.h |
| duk_eval_error_stash | 3 | duktape.c, duktape.h |
| duk_range_error_stash | 3 | duktape.c, duktape.h |
| duk_reference_error_stash | 3 | duktape.c, duktape.h |
| duk_syntax_error_stash | 3 | duktape.c, duktape.h |
| duk_type_error_stash | 3 | duktape.c, duktape.h |
| duk_uri_error_stash | 3 | duktape.c, duktape.h |
| duk_require_top_index | 3 | duktape.c, duktape.h |
| duk_swap_top | 3 | duktape.c, duktape.h |
| duk_pull | 3 | duktape.c, duktape.h |
| duk_push_new_target | 3 | duktape.c, duktape.h |
| duk_push_current_thread | 3 | duktape.c, duktape.h |
| duk_push_error_object_stash | 3 | duktape.c, duktape.h |
| duk_push_buffer | 3 | ImageData.bnd, duktape.c, duktape.h |
| duk_check_type | 3 | duktape.c, duktape.h |
| duk_is_pointer | 3 | duktape.c, duktape.h |
| duk_is_lightfunc | 3 | duktape.c, duktape.h |
| duk_get_buffer | 3 | duktape.c, duktape.h |
| duk_get_context | 3 | duktape.c, duktape.h |
| duk_get_heapptr | 3 | duktape.c, duktape.h |
| duk_opt_boolean | 3 | DOMTokenList.bnd, duktape.c, duktape.h |
| duk_require_boolean | 3 | duktape.c, duktape.h |
| duk_require_object | 3 | duktape.c, duktape.h |
| duk_require_pointer | 3 | duktape.c, duktape.h |
| duk_require_c_function | 3 | duktape.c, duktape.h |
| duk_require_function | 3 | duktape.c, duktape.h |
| duk_require_heapptr | 3 | duktape.c, duktape.h |
| duk_to_null | 3 | duktape.c, duktape.h |
| duk_to_uint16 | 3 | duktape.c, duktape.h |
| duk_to_pointer | 3 | duktape.c, duktape.h |
| duk_to_stacktrace | 3 | duktape.c, duktape.h |
| duk_json_encode | 3 | duktape.c, duktape.h |
| duk_json_decode | 3 | duktape.c, duktape.h |
| duk_get_prop_lstring | 3 | duktape.c, duktape.h |
| duk_get_prop_heapptr | 3 | duktape.c, duktape.h |
| duk_put_prop_lstring | 3 | duktape.c, duktape.h |
| duk_put_prop_heapptr | 3 | duktape.c, duktape.h |
| duk_del_prop_literal_raw | 3 | duktape.c, duktape.h |
| duk_has_prop_string | 3 | duktape.c, duktape.h |
| duk_has_prop_literal_raw | 3 | duktape.c, duktape.h |
| duk_get_global_literal_raw | 3 | duktape.c, duktape.h |
| duk_put_global_literal_raw | 3 | duktape.c, duktape.h |
| duk_inspect_value | 3 | duktape.c, duktape.h |
| duk_inspect_callstack_entry | 3 | duktape.c, duktape.h |
| duk_get_prototype | 3 | duktape.c, duktape.h |
| duk_equals | 3 | duktape.c, duktape.h |
| duk_samevalue | 3 | duktape.c, duktape.h |
| duk_dump_function | 3 | duktape.c, duktape.h |
| duk_load_function | 3 | duktape.c, duktape.h |
| duk_get_now | 3 | duktape.c, duktape.h |
| duk_time_to_components | 3 | duktape.c, duktape.h |
| duk_bi_date | 3 | duktape.c, duktape.h |
| duk__get_part_helper | 3 | duktape.c, duktape.h |
| duk_features | 3 | duktape.c |
| duk_repl_fpclassify | 3 | duk_config.h, duktape.c |
| duk_repl_signbit | 3 | duk_config.h, duktape.c |
| duk_repl_isfinite | 3 | duk_config.h, duktape.c |
| duk_repl_isnan | 3 | duk_config.h, duktape.c |
| duk_repl_isinf | 3 | duk_config.h, duktape.c |
| duk_strtab_entry | 3 | duktape.c |
| duk_tval_get_number_packed | 3 | duktape.c |
| duk_tval_get_number_unpacked | 3 | duktape.c |
| duk_tval_get_number_unpacked_fastint | 3 | duktape.c |
| duk_tval_assert_valid | 3 | duktape.c |
| duk_strings_data | 3 | duktape.c |
| duk_bi_object_constructor | 3 | duktape.c |
| duk_bi_function_constructor | 3 | duktape.c |
| duk_bi_array_constructor | 3 | duktape.c |
| duk_bi_string_constructor | 3 | duktape.c |
| duk_bi_boolean_constructor | 3 | duktape.c |
| duk_bi_number_constructor | 3 | duktape.c |
| duk_bi_date_constructor | 3 | duktape.c |
| duk_bi_regexp_constructor | 3 | duktape.c |
| duk_bi_error_constructor_shared | 3 | duktape.c |
| duk_bi_type_error_thrower | 3 | duktape.c |
| duk_bi_global_object_parse_int | 3 | duktape.c |
| duk_bi_global_object_parse_float | 3 | duktape.c |
| duk_bi_thread_constructor | 3 | duktape.c |
| duk_bi_pointer_constructor | 3 | duktape.c |
| duk_bi_proxy_constructor | 3 | duktape.c |
| duk_bi_symbol_constructor_shared | 3 | duktape.c |
| duk_bi_arraybuffer_constructor | 3 | duktape.c |
| duk_bi_dataview_constructor | 3 | duktape.c |
| duk_bi_nodejs_buffer_constructor | 3 | duktape.c |
| duk_bi_textencoder_constructor | 3 | duktape.c |
| duk_bi_textdecoder_constructor | 3 | duktape.c |
| duk_bi_global_object_is_nan | 3 | duktape.c |
| duk_bi_global_object_is_finite | 3 | duktape.c |
| duk_bi_global_object_decode_uri | 3 | duktape.c |
| duk_bi_global_object_decode_uri_component | 3 | duktape.c |
| duk_bi_global_object_encode_uri | 3 | duktape.c |
| duk_bi_global_object_encode_uri_component | 3 | duktape.c |
| duk_bi_global_object_escape | 3 | duktape.c |
| duk_bi_global_object_unescape | 3 | duktape.c |
| duk_bi_object_getprototype_shared | 3 | duktape.c |
| duk_bi_object_setprototype_shared | 3 | duktape.c |
| duk_bi_object_constructor_get_own_property_descriptor | 3 | duktape.c |
| duk_bi_object_constructor_keys_shared | 3 | duktape.c |
| duk_bi_object_constructor_assign | 3 | duktape.c |
| duk_bi_object_constructor_create | 3 | duktape.c |
| duk_bi_object_constructor_define_property | 3 | duktape.c |
| duk_bi_object_constructor_seal_freeze_shared | 3 | duktape.c |
| duk_bi_object_constructor_prevent_extensions | 3 | duktape.c |
| duk_bi_object_constructor_is_sealed_frozen_shared | 3 | duktape.c |
| duk_bi_object_constructor_is_extensible | 3 | duktape.c |
| duk_bi_object_constructor_is | 3 | duktape.c |
| duk_bi_object_prototype_to_locale_string | 3 | duktape.c |
| duk_bi_object_prototype_value_of | 3 | duktape.c |
| duk_bi_object_prototype_has_own_property | 3 | duktape.c |
| duk_bi_object_prototype_is_prototype_of | 3 | duktape.c |
| duk_bi_object_prototype_property_is_enumerable | 3 | duktape.c |
| duk_bi_object_prototype_defineaccessor | 3 | duktape.c |
| duk_bi_object_prototype_lookupaccessor | 3 | duktape.c |
| duk_bi_function_prototype_to_string | 3 | duktape.c |
| duk_bi_function_prototype_bind | 3 | duktape.c |
| duk_bi_function_prototype_hasinstance | 3 | duktape.c |
| duk_bi_native_function_length | 3 | duktape.c |
| duk_bi_native_function_name | 3 | duktape.c |
| duk_bi_array_constructor_is_array | 3 | duktape.c |
| duk_bi_array_prototype_to_string | 3 | duktape.c |
| duk_bi_array_prototype_join_shared | 3 | duktape.c |
| duk_bi_array_prototype_concat | 3 | duktape.c |
| duk_bi_array_prototype_pop | 3 | duktape.c |
| duk_bi_array_prototype_push | 3 | duktape.c |
| duk_bi_array_prototype_reverse | 3 | duktape.c |
| duk_bi_array_prototype_shift | 3 | duktape.c |
| duk_bi_array_prototype_slice | 3 | duktape.c |
| duk_bi_array_prototype_sort | 3 | duktape.c |
| duk_bi_array_prototype_splice | 3 | duktape.c |
| duk_bi_array_prototype_unshift | 3 | duktape.c |
| duk_bi_array_prototype_indexof_shared | 3 | duktape.c |
| duk_bi_array_prototype_iter_shared | 3 | duktape.c |
| duk_bi_array_prototype_reduce_shared | 3 | duktape.c |
| duk_bi_string_constructor_from_char_code | 3 | duktape.c |
| duk_bi_string_constructor_from_code_point | 3 | duktape.c |
| duk_bi_string_prototype_to_string | 3 | duktape.c |
| duk_bi_string_prototype_char_at | 3 | duktape.c |
| duk_bi_string_prototype_char_code_at | 3 | duktape.c |
| duk_bi_string_prototype_concat | 3 | duktape.c |
| duk_bi_string_prototype_indexof_shared | 3 | duktape.c |
| duk_bi_string_prototype_locale_compare | 3 | duktape.c |
| duk_bi_string_prototype_match | 3 | duktape.c |
| duk_bi_string_prototype_replace | 3 | duktape.c |
| duk_bi_string_prototype_search | 3 | duktape.c |
| duk_bi_string_prototype_slice | 3 | duktape.c |
| duk_bi_string_prototype_split | 3 | duktape.c |
| duk_bi_string_prototype_substring | 3 | duktape.c |
| duk_bi_string_prototype_caseconv_shared | 3 | duktape.c |
| duk_bi_string_prototype_trim | 3 | duktape.c |
| duk_bi_string_prototype_repeat | 3 | duktape.c |
| duk_bi_string_prototype_startswith_endswith | 3 | duktape.c |
| duk_bi_string_prototype_includes | 3 | duktape.c |
| duk_bi_string_prototype_substr | 3 | duktape.c |
| duk_bi_boolean_prototype_tostring_shared | 3 | duktape.c |
| duk_bi_number_check_shared | 3 | duktape.c |
| duk_bi_number_prototype_to_locale_string | 3 | duktape.c |
| duk_bi_number_prototype_value_of | 3 | duktape.c |
| duk_bi_number_prototype_to_fixed | 3 | duktape.c |
| duk_bi_number_prototype_to_exponential | 3 | duktape.c |
| duk_bi_number_prototype_to_precision | 3 | duktape.c |
| duk_bi_date_constructor_parse | 3 | duktape.c |
| duk_bi_date_constructor_utc | 3 | duktape.c |
| duk_bi_date_constructor_now | 3 | duktape.c |
| duk_bi_date_prototype_tostring_shared | 3 | duktape.c |
| duk_bi_date_prototype_to_json | 3 | duktape.c |
| duk_bi_date_prototype_get_shared | 3 | duktape.c |
| duk_bi_date_prototype_get_timezone_offset | 3 | duktape.c |
| duk_bi_date_prototype_set_time | 3 | duktape.c |
| duk_bi_date_prototype_set_shared | 3 | duktape.c |
| duk_bi_date_prototype_toprimitive | 3 | duktape.c |
| duk_bi_regexp_prototype_exec | 3 | duktape.c |
| duk_bi_regexp_prototype_test | 3 | duktape.c |
| duk_bi_regexp_prototype_tostring | 3 | duktape.c |
| duk_bi_regexp_prototype_flags | 3 | duktape.c |
| duk_bi_regexp_prototype_shared_getter | 3 | duktape.c |
| duk_bi_error_prototype_stack_setter | 3 | duktape.c |
| duk_bi_error_prototype_filename_setter | 3 | duktape.c |
| duk_bi_error_prototype_linenumber_setter | 3 | duktape.c |
| duk_bi_math_object_onearg_shared | 3 | duktape.c |
| duk_bi_math_object_twoarg_shared | 3 | duktape.c |
| duk_bi_math_object_clz32 | 3 | duktape.c |
| duk_bi_math_object_hypot | 3 | duktape.c |
| duk_bi_math_object_imul | 3 | duktape.c |
| duk_bi_math_object_max | 3 | duktape.c |
| duk_bi_math_object_min | 3 | duktape.c |
| duk_bi_math_object_random | 3 | duktape.c |
| duk_bi_math_object_sign | 3 | duktape.c |
| duk_bi_json_object_parse | 3 | duktape.c |
| duk_bi_json_object_stringify | 3 | duktape.c |
| duk_bi_duktape_object_info | 3 | duktape.c |
| duk_bi_duktape_object_act | 3 | duktape.c |
| duk_bi_duktape_object_gc | 3 | duktape.c |
| duk_bi_duktape_object_fin | 3 | duktape.c |
| duk_bi_duktape_object_enc | 3 | duktape.c |
| duk_bi_duktape_object_dec | 3 | duktape.c |
| duk_bi_duktape_object_compact | 3 | duktape.c |
| duk_bi_thread_current | 3 | duktape.c |
| duk_bi_pointer_prototype_tostring_shared | 3 | duktape.c |
| duk_bi_reflect_object_delete_property | 3 | duktape.c |
| duk_bi_reflect_object_get | 3 | duktape.c |
| duk_bi_reflect_object_has | 3 | duktape.c |
| duk_bi_reflect_object_set | 3 | duktape.c |
| duk_bi_symbol_key_for | 3 | duktape.c |
| duk_bi_symbol_tostring_shared | 3 | duktape.c |
| duk_bi_symbol_toprimitive | 3 | duktape.c |
| duk_bi_arraybuffer_isview | 3 | duktape.c |
| duk_bi_buffer_slice_shared | 3 | duktape.c |
| duk_bi_buffer_readfield | 3 | duktape.c |
| duk_bi_buffer_writefield | 3 | duktape.c |
| duk_bi_typedarray_set | 3 | duktape.c |
| duk_bi_uint8array_allocplain | 3 | duktape.c |
| duk_bi_uint8array_plainof | 3 | duktape.c |
| duk_bi_nodejs_buffer_concat | 3 | duktape.c |
| duk_bi_nodejs_buffer_is_encoding | 3 | duktape.c |
| duk_bi_nodejs_buffer_is_buffer | 3 | duktape.c |
| duk_bi_nodejs_buffer_byte_length | 3 | duktape.c |
| duk_bi_buffer_compare_shared | 3 | duktape.c |
| duk_bi_nodejs_buffer_tostring | 3 | duktape.c |
| duk_bi_nodejs_buffer_tojson | 3 | duktape.c |
| duk_bi_nodejs_buffer_fill | 3 | duktape.c |
| duk_bi_nodejs_buffer_copy | 3 | duktape.c |
| duk_bi_nodejs_buffer_write | 3 | duktape.c |
| duk_bi_textencoder_prototype_encoding_getter | 3 | duktape.c |
| duk_bi_textencoder_prototype_encode | 3 | duktape.c |
| duk_bi_textdecoder_prototype_shared_getter | 3 | duktape.c |
| duk_bi_textdecoder_prototype_decode | 3 | duktape.c |
| duk_bi_performance_now | 3 | duktape.c |
| duk_raw_write_xutf8 | 3 | duktape.c |
| duk_raw_writeinc_u16_be | 3 | duktape.c |
| duk_raw_writeinc_u32_be | 3 | duktape.c |
| duk_raw_writeinc_float_be | 3 | duktape.c |
| duk_raw_writeinc_double_be | 3 | duktape.c |
| duk_raw_writeinc_xutf8 | 3 | duktape.c |
| duk_raw_writeinc_cesu8 | 3 | duktape.c |
| duk_raw_readinc_u16_be | 3 | duktape.c |
| duk_raw_readinc_u32_be | 3 | duktape.c |
| duk_raw_readinc_double_be | 3 | duktape.c |
| duk_bw_init_pushbuf | 3 | duktape.c |
| duk_bw_init | 3 | duktape.c |
| duk__space | 3 | duktape.c |
| duk_unicode_get_cesu8_length | 3 | duktape.c |
| duk_bw_remove_raw_slice | 3 | duktape.c |
| duk_bw_write_ensure_slice | 3 | duktape.c |
| duk_bw_insert_ensure_bytes | 3 | duktape.c |
| duk_bw_insert_ensure_slice | 3 | duktape.c |
| duk_bw_insert_ensure_area | 3 | duktape.c |
| duk_be_finish | 3 | duktape.c |
| duk_raw_read_float_be | 3 | duktape.c |
| duk_is_whole_get_int32_nonegzero | 3 | duktape.c |
| duk_is_whole_get_int32 | 3 | duktape.c |
| duk_double_is_posinf | 3 | duktape.c |
| duk_double_is_nan_zero_inf | 3 | duktape.c |
| duk_double_signbit | 3 | duktape.c |
| duk_double_same_sign | 3 | duktape.c |
| duk_double_is_finite | 3 | duktape.c |
| duk_double_is_safe_integer | 3 | duktape.c |
| duk_double_to_int_t | 3 | duktape.c |
| duk_double_to_uint_t | 3 | duktape.c |
| duk_double_to_float_t | 3 | duktape.c |
| duk_lexer_initctx | 3 | duktape.c |
| duk_lexer_setpoint | 3 | duktape.c |
| duk_lexer_getpoint | 3 | duktape.c |
| duk_lexer_parse_js_input_element | 3 | duktape.c |
| duk_lexer_parse_re_token | 3 | duktape.c |
| duk_lexer_parse_re_ranges | 3 | duktape.c |
| duk_regexp_match_force_global | 3 | duktape.c |
| duk_heaphdr_assert_valid | 3 | duktape.c |
| duk_tval_incref | 3 | duktape.c |
| duk_tval_decref | 3 | duktape.c |
| duk_tval_decref_norz | 3 | duktape.c |
| duk_refzero_check_fast | 3 | duktape.c |
| duk_refzero_check_slow | 3 | duktape.c |
| duk_heaphdr_refcount_finalize_norz | 3 | duktape.c |
| duk_valstack_shrink_check_nothrow | 3 | duktape.c |
| duk_push_this_check_object_coercible | 3 | duktape.c |
| duk_push_i32 | 3 | duktape.c |
| duk_push_size_t | 3 | duktape.c |
| duk_is_callable_tval | 3 | duktape.c |
| duk_get_hthread | 3 | duktape.c |
| duk_get_hcompfunc | 3 | duktape.c |
| duk_get_hnatfunc | 3 | duktape.c |
| duk_safe_to_hstring | 3 | duktape.c |
| duk_to_uint8clamped | 3 | duktape.c |
| duk_require_lstring_notsymbol | 3 | duktape.c |
| duk_require_hcompfunc | 3 | duktape.c |
| duk_require_hnatfunc | 3 | duktape.c |
| duk_push_hboundfunc | 3 | duktape.c |
| duk_push_c_function_builtin | 3 | duktape.c |
| duk_push_lightfunc_name | 3 | duktape.c |
| duk_push_hnatfunc_name | 3 | duktape.c |
| duk_push_string_tval_readable_error | 3 | duktape.c |
| duk_get_prop_stridx_short_raw | 3 | duktape.c |
| duk_xget_owndataprop | 3 | duktape.c |
| duk_xget_owndataprop_stridx | 3 | duktape.c |
| duk_xget_owndataprop_stridx_short_raw | 3 | duktape.c |
| duk_put_prop_stridx_short_raw | 3 | duktape.c |
| duk_del_prop_stridx_short_raw | 3 | duktape.c |
| duk_has_prop_stridx_short_raw | 3 | duktape.c |
| duk_xdef_prop_stridx_short_raw | 3 | duktape.c |
| duk_xdef_prop_wec | 3 | duktape.c |
| duk_resolve_nonbound_function | 3 | duktape.c |
| duk_insert_undefined_n | 3 | duktape.c |
| duk_to_primitive_ordinary | 3 | duktape.c |
| duk_clear_prototype | 3 | duktape.c |
| duk_js_to_arrayindex_hstring_fast | 3 | duktape.c |
| duk_hstring_assert_valid | 3 | duktape.c |
| duk_hstring_equals_ascii_cstring | 3 | duktape.c |
| duk_hstring_init_charlen | 3 | duktape.c |
| duk_hobject_assert_valid | 3 | duktape.c |
| duk_class_number_to_stridx | 3 | duktape.c |
| duk_hobject_set_prototype_updref | 3 | duktape.c |
| duk__obj | 3 | duktape.c |
| duk__proto | 3 | duktape.c |
| duk_hobject_alloc_unchecked | 3 | duktape.c |
| duk_hobject_alloc | 3 | duktape.c |
| duk_hcompfunc_alloc | 3 | duktape.c |
| duk_hnatfunc_alloc | 3 | duktape.c |
| duk_hboundfunc_alloc | 3 | duktape.c |
| duk_hbufobj_alloc | 3 | duktape.c |
| duk_hproxy_alloc | 3 | duktape.c |
| duk_hobject_find_entry_tval_ptr_and_attrs | 3 | duktape.c |
| duk_hobject_define_property_internal_arridx | 3 | duktape.c |
| duk_hobject_object_seal_freeze_helper | 3 | duktape.c |
| duk_hobject_object_is_sealed_frozen_helper | 3 | duktape.c |
| duk_hobject_pc2line_pack | 3 | duktape.c |
| duk_hcompfunc_assert_valid | 3 | duktape.c |
| duk_hnatfunc_assert_valid | 3 | duktape.c |
| duk_hboundfunc_assert_valid | 3 | duktape.c |
| duk_hbufobj_assert_valid | 3 | duktape.c |
| duk_hbufobj_clamp_bytelength | 3 | duktape.c |
| duk_hbufobj_push_uint8array_from_plain | 3 | duktape.c |
| duk_hthread_assert_valid | 3 | duktape.c |
| duk_ctx_assert_valid | 3 | duktape.c |
| duk_hthread_copy_builtin_objects | 3 | duktape.c |
| duk_hthread_activation_free | 3 | duktape.c |
| duk_hthread_activation_unwind_reuse_norz | 3 | duktape.c |
| duk_hthread_catcher_unwind_nolexenv_norz | 3 | duktape.c |
| duk_hthread_get_valstack_ptr | 3 | duktape.c |
| duk_hthread_sync_currpc | 3 | duktape.c |
| duk_harrays | 3 | duktape.c |
| duk_harray_assert_valid | 3 | duktape.c |
| duk_hdecenv_assert_valid | 3 | duktape.c |
| duk_hobjenv_assert_valid | 3 | duktape.c |
| duk_hbuffer_assert_valid | 3 | duktape.c |
| duk_hbuffer_alloc | 3 | duktape.c |
| duk_hbuffer_get_dynalloc_ptr | 3 | duktape.c |
| duk_hbuffer_reset | 3 | duktape.c |
| duk_hproxy_assert_valid | 3 | duktape.c |
| duk_heap_switch_thread | 3 | duktape.c |
| duk_heap_insert_into_heap_allocated | 3 | duktape.c |
| duk_heap_remove_from_heap_allocated | 3 | duktape.c |
| duk_heap_insert_into_finalize_list | 3 | duktape.c |
| duk_heap_remove_from_finalize_list | 3 | duktape.c |
| duk_heap_mem_alloc_zeroed | 3 | duktape.c |
| duk_heap_mem_alloc_checked | 3 | duktape.c |
| duk_heap_mem_alloc_checked_zeroed | 3 | duktape.c |
| duk_heap_assert_valid | 3 | duktape.c |
| duk_error_throw | 3 | duktape.c |
| duk_heap_alloc | 3 | duktape.c |
| duk_heap_in_heap_allocated | 3 | duktape.c |
| duk_heap_strtable_intern_literal_checked | 3 | duktape.c |
| duk_heap_strtable_unlink | 3 | duktape.c |
| duk_heap_strtable_unlink_prev | 3 | duktape.c |
| duk_heap_strtable_force_resize | 3 | duktape.c |
| duk_heap_strtable_free | 3 | duktape.c |
| duk_default_alloc_function | 3 | duktape.c |
| duk_default_realloc_function | 3 | duktape.c |
| duk_default_free_function | 3 | duktape.c |
| duk_debug_read_peek | 3 | duktape.c |
| duk_debug_send_status | 3 | duktape.c |
| duk_debug_send_throw | 3 | duktape.c |
| duk_debug_add_breakpoint | 3 | duktape.c |
| duk_debug_remove_breakpoint | 3 | duktape.c |
| duk_debug_format_funcptr | 3 | duktape.c |
| duk_err_error_internal | 3 | duktape.c |
| duk_err_error_alloc_failed | 3 | duktape.c |
| duk_err_range_index | 3 | duktape.c |
| duk_err_range_push_beyond | 3 | duktape.c |
| duk_err_type_invalid_args | 3 | duktape.c |
| duk_err_type_invalid_state | 3 | duktape.c |
| duk_err_type_invalid_trap_result | 3 | duktape.c |
| duk_err_eval | 3 | duktape.c |
| duk_err_reference | 3 | duktape.c |
| duk_err_syntax | 3 | duktape.c |
| duk_err_uri | 3 | duktape.c |
| duk__assert_tmp_du | 3 | duktape.c |
| duk_error_prototype_from_code | 3 | duktape.c |
| duk_unicode_re_canon_lookup | 3 | duktape.c |
| duk_unicode_xutf8_markers | 3 | duktape.c |
| duk_unicode_is_utf8_compatible | 3 | duktape.c |
| duk_unicode_case_convert_string | 3 | duktape.c |
| duk_js_toint32 | 3 | duktape.c |
| duk_js_touint16 | 3 | duktape.c |
| duk_js_instanceof_ordinary | 3 | duktape.c |
| duk_js_in | 3 | duktape.c |
| duk_js_arith_mod | 3 | duktape.c |
| duk_js_getvar_envrec | 3 | duktape.c |
| duk_js_delvar_activation | 3 | duktape.c |
| duk_js_declvar_activation | 3 | duktape.c |
| duk_js_close_environment_record | 3 | duktape.c |
| duk_js_execute_bytecode | 3 | duktape.c |
| duk_bi_date_timeval_in_valid_range | 3 | duktape.c |
| duk_bi_date_year_in_valid_range | 3 | duktape.c |
| duk_bi_date_get_now_gettimeofday | 3 | duk_config.h, duktape.c |
| duk_bi_date_get_now_time | 3 | duk_config.h, duktape.c |
| duk_bi_date_get_now_windows | 3 | duk_config.h, duktape.c |
| duk_bi_date_get_now_windows_subms | 3 | duk_config.h, duktape.c |
| duk_bi_date_get_local_tzoffset_gmtime | 3 | duk_config.h, duktape.c |
| duk_bi_date_get_local_tzoffset_windows_no_dst | 3 | duk_config.h, duktape.c |
| duk_bi_date_parse_string_strptime | 3 | duk_config.h, duktape.c |
| duk_bi_date_parse_string_getdate | 3 | duk_config.h, duktape.c |
| duk_bi_date_format_parts_strftime | 3 | duk_config.h, duktape.c |
| duk_bi_date_get_monotonic_time_clock_gettime | 3 | duk_config.h, duktape.c |
| duk_bi_date_get_monotonic_time_windows_qpc | 3 | duk_config.h, duktape.c |
| duk_textdecoder_decode_utf8_nodejs | 3 | duktape.c |
| duk_selftest_run_tests | 3 | duktape.c |
| duk__uni_decode_value | 3 | duktape.c |
| duk__case_transform_helper | 3 | duktape.c |
| duk__u64_union | 3 | duktape.c |
| duk__dump_string_prop | 3 | duktape.c |
| duk__dump_func | 3 | duktape.c |
| duk__load_func | 3 | duktape.c |
| duk__call_get_idx_func_unvalidated | 3 | duktape.c |
| duk__base64_decode_nequal_step | 3 | duktape.c |
| duk__base64_encode_helper | 3 | duktape.c |
| duk__base64_decode_helper | 3 | duktape.c |
| duk__do_compile | 3 | duktape.c |
| duk__symbol_type_strings | 3 | duktape.c |
| duk__type_from_tag | 3 | duktape.c |
| duk__type_mask_from_tag | 3 | duktape.c |
| duk__valstack_grow | 3 | duktape.c |
| duk__get_boolean_raw | 3 | duktape.c |
| duk__get_number_raw | 3 | duktape.c |
| duk__get_pointer_raw | 3 | duktape.c |
| duk__defaultvalue_coerce_attempt | 3 | duktape.c |
| duk__toprim_hint_strings | 3 | duktape.c |
| duk__to_primitive_helper | 3 | duktape.c |
| duk__to_int_uint_helper | 3 | duktape.c |
| duk__safe_to_string_raw | 3 | duktape.c |
| duk__safe_to_stacktrace_raw | 3 | duktape.c |
| duk__type_names | 3 | duktape.c |
| duk__bufobj_flags_lookup | 3 | duktape.c |
| duk__pop_n_unsafe_raw | 3 | duktape.c |
| duk__pop_unsafe_raw | 3 | duktape.c |
| duk__pop_2_unsafe_raw | 3 | duktape.c |
| duk__throw_error_from_stash | 3 | duktape.c |
| duk__push_string_tval_readable | 3 | duktape.c |
| duk__concat_and_join_helper | 3 | duktape.c |
| duk__arraypart_fastpath_this | 3 | duktape.c |
| duk__buffer_class_from_elemtype | 3 | duktape.c |
| duk__buffer_proto_from_elemtype | 3 | duktape.c |
| duk__buffer_nbytes_from_fldtype | 3 | duktape.c |
| duk__resolve_offset_opt_length | 3 | duktape.c |
| duk__clamp_startend_nonegidx_noshift | 3 | duktape.c |
| duk__clamp_startend_negidx_shifted | 3 | duktape.c |
| duk__hbufobj_fixed_from_argvalue | 3 | duktape.c |
| duk__autospawn_arraybuffer | 3 | duktape.c |
| duk__cbor_double_to_uint32 | 3 | duktape.c |
| duk__cbor_encode_req_stack | 3 | duktape.c |
| duk__cbor_encode_double | 3 | duktape.c |
| duk__cbor_encode_string_top | 3 | duktape.c |
| duk__cbor_decode_req_stack | 3 | duktape.c |
| duk__cbor_decode_objarr_entry | 3 | duktape.c |
| duk__cbor_decode_objarr_exit | 3 | duktape.c |
| duk__cbor_decode_read_u16 | 3 | duktape.c |
| duk__cbor_decode_skip_aival_int | 3 | duktape.c |
| duk__cbor_decode_half_float | 3 | duktape.c |
| duk__cbor_decode_float | 3 | duktape.c |
| duk__cbor_decode_double | 3 | duktape.c |
| duk__date_equivyear | 3 | duktape.c |
| duk__parse_string | 3 | duktape.c |
| duk__days_in_month | 3 | duktape.c |
| duk__set_parts_from_args | 3 | duktape.c |
| duk__date_magics | 3 | duktape.c |
| duk__utf8_emit_repl | 3 | duktape.c |
| duk__decode_helper | 3 | duktape.c |
| duk__get_textdecoder_context | 3 | duktape.c |
| duk__transform_callback_encode_uri | 3 | duktape.c |
| duk__transform_callback_decode_uri | 3 | duktape.c |
| duk__json_dec_peek | 3 | duktape.c |
| duk__json_dec_plain_string | 3 | duktape.c |
| duk__json_dec_pointer | 3 | duktape.c |
| duk__json_dec_buffer | 3 | duktape.c |
| duk__json_dec_number | 3 | duktape.c |
| duk__json_dec_object | 3 | duktape.c |
| duk__json_dec_array | 3 | duktape.c |
| duk__emit_1 | 3 | duktape.c |
| duk__emit_2 | 3 | duktape.c |
| duk__unemit_1 | 3 | duktape.c |
| duk__emit_hstring | 3 | duktape.c |
| duk__emit_cstring | 3 | duktape.c |
| duk__emit_stridx | 3 | duktape.c |
| duk__json_enc_allow_into_proplist | 3 | duktape.c |
| duk__json_enc_buffer_json_fastpath | 3 | duktape.c |
| duk__json_quotestr_esc | 3 | duktape.c |
| duk__json_decstr_lookup | 3 | duktape.c |
| duk__json_eatwhite_lookup | 3 | duktape.c |
| duk__json_decnumber_lookup | 3 | duktape.c |
| duk__json_dec_string_escape | 3 | duktape.c |
| duk__json_enc_buffer_data | 3 | duktape.c |
| duk__math_minmax | 3 | duktape.c |
| duk__cbrt | 3 | duktape.c |
| duk__log2 | 3 | duktape.c |
| duk__log10 | 3 | duktape.c |
| duk__trunc | 3 | duktape.c |
| duk__round_fixed | 3 | duktape.c |
| duk__atan2_fixed | 3 | duktape.c |
| duk__one_arg_funcs | 3 | duktape.c |
| duk__two_arg_funcs | 3 | duktape.c |
| duk__get_this_regexp | 3 | duktape.c |
| duk__str_tostring_notregexp | 3 | duktape.c |
| duk__str_search_shared | 3 | duktape.c |
| duk__construct_from_codepoints | 3 | duktape.c |
| duk__to_regexp_helper | 3 | duktape.c |
| duk__auto_unbox_symbol | 3 | duktape.c |
| duk__bc_optab | 3 | duktape.c |
| duk__print_instr | 3 | duktape.c |
| duk__print_heaphdr | 3 | duktape.c |
| duk__print_shared_heaphdr | 3 | duktape.c |
| duk__print_shared_heaphdr_string | 3 | duktape.c |
| duk__debug_do_detach1 | 3 | duktape.c |
| duk__debug_do_detach2 | 3 | duktape.c |
| duk__debug_null_most_callbacks | 3 | duktape.c |
| duk__debug_set_pause_state | 3 | duktape.c |
| duk__debug_read_hbuffer_raw | 3 | duktape.c |
| duk_debug_write_strbuf | 3 | duktape.c |
| duk__debug_write_pointer_raw | 3 | duktape.c |
| duk__debug_write_tval_heapptr | 3 | duktape.c |
| duk__debug_dump_heaphdr | 3 | duktape.c |
| duk__debug_getinfo_hstring_keys | 3 | duktape.c |
| duk__debug_getinfo_hstring_masks | 3 | duktape.c |
| duk__debug_getinfo_hobject_keys | 3 | duktape.c |
| duk__debug_getinfo_hobject_masks | 3 | duktape.c |
| duk__debug_getinfo_hbuffer_keys | 3 | duktape.c |
| duk__debug_getinfo_hbuffer_masks | 3 | duktape.c |
| duk__debug_getprop_index | 3 | duktape.c |
| duk__debug_process_message | 3 | duktape.c |
| duk__err_augment_user | 3 | duktape.c |
| duk__uncaught_minimal | 3 | duktape.c |
| duk__uncaught_error_aware | 3 | duktape.c |
| duk__init_heap_strings | 3 | duktape.c |
| duk_uint_least8_t | 3 | duk_config.h, duktape.c |
| duk_int_least8_t | 3 | duk_config.h, duktape.c |
| duk_uint_least16_t | 3 | duk_config.h, duktape.c |
| duk_int_least16_t | 3 | duk_config.h, duktape.c |
| duk_uint_least32_t | 3 | duk_config.h, duktape.c |
| duk_int_least32_t | 3 | duk_config.h, duktape.c |
| duk_uint_least64_t | 3 | duk_config.h, duktape.c |
| duk_int_least64_t | 3 | duk_config.h, duktape.c |
| duk_int_fast8_t | 3 | duk_config.h, duktape.c |
| duk_uint_fast64_t | 3 | duk_config.h, duktape.c |
| duk_int_fast64_t | 3 | duk_config.h, duktape.c |
| duk__mark_tvals | 3 | duktape.c |
| duk__mark_hstring | 3 | duktape.c |
| duk__mark_hobject | 3 | duktape.c |
| duk__mark_roots_heap | 3 | duktape.c |
| duk__mark_finalizable | 3 | duktape.c |
| duk__mark_finalize_list | 3 | duktape.c |
| duk__finalize_refcounts | 3 | duktape.c |
| duk__clear_finalize_list_flags | 3 | duktape.c |
| duk__sweep_stringtable | 3 | duktape.c |
| duk__compact_objects | 3 | duktape.c |
| duk__assert_heaphdr_flags | 3 | duktape.c |
| duk__assert_validity | 3 | duktape.c |
| duk__assert_valid_refcounts | 3 | duktape.c |
| duk__check_refcount_heaphdr | 3 | duktape.c |
| duk__check_assert_refcounts_cb1 | 3 | duktape.c |
| duk__hobject_refzero_helper | 3 | duktape.c |
| duk__heaphdr_refzero_helper | 3 | duktape.c |
| duk__scan_forwards | 3 | duktape.c |
| duk__scan_backwards | 3 | duktape.c |
| duk__strtable_grow_inplace | 3 | duktape.c |
| duk__strtable_shrink_inplace | 3 | duktape.c |
| duk__strtable_resize_check | 3 | duktape.c |
| duk_hobject_props | 3 | duktape.c |
| duk__hstring_sort_key | 3 | duktape.c |
| duk__add_enum_key_stridx | 3 | duktape.c |
| duk__check_arguments_map_for_put | 3 | duktape.c |
| duk__check_arguments_map_for_delete | 3 | duktape.c |
| duk__handle_put_array_length | 3 | duktape.c |
| duk__grow_props_for_array_item | 3 | duktape.c |
| duk__get_min_grow_e | 3 | duktape.c |
| duk__get_min_grow_a | 3 | duktape.c |
| duk__abandon_array_density_check | 3 | duktape.c |
| duk__hobject_get_entry_object_stridx | 3 | duktape.c |
| duk__push_string | 3 | duktape.c |
| duk__hthread_catcher_alloc_slow | 3 | duktape.c |
| duk__hthread_activation_alloc_slow | 3 | duktape.c |
| duk__activation_unwind_nofree_norz | 3 | duktape.c |
| duk__call_c_recursion_limit_check | 3 | duktape.c |
| duk__interrupt_fixup | 3 | duktape.c |
| duk__update_default_instance_proto | 3 | duktape.c |
| duk__update_func_caller_prop | 3 | duktape.c |
| duk__coerce_nonstrict_this_binding | 3 | duktape.c |
| duk__safe_call_adjust_valstack | 3 | duktape.c |
| duk__call_thread_state_update | 3 | duktape.c |
| duk__handle_call_raw | 3 | duktape.c |
| duk__handle_safe_call_shared_unwind | 3 | duktape.c |
| duk__settemp_checkmax | 3 | duktape.c |
| duk__alloctemp | 3 | duktape.c |
| duk__reset_func_for_pass2 | 3 | duktape.c |
| duk__init_varmap_and_prologue_for_pass2 | 3 | duktape.c |
| duk__cleanup_varmap | 3 | duktape.c |
| duk__insert_jump_entry | 3 | duktape.c |
| duk__ivalue_var_fromstack | 3 | duktape.c |
| duk__ispec_toforcedreg | 3 | duktape.c |
| duk__lookup_active_register_binding | 3 | duktape.c |
| duk__reset_labels_to_length | 3 | duktape.c |
| duk__expr_toplain_ignore | 3 | duktape.c |
| duk__exprtop_toforcedreg | 3 | duktape.c |
| duk__nud_array_literal | 3 | duktape.c |
| duk__nud_object_literal | 3 | duktape.c |
| duk__parse_for_stmt | 3 | duktape.c |
| duk__parse_switch_stmt | 3 | duktape.c |
| duk__parse_if_stmt | 3 | duktape.c |
| duk__parse_do_stmt | 3 | duktape.c |
| duk__parse_while_stmt | 3 | duktape.c |
| duk__parse_break_or_continue_stmt | 3 | duktape.c |
| duk__parse_return_stmt | 3 | duktape.c |
| duk__parse_throw_stmt | 3 | duktape.c |
| duk__parse_try_stmt | 3 | duktape.c |
| duk__parse_with_stmt | 3 | duktape.c |
| duk__parse_func_formals | 3 | duktape.c |
| duk__hstring_is_eval_or_arguments | 3 | duktape.c |
| duk__emit_load_int32_raw | 3 | duktape.c |
| duk__objlit_state | 3 | duktape.c |
| duk__js_execute_bytecode_inner | 3 | duktape.c |
| duk__tvtmp | 3 | duktape.c |
| duk__reconfig_valstack_ecma_catcher | 3 | duktape.c |
| duk__set_catcher_regs_norz | 3 | duktape.c |
| duk__handle_yield | 3 | duktape.c |
| duk__executor_handle_call | 3 | duktape.c |
| duk__toint32_touint32_helper | 3 | duktape.c |
| duk__js_instanceof_helper | 3 | duktape.c |
| duk__closure_copy_proplist | 3 | duktape.c |
| duk__getvar_helper | 3 | duktape.c |
| duk__putvar_helper | 3 | duktape.c |
| duk__delvar_helper | 3 | duktape.c |
| duk__fill_lexer_buffer | 3 | duktape.c |
| duk__init_lexer_window | 3 | duktape.c |
| duk__hexval_validate | 3 | duktape.c |
| duk__hexval | 3 | duktape.c |
| duk__lexer_parse_legacy_octal | 3 | duktape.c |
| duk__str2num_exp_limits | 3 | duktape.c |
| duk__exp_limits | 3 | duktape.c |
| duk__bi_mul_copy | 3 | duktape.c |
| duk__dragon4_format_uint32 | 3 | duktape.c |
| duk__dragon4_prepare | 3 | duktape.c |
| duk__dragon4_scale | 3 | duktape.c |
| duk__dragon4_generate | 3 | duktape.c |
| duk__dragon4_fixed_format_round | 3 | duktape.c |
| duk__append_slice | 3 | duktape.c |
| duk__append_jump_offset | 3 | duktape.c |
| duk__re_canon_next_discontinuity | 3 | duktape.c |
| duk__regexp_generate_ranges | 3 | duktape.c |
| duk__inp_backtrack | 3 | duktape.c |
| duk__inp_get_prev_cp | 3 | duktape.c |
| duk__regexp_match_helper | 3 | duktape.c |
| duk__bitpacked_lookup | 3 | duktape.c |
| duk__rnd_rotl | 3 | duktape.c |
| duk_pcompile | 2 | dukky.c, duktape.h |
| duk_eval | 2 | duktape.h |
| duk_alloc_raw | 2 | duktape.c, duktape.h |
| duk_free_raw | 2 | duktape.c, duktape.h |
| duk_realloc_raw | 2 | duktape.c, duktape.h |
| duk_alloc | 2 | duktape.c, duktape.h |
| duk_free | 2 | duktape.c, duktape.h |
| duk_realloc | 2 | duktape.c, duktape.h |
| duk_eval_error | 2 | duktape.h |
| duk_range_error | 2 | duktape.h |
| duk_reference_error | 2 | duktape.h |
| duk_syntax_error | 2 | duktape.h |
| duk_uri_error | 2 | duktape.h |
| duk_check_stack_top | 2 | duktape.c, duktape.h |
| duk_require_stack_top | 2 | duktape.c, duktape.h |
| duk_push_current_function | 2 | duktape.c, duktape.h |
| duk_push_heap_stash | 2 | duktape.c, duktape.h |
| duk_push_global_stash | 2 | duktape.c, duktape.h |
| duk_push_thread_stash | 2 | duktape.c, duktape.h |
| duk_push_c_lightfunc | 2 | duktape.c, duktape.h |
| duk_is_c_function | 2 | duktape.c, duktape.h |
| duk_is_ecmascript_function | 2 | duktape.c, duktape.h |
| duk_is_bound_function | 2 | duktape.c, duktape.h |
| duk_is_thread | 2 | duktape.c, duktape.h |
| duk_is_dynamic_buffer | 2 | duktape.c, duktape.h |
| duk_is_fixed_buffer | 2 | duktape.c, duktape.h |
| duk_is_external_buffer | 2 | duktape.c, duktape.h |
| duk_is_primitive | 2 | duktape.c, duktape.h |
| duk_is_error | 2 | duktape.c, duktape.h |
| duk_get_boolean_default | 2 | duktape.c, duktape.h |
| duk_get_number_default | 2 | duktape.c, duktape.h |
| duk_get_int_default | 2 | duktape.c, duktape.h |
| duk_get_uint_default | 2 | duktape.c, duktape.h |
| duk_get_string_default | 2 | duktape.c, duktape.h |
| duk_get_lstring_default | 2 | duktape.c, duktape.h |
| duk_get_buffer_default | 2 | duktape.c, duktape.h |
| duk_get_buffer_data_default | 2 | duktape.c, duktape.h |
| duk_get_pointer_default | 2 | duktape.c, duktape.h |
| duk_get_c_function_default | 2 | duktape.c, duktape.h |
| duk_get_context_default | 2 | duktape.c, duktape.h |
| duk_get_heapptr_default | 2 | duktape.c, duktape.h |
| duk_opt_number | 2 | duktape.c, duktape.h |
| duk_opt_int | 2 | duktape.c, duktape.h |
| duk_opt_uint | 2 | duktape.c, duktape.h |
| duk_opt_string | 2 | duktape.c, duktape.h |
| duk_opt_lstring | 2 | duktape.c, duktape.h |
| duk_opt_buffer | 2 | duktape.c, duktape.h |
| duk_opt_buffer_data | 2 | duktape.c, duktape.h |
| duk_opt_pointer | 2 | duktape.c, duktape.h |
| duk_opt_c_function | 2 | duktape.c, duktape.h |
| duk_opt_context | 2 | duktape.c, duktape.h |
| duk_opt_heapptr | 2 | duktape.c, duktape.h |
| duk_require_undefined | 2 | duktape.c, duktape.h |
| duk_require_null | 2 | duktape.c, duktape.h |
| duk_require_constructable | 2 | duktape.c, duktape.h |
| duk_require_object_coercible | 2 | duktape.c, duktape.h |
| duk_to_buffer | 2 | duktape.c, duktape.h |
| duk_to_fixed_buffer | 2 | duktape.c, duktape.h |
| duk_to_dynamic_buffer | 2 | duktape.c, duktape.h |
| duk_cbor_encode | 2 | duktape.c, duktape.h |
| duk_cbor_decode | 2 | duktape.c, duktape.h |
| duk_steal_buffer | 2 | duktape.c, duktape.h |
| duk_config_buffer | 2 | duktape.c, duktape.h |
| duk_del_prop_lstring | 2 | duktape.c, duktape.h |
| duk_del_prop_literal | 2 | duktape.h |
| duk_del_prop_heapptr | 2 | duktape.c, duktape.h |
| duk_has_prop_lstring | 2 | duktape.c, duktape.h |
| duk_has_prop_literal | 2 | duktape.h |
| duk_has_prop_heapptr | 2 | duktape.c, duktape.h |
| duk_get_global_lstring | 2 | duktape.c, duktape.h |
| duk_get_global_literal | 2 | duktape.h |
| duk_get_global_heapptr | 2 | duktape.c, duktape.h |
| duk_put_global_lstring | 2 | duktape.c, duktape.h |
| duk_put_global_literal | 2 | duktape.h |
| duk_put_global_heapptr | 2 | duktape.c, duktape.h |
| duk_get_magic | 2 | duktape.c, duktape.h |
| duk_put_function_list | 2 | duktape.c, duktape.h |
| duk_put_number_list | 2 | duktape.c, duktape.h |
| duk_seal | 2 | duktape.c, duktape.h |
| duk_freeze | 2 | duktape.c, duktape.h |
| duk_map_string | 2 | duktape.c, duktape.h |
| duk_char_code_at | 2 | duktape.c, duktape.h |
| duk_instanceof | 2 | duktape.c, duktape.h |
| duk_random | 2 | duktape.c, duktape.h |
| duk_call_prop | 2 | duktape.c, duktape.h |
| duk_pcall_prop | 2 | duktape.c, duktape.h |
| duk_pnew | 2 | duktape.c, duktape.h |
| duk_eval_string_noresult | 2 | duktape.c, duktape.h |
| duk_components_to_time | 2 | duktape.c, duktape.h |
| duk_numconv | 2 | duktape.c |
| duk_forwdecl | 2 | duktape.c |
| duk__tmp | 2 | duktape.c |
| duk_tval_struct | 2 | duktape.c |
| duk_util_probe_steps | 2 | duktape.c |
| duk_raw_readinc_float_be | 2 | duktape.c |
| duk_raw_write_cesu8 | 2 | duktape.c |
| duk_memzero_unsafe | 2 | duktape.c |
| duk_double_is_anyinf | 2 | duktape.c |
| duk_double_is_neginf | 2 | duktape.c |
| duk_float_equals | 2 | duktape.c |
| duk_strings | 2 | duktape.c |
| duk_js_bytecode | 2 | duktape.c |
| duk_js_compiler | 2 | duktape.c |
| duk_regexp | 2 | duktape.c |
| duk_hstring_decref | 2 | duktape.c |
| duk_hstring_decref_norz | 2 | duktape.c |
| duk_hbuffer_decref | 2 | duktape.c |
| duk_hbuffer_decref_norz | 2 | duktape.c |
| duk_hobject_decref | 2 | duktape.c |
| duk_hobject_decref_norz | 2 | duktape.c |
| duk_api_internal | 2 | duktape.c |
| duk_push_i64 | 2 | duktape.c |
| duk_require_hobject_promote_lfunc | 2 | duktape.c |
| duk_get_voidptr | 2 | duktape.c |
| duk_known_hcompfunc | 2 | duktape.c |
| duk_require_string_notsymbol | 2 | duktape.c |
| duk_push_hthread | 2 | duktape.c |
| duk_del_prop_stridx_short | 2 | duktape.c |
| duk_has_prop_stridx_short | 2 | duktape.c |
| duk_xdef_prop_stridx_builtin | 2 | duktape.c |
| duk_pop_3_nodecref_unsafe | 2 | duktape.c |
| duk_hobject_resize_arraypart | 2 | duktape.c |
| duk_rom_strings_stridx | 2 | duktape.c |
| duk_debug_read_heapptr | 2 | duktape.c |
| duk_debug_write_request | 2 | duktape.c |
| duk_debug_snprintf | 2 | duktape.c |
| duk_unicode_support | 2 | duktape.c |
| duk_js_buffer_compare | 2 | duktape.c |
| duk_js_hasvar_envrec | 2 | duktape.c |
| duk_js_delvar_envrec | 2 | duktape.c |
| duk__slow_case_conversion | 2 | duktape.c |
| duk__load_buffer_raw | 2 | duktape.c |
| duk__dump_hbuffer_raw | 2 | duktape.c |
| duk__dump_buffer_prop | 2 | duktape.c |
| duk__dump_uint32_prop | 2 | duktape.c |
| duk__dump_varmap | 2 | duktape.c |
| duk__dump_formals | 2 | duktape.c |
| duk_js_closure | 2 | duktape.c |
| duk__pcall_raw | 2 | duktape.c |
| duk__pcall_method_raw | 2 | duktape.c |
| duk__pcall_prop_raw | 2 | duktape.c |
| duk__pnew_helper | 2 | duktape.c |
| duk__base64_encode_fast_2 | 2 | duktape.c |
| duk__base64_encode_fast_1 | 2 | duktape.c |
| duk__inspect_multiple_uint | 2 | duktape.c |
| duk__get_symbol_type | 2 | duktape.c |
| duk__get_symbol_type_string | 2 | duktape.c |
| duk__const_tval_unused | 2 | duktape.c |
| duk_js_ops | 2 | duktape.c |
| duk__toint_coercer | 2 | duktape.c |
| duk__push_func_from_lightfunc | 2 | duktape.c |
| duk_ssize_t | 2 | duktape.c |
| duk__try_push_vsprintf | 2 | duktape.c |
| duk__validate_push_heapptr | 2 | duktape.c |
| duk__array_pop_fastpath | 2 | duktape.c |
| duk__array_push_fastpath | 2 | duktape.c |
| duk__debuglog_qsort_state | 2 | duktape.c |
| duk__buffer_proto_from_classnum | 2 | duktape.c |
| duk__hbufobj_promote_this | 2 | duktape.c |
| duk__get_bufobj_this | 2 | duktape.c |
| duk__arraybuffer_plain_slice | 2 | duktape.c |
| duk__cbor_encode_objarr_entry | 2 | duktape.c |
| duk__cbor_encode_objarr_exit | 2 | duktape.c |
| duk__cbor_encode_ensure_slowpath | 2 | duktape.c |
| duk__cbor_encode_double_fp | 2 | duktape.c |
| duk__cbor_encode_object | 2 | duktape.c |
| duk__cbor_encode_buffer | 2 | duktape.c |
| duk__cbor_encode_pointer | 2 | duktape.c |
| duk__cbor_encode_lightfunc | 2 | duktape.c |
| duk__cbor_decode_peekbyte | 2 | duktape.c |
| duk__cbor_decode_rewind | 2 | duktape.c |
| duk__cbor_decode_join_buffers | 2 | duktape.c |
| duk__parse_string_iso8601_subset | 2 | duktape.c |
| duk__year_from_day | 2 | duktape.c |
| duk__make_day | 2 | duktape.c |
| duk__format_parts_iso8601 | 2 | duktape.c |
| duk__to_string_helper | 2 | duktape.c |
| duk__convert_filetime_to_ularge | 2 | duktape.c |
| duk__utf8_decode_next | 2 | duktape.c |
| duk__utf8_encode_char | 2 | duktape.c |
| duk__encode_uriunescaped_table | 2 | duktape.c |
| duk__encode_uricomponent_unescaped_table | 2 | duktape.c |
| duk__decode_uri_reserved_table | 2 | duktape.c |
| duk__decode_uri_component_reserved_table | 2 | duktape.c |
| duk__escape_unescaped_table | 2 | duktape.c |
| duk__transform_callback | 2 | duktape.c |
| duk__transform_callback_escape | 2 | duktape.c |
| duk__transform_callback_unescape | 2 | duktape.c |
| duk__json_quotestr_lookup | 2 | duktape.c |
| duk__json_stringify_fast | 2 | duktape.c |
| duk_hstrings | 2 | duktape.c |
| duk__fmin_fixed | 2 | duktape.c |
| duk__fmax_fixed | 2 | duktape.c |
| duk__fabs | 2 | duktape.c |
| duk__acos | 2 | duktape.c |
| duk__asin | 2 | duktape.c |
| duk__atan | 2 | duktape.c |
| duk__ceil | 2 | duktape.c |
| duk__cos | 2 | duktape.c |
| duk__exp | 2 | duktape.c |
| duk__floor | 2 | duktape.c |
| duk__log | 2 | duktape.c |
| duk__sin | 2 | duktape.c |
| duk__sqrt | 2 | duktape.c |
| duk__tan | 2 | duktape.c |
| duk__print_opcode | 2 | duktape.c |
| duk__print_catcher | 2 | duktape.c |
| duk__print_activation | 2 | duktape.c |
| duk_debug_read_flush | 2 | duktape.c |
| duk__debug_read_int32_raw | 2 | duktape.c |
| duk__debug_read_double_raw | 2 | duktape.c |
| duk__debug_skip_dvalue | 2 | duktape.c |
| duk__debug_skip_to_eom | 2 | duktape.c |
| duk__debug_read_validate_csindex | 2 | duktape.c |
| duk__debug_handle_basic_info | 2 | duktape.c |
| duk__debug_handle_trigger_status | 2 | duktape.c |
| duk__debug_handle_pause | 2 | duktape.c |
| duk__debug_handle_resume | 2 | duktape.c |
| duk__debug_handle_step | 2 | duktape.c |
| duk__debug_handle_list_break | 2 | duktape.c |
| duk__debug_handle_add_break | 2 | duktape.c |
| duk__debug_handle_del_break | 2 | duktape.c |
| duk__debug_handle_get_var | 2 | duktape.c |
| duk__debug_handle_put_var | 2 | duktape.c |
| duk__debug_handle_get_call_stack | 2 | duktape.c |
| duk__debug_handle_get_locals | 2 | duktape.c |
| duk__debug_handle_eval | 2 | duktape.c |
| duk__debug_handle_detach | 2 | duktape.c |
| duk__debug_handle_apprequest | 2 | duktape.c |
| duk__debug_dump_heap_allocated | 2 | duktape.c |
| duk__debug_dump_strtab | 2 | duktape.c |
| duk__debug_handle_dump_heap | 2 | duktape.c |
| duk__debug_handle_get_bytecode | 2 | duktape.c |
| duk__debug_getinfo_heaphdr_keys | 2 | duktape.c |
| duk__debug_getinfo_heaphdr_masks | 2 | duktape.c |
| duk__debug_handle_get_heap_obj_info | 2 | duktape.c |
| duk__debug_handle_get_obj_prop_desc | 2 | duktape.c |
| duk__debug_handle_get_obj_prop_desc_range | 2 | duktape.c |
| duk__add_traceback | 2 | duktape.c |
| duk__add_fileline | 2 | duktape.c |
| duk__add_compiler_error_line | 2 | duktape.c |
| duk__err_augment_builtin_create | 2 | duktape.c |
| duk__uncaught_readable | 2 | duktape.c |
| duk__have_active_catcher | 2 | duktape.c |
| duk__heap_free_activation_freelist | 2 | duktape.c |
| duk__heap_free_catcher_freelist | 2 | duktape.c |
| duk__free_allocated | 2 | duktape.c |
| duk__free_finalize_list | 2 | duktape.c |
| duk__free_stringtable | 2 | duktape.c |
| duk__free_run_finalizers | 2 | duktape.c |
| duk__init_heap_thread | 2 | duktape.c |
| duk__dump_type_sizes | 2 | duktape.c |
| duk_small_int_fast_t | 2 | duk_config.h, duktape.c |
| duk__dump_type_limits | 2 | duktape.c |
| duk__dump_misc_options | 2 | duktape.c |
| duk__fake_global_finalizer | 2 | duktape.c |
| duk__run_global_torture_finalizer | 2 | duktape.c |
| duk__finalize_helper | 2 | duktape.c |
| duk__wipe_litcache | 2 | duktape.c |
| duk__protected_compact_object | 2 | duktape.c |
| duk__gc_heaphdr_assert | 2 | duktape.c |
| duk__gc_hstring_assert | 2 | duktape.c |
| duk__assert_heaphdr_flags_cb | 2 | duktape.c |
| duk__assert_validity_cb2 | 2 | duktape.c |
| duk__assert_valid_refcounts_cb | 2 | duktape.c |
| duk__clear_assert_refcounts_cb2 | 2 | duktape.c |
| duk__clear_assert_refcounts | 2 | duktape.c |
| duk__check_assert_refcounts_cb2 | 2 | duktape.c |
| duk__check_assert_refcounts | 2 | duktape.c |
| duk__assert_litcache_nulls | 2 | duktape.c |
| duk__dump_stats | 2 | duktape.c |
| duk__heap_mem_alloc_slowpath | 2 | duktape.c |
| duk__heap_mem_realloc_slowpath | 2 | duktape.c |
| duk__heap_mem_realloc_indirect_slowpath | 2 | duktape.c |
| duk__decref_tvals_norz | 2 | duktape.c |
| duk__refcount_refzero_hobject | 2 | duktape.c |
| duk__refcount_refzero_hstring | 2 | duktape.c |
| duk__refcount_refzero_hbuffer | 2 | duktape.c |
| duk__hstring_refzero_helper | 2 | duktape.c |
| duk__hbuffer_refzero_helper | 2 | duktape.c |
| duk__strtable_alloc_hstring | 2 | duktape.c |
| duk__strtable_resize_torture | 2 | duktape.c |
| duk__strtable_do_intern | 2 | duktape.c |
| duk__strtab_romstring_lookup | 2 | duktape.c |
| duk__strtable_litcache_key | 2 | duktape.c |
| duk__sort_compare_es6 | 2 | duktape.c |
| duk__hobject_pc2line_query_raw | 2 | duktape.c |
| duk__key_is_plain_buf_ownprop | 2 | duktape.c |
| duk__abandon_array_slow_check_required | 2 | duktape.c |
| duk__abandon_array_check | 2 | duktape.c |
| duk__obtain_arridx_slot_slowpath | 2 | duktape.c |
| duk__grow_props_for_new_entry_item | 2 | duktape.c |
| duk_hobject_get_internal_value_heaphdr | 2 | duktape.c |
| duk__getprop_shallow_fastpath_array_tval | 2 | duktape.c |
| duk__putprop_shallow_fastpath_array_tval | 2 | duktape.c |
| duk__getprop_fastpath_bufobj_tval | 2 | duktape.c |
| duk__putprop_fastpath_bufobj_tval | 2 | duktape.c |
| duk__duplicate_ram_global_object | 2 | duktape.c |
| duk__push_stridx | 2 | duktape.c |
| duk__push_double | 2 | duktape.c |
| duk__call_c_recursion_limit_check_slowpath | 2 | duktape.c |
| duk__call_callstack_limit_check_slowpath | 2 | duktape.c |
| duk__call_callstack_limit_check | 2 | duktape.c |
| duk__create_arguments_object | 2 | duktape.c |
| duk__handle_createargs_for_call | 2 | duktape.c |
| duk__handle_bound_chain_for_call | 2 | duktape.c |
| duk__handle_specialfuncs_for_call | 2 | duktape.c |
| duk__handle_proxy_for_call | 2 | duktape.c |
| duk__handle_oldenv_for_call | 2 | duktape.c |
| duk__resolve_target_fastpath_check | 2 | duktape.c |
| duk__resolve_target_func_and_this_binding | 2 | duktape.c |
| duk__call_setup_act_attempt_tailcall | 2 | duktape.c |
| duk__call_setup_act_not_tailcall | 2 | duktape.c |
| duk__call_env_setup | 2 | duktape.c |
| duk__handle_safe_call_inner | 2 | duktape.c |
| duk__comp_recursion_increase | 2 | duktape.c |
| duk__comp_recursion_decrease | 2 | duktape.c |
| duk__emit_a | 2 | duktape.c |
| duk__emit_b | 2 | duktape.c |
| duk__expr_toreg | 2 | duktape.c |
| duk__expr_totemp | 2 | duktape.c |
| duk__expr_totempconst | 2 | duktape.c |
| duk__exprtop_totemp | 2 | duktape.c |
| duk__exprtop_toplain_ignore | 2 | duktape.c |
| duk__peephole_optimize_bytecode | 2 | duktape.c |
| duk__dump_ispec | 2 | duktape.c |
| duk__dump_ivalue | 2 | duktape.c |
| duk__const_needs_refcount | 2 | duktape.c |
| duk__expr_lhs | 2 | duktape.c |
| duk__js_compile_raw | 2 | duktape.c |
| duk__compute_mod | 2 | duktape.c |
| duk__compute_exp | 2 | duktape.c |
| duk__vm_bitwise_not | 2 | duktape.c |
| duk__vm_logical_not | 2 | duktape.c |
| duk__handle_catch_part1 | 2 | duktape.c |
| duk__handle_catch_part2 | 2 | duktape.c |
| duk__handle_label | 2 | duktape.c |
| duk__handle_longjmp | 2 | duktape.c |
| duk_bi_duk_object_yield | 2 | duktape.c |
| duk_bi_duk_object_resume | 2 | duktape.c |
| duk__interrupt_handle_debugger | 2 | duktape.c |
| duk__executor_interrupt | 2 | duktape.c |
| duk__executor_recheck_debugger | 2 | duktape.c |
| duk__handle_op_initset_initget | 2 | duktape.c |
| duk__handle_op_trycatch | 2 | duktape.c |
| duk__handle_op_endtry | 2 | duktape.c |
| duk__handle_op_endcatch | 2 | duktape.c |
| duk__handle_op_endfin | 2 | duktape.c |
| duk__handle_op_initenum | 2 | duktape.c |
| duk__handle_op_nextenum | 2 | duktape.c |
| duk__tonumber_string_raw | 2 | duktape.c |
| duk__js_samevalue_number | 2 | duktape.c |
| duk__compare_fastint | 2 | duktape.c |
| duk__inc_data_inner_refcounts | 2 | duktape.c |
| duk__preallocate_env_entries | 2 | duktape.c |
| duk__getid_open_decl_env_regs | 2 | duktape.c |
| duk__getid_activation_regs | 2 | duktape.c |
| duk__declvar_helper | 2 | duktape.c |
| duk__is_hex_digit | 2 | duktape.c |
| duk__advance_chars | 2 | duktape.c |
| duk__initbuffer | 2 | duktape.c |
| duk__appendbuffer | 2 | duktape.c |
| duk__appendbuffer_ascii | 2 | duktape.c |
| duk__read_char | 2 | duktape.c |
| duk__lexer_parse_string_literal | 2 | duktape.c |
| duk__bi_print | 2 | duktape.c |
| duk__bi_add_small | 2 | duktape.c |
| duk__bi_sub_copy | 2 | duktape.c |
| duk__bi_is_even | 2 | duktape.c |
| duk__bi_is_zero | 2 | duktape.c |
| duk__bi_is_2to52 | 2 | duktape.c |
| duk__bi_twoexp | 2 | duktape.c |
| duk__dragon4_convert_and_push | 2 | duktape.c |
| duk__dragon4_double_to_ctx | 2 | duktape.c |
| duk__dragon4_ctx_to_double | 2 | duktape.c |
| duk__numconv_stringify_raw | 2 | duktape.c |
| duk__numconv_parse_raw | 2 | duktape.c |
| duk__append_2bytes | 2 | duktape.c |
| duk__insert_i32 | 2 | duktape.c |
| duk__append_u16_list | 2 | duktape.c |
| duk__insert_slice | 2 | duktape.c |
| duk__remove_slice | 2 | duktape.c |
| duk__re_range_lookup1 | 2 | duktape.c |
| duk__re_range_lookup2 | 2 | duktape.c |
| duk__append_range_atom_matcher | 2 | duktape.c |
| duk__parse_regexp_flags | 2 | duktape.c |
| duk__create_escaped_source | 2 | duktape.c |
| duk__utf8_backtrack | 2 | duktape.c |
| duk__utf8_advance | 2 | duktape.c |
| duk__test_u32_union | 2 | duktape.c |
| duk__selftest_types | 2 | duktape.c |
| duk__selftest_packed_tval | 2 | duktape.c |
| duk__selftest_twos_complement | 2 | duktape.c |
| duk__selftest_byte_order | 2 | duktape.c |
| duk__selftest_bswap_macros | 2 | duktape.c |
| duk__selftest_double_union_size | 2 | duktape.c |
| duk__selftest_double_aliasing | 2 | duktape.c |
| duk__selftest_double_zero_sign | 2 | duktape.c |
| duk__selftest_double_rounding | 2 | duktape.c |
| duk__selftest_fmod | 2 | duktape.c |
| duk__selftest_struct_align | 2 | duktape.c |
| duk__selftest_64bit_arithmetic | 2 | duktape.c |
| duk__selftest_cast_double_to_small_uint | 2 | duktape.c |
| duk__selftest_cast_double_to_uint32 | 2 | duktape.c |
| duk__selftest_alloc_funcs | 2 | duktape.c |
| duk__rnd_splitmix64 | 2 | duktape.c |
| duk__xoroshiro128plus | 2 | duktape.c |
| duk_push_bool | 1 | DOMImplementation.bnd |
| duk_create_heap_default | 1 | duktape.h |
| duk_error_va | 1 | duktape.h |
| duk_generic_error_va | 1 | duktape.h |
| duk_eval_error_va | 1 | duktape.h |
| duk_range_error_va | 1 | duktape.h |
| duk_reference_error_va | 1 | duktape.h |
| duk_syntax_error_va | 1 | duktape.h |
| duk_type_error_va | 1 | duktape.h |
| duk_uri_error_va | 1 | duktape.h |
| duk_xmove_top | 1 | duktape.h |
| duk_xcopy_top | 1 | duktape.h |
| duk_push_thread_new_globalenv | 1 | duktape.h |
| duk_push_error_object_va | 1 | duktape.h |
| duk_push_external_buffer | 1 | duktape.h |
| duk_is_none | 1 | duktape.h |
| duk_is_object_coercible | 1 | duktape.h |
| duk_is_eval_error | 1 | duktape.h |
| duk_is_range_error | 1 | duktape.h |
| duk_is_reference_error | 1 | duktape.h |
| duk_is_syntax_error | 1 | duktape.h |
| duk_is_type_error | 1 | duktape.h |
| duk_is_uri_error | 1 | duktape.h |
| duk_get_undefined | 1 | duktape.h |
| duk_get_null | 1 | duktape.h |
| duk_require_length | 1 | duktape.h |
| duk_opt_length | 1 | duktape.h |
| duk_eval_noresult | 1 | duktape.h |
| duk_peval | 1 | duktape.h |
| duk_peval_noresult | 1 | duktape.h |
| duk_eval_string | 1 | duktape.h |
| duk_peval_string | 1 | duktape.h |
| duk_peval_string_noresult | 1 | duktape.h |
| duk_compile_string | 1 | duktape.h |
| duk_compile_string_filename | 1 | duktape.h |
| duk_pcompile_string | 1 | duktape.h |
| duk_pcompile_string_filename | 1 | duktape.h |
| duk_eval_lstring | 1 | duktape.h |
| duk_eval_lstring_noresult | 1 | duktape.h |
| duk_peval_lstring | 1 | duktape.h |
| duk_peval_lstring_noresult | 1 | duktape.h |
| duk_compile_lstring | 1 | duktape.h |
| duk_compile_lstring_filename | 1 | duktape.h |
| duk_pcompile_lstring | 1 | duktape.h |
| duk_rom_compressed_pointers | 1 | duktape.h |
| duk_dblunion | 1 | duktape.c |
| duk_fltunion | 1 | duktape.c |
| duk_replacements | 1 | duktape.c |
| duk_exception | 1 | duktape.c |
| duk_builtins | 1 | duktape.c |
| duk_util | 1 | duktape.c |
| duk_bw_remove_ensure_slice | 1 | duktape.c |
| duk_memcpy_safe | 1 | duktape.c |
| duk_js_executor | 1 | duktape.c |
| duk_refcount | 1 | duktape.c |
| duk_dup_m1 | 1 | duktape.c |
| duk_push_u64 | 1 | duktape.c |
| duk_push_ | 1 | duktape.c |
| duk_push_idx | 1 | duktape.c |
| duk_push_hnatfunc | 1 | duktape.c |
| duk_xxx_prop_stridx_short | 1 | duktape.c |
| duk_xdef_prop_stridx_wec | 1 | duktape.c |
| duk_unpack | 1 | duktape.c |
| duk_henv | 1 | duktape.c |
| duk_activations | 1 | duktape.c |
| duk_catchers | 1 | duktape.c |
| duk_util_tinyrandom | 1 | duktape.c |
| duk_debugger | 1 | duktape.c |
| duk_debug_read_pointer | 1 | duktape.c |
| duk_debug_read_buffer | 1 | duktape.c |
| duk_debug_read_hbuffer | 1 | duktape.c |
| duk_debug | 1 | duktape.c |
| duk_unicode | 1 | duktape.c |
| duk_json | 1 | duktape.c |
| duk_bi_json | 1 | duktape.c |
| duk_js | 1 | duktape.c |
| duk_js_lessthan | 1 | duktape.c |
| duk_js_greaterthan | 1 | duktape.c |
| duk_js_lessthanorequal | 1 | duktape.c |
| duk_js_greaterthanorequal | 1 | duktape.c |
| duk_bi_protos | 1 | duktape.c |
| duk_selftest | 1 | duktape.c |
| duk_regexp_executor | 1 | duktape.c |
| duk_push_string_file_raw | 1 | duktape.c |
| duk_put_prop_xxx | 1 | duktape.c |
| duk_hobject_enum | 1 | duktape.c |
| duk_push_hobject_or_undefined | 1 | duktape.c |
| duk_known_xxx | 1 | duktape.c |
| duk_bi_buffer | 1 | duktape.c |
| duk_get_monotonic_time | 1 | duktape.c |
| duk__cbor_decode_ensure | 1 | duktape.c |
| duk_push_fixed_buffer_with_data | 1 | duktape.c |
| duk_push_bound_function | 1 | duktape.c |
| duk_xdef_prop_xxx | 1 | duktape.c |
| duk_dup_unvalidated | 1 | duktape.c |
| duk_bi_performance_timeorigin_getter | 1 | duktape.c |
| duk_bi_promise_constructor | 1 | duktape.c |
| duk_bi_promise_all | 1 | duktape.c |
| duk_bi_promise_race | 1 | duktape.c |
| duk_bi_promise_reject | 1 | duktape.c |
| duk_bi_promise_resolve | 1 | duktape.c |
| duk_bi_promise_catch | 1 | duktape.c |
| duk_bi_promise_then | 1 | duktape.c |
| duk_bi_object | 1 | duktape.c |
| duk_bi_function | 1 | duktape.c |
| duk_auto_unbox_tval | 1 | duktape.c |
| duk_auto_unbox | 1 | duktape.c |
| duk_require_func_promote_lfunc | 1 | duktape.c |
| duk_hashstring | 1 | duktape.c |
| duk_hstring_refzero_norz | 1 | duktape.c |
| duk_hbuffer_refzero_norz | 1 | duktape.c |
| duk_get_prop_stridx_top | 1 | duktape.c |
| duk_clone_hobject | 1 | duktape.c |
| duk_rom_builtins_bidx | 1 | duktape.c |
| duk_replace_tval | 1 | duktape.c |
| duk_pack_slice | 1 | duktape.c |
| duk_js_var | 1 | duktape.c |
| duk_handle_call_protected | 1 | duktape.c |
| duk__parse_XXX | 1 | duktape.c |
| duk_error_augment | 1 | duktape.c |
| duk__push_tvals_incref_only | 1 | duktape.c |
| duk_small_ | 1 | duktape.c |
| duk_js_call | 1 | duktape.c |
| duk__bi_add_copy | 1 | duktape.c |
| duk__bi_sub_small | 1 | duktape.c |
| duk__bigints | 1 | duktape.c |
| duk__append_i32 | 1 | duktape.c |
| duk_double_ | 1 | duktape.c |
| duk_libdom | 1 | netsurf.bnd |
| duk_arridx_t | 1 | duk_config.h |
| duk_custom | 1 | duk_config.h |
