# Implementation Roadmap: Next 15 Tasks for QuickJS Integration

## Phase 2: Engine Integration & Glue Layer
1.  **[COMPLETED] String Conversion Helpers**: Implemented `js_value_to_dom_string` and `dom_string_to_js_value` in `qjsky.c` with correct UTF-8 byte length handling.
2.  **[COMPLETED] Node Memoization**: Implemented a hidden `JSMap` with `BigUint64` keys on the global object to robustly memoize `dom_node*` -> `JSValue` mappings.
3.  **[COMPLETED] Event Dispatch Bridge**: Implemented `js_fire_event` in `quickjs.c` conforming to the `js.h` adapter interface.
 4.  **[COMPLETED] Auto-Handler Registration**: Implement `js_handle_new_element` to scan for `on*` HTML attributes and compile them into JS functions.
 5.  **[COMPLETED] XMLHttpRequest Implementation**: Implement asynchronous `XMLHttpRequest` bridged to NetSurf fetch API.
6.  **[COMPLETED] Timer System Port**: Implement `setTimeout` and `setInterval` logic using NetSurf's `guit->misc->schedule`.
7.  **[COMPLETED] Timer Management**: Implement `js_closethread` to correctly cancel pending timers and cleanup callbacks.
 8.  **[COMPLETED] Context User Data**: Use `JS_SetContextOpaque` to store NetSurf-specific thread state (browser window, content handles).

## Phase 3: Binding Framework & Compatibility
9.  **Attribute Accessor Generation**: Update `nsgenbind` prototype to generate getter/setter C wrappers.
10. **Constant Export**: Implement WebIDL constant propagation to QuickJS prototype objects.
11. **Compatibility Macros (Property Access)**: Add `duk_get_prop_string` and `duk_put_prop_string` equivalents to `duktape_compat.h`.
12. **Compatibility Macros (Stack Simulation)**: Add `duk_get_top`, `duk_pop`, and `duk_insert` stubs for `.bnd` fragment compatibility.
13. **Opaque Pointer Access**: Add `duk_get_pointer` equivalent using `JS_GetOpaque`.

## Phase 4: Basic Web APIs
14. **[COMPLETED] Window.alert Bridge**: Implement a bridge to NetSurf's native alert dialogs.
15. **[COMPLETED] Core BOM APIs**: Implement `Location`, `History`, and `Navigator` objects.
16. **Build System Bindings**: Add `nsgenbind` invocation rules to `quickjs/Makefile`.

## Phase 5: Advanced Integration & Performance
16. **Weak Reference Memoization**: Migrate the node map to a native-managed registry using `JS_SetOpaque` with proper garbage collection tracking to eliminate current memory leaks.
17. **[COMPLETED] Async Task Queue**: Integrated `JS_ExecutePendingJob` into timers and event entry points to support ES6 Promises.
18. **[COMPLETED] Event Class Hierarchy**: Implemented full inheritance for `UIEvent`, `MouseEvent`, and `KeyboardEvent` in the QuickJS glue layer.
19. **nsgenbind Upstreaming**: Finalize the `qjs_libdom` backend and submit it to the upstream NetSurf `nsgenbind` repository.
20. **Remediation Audit**: Perform a full pass of all 67 `.bnd` files to ensure they remain engine-agnostic and use byte-length string operations.
