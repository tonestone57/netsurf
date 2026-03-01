# Implementation Roadmap: Next 15 Tasks for QuickJS Integration

## Phase 2: Engine Integration & Glue Layer
1.  **String Conversion Helpers**: Implement `js_value_to_dom_string` and `dom_string_to_js_value` in `qjsky.c`.
2.  **Node Memoization**: Implement a `JSMap` or hidden property on the global object to memoize `dom_node*` -> `JSValue` mappings.
3.  **Event Dispatch Bridge**: Implement `js_fire_event` in `quickjs.c` to translate NetSurf events into JS function calls.
4.  **Auto-Handler Registration**: Implement `js_handle_new_element` to scan for `on*` HTML attributes.
5.  **XMLHttpRequest Skeleton**: Create the base implementation for `XMLHttpRequest` in `quickjs/`.
6.  **Timer System Port**: Implement `setTimeout` and `setInterval` logic using NetSurf's `guit->misc->schedule`.
7.  **Timer Management**: Implement `js_closethread` to correctly cancel pending timers and cleanup callbacks.
8.  **Context User Data**: Use `JS_SetContextOpaque` to store NetSurf-specific thread state (browser window, content handles).

## Phase 3: Binding Framework & Compatibility
9.  **Attribute Accessor Generation**: Update `nsgenbind` prototype to generate getter/setter C wrappers.
10. **Constant Export**: Implement WebIDL constant propagation to QuickJS prototype objects.
11. **Compatibility Macros (Property Access)**: Add `duk_get_prop_string` and `duk_put_prop_string` equivalents to `duktape_compat.h`.
12. **Compatibility Macros (Stack Simulation)**: Add `duk_get_top`, `duk_pop`, and `duk_insert` stubs for `.bnd` fragment compatibility.
13. **Opaque Pointer Access**: Add `duk_get_pointer` equivalent using `JS_GetOpaque`.

## Phase 4: Basic Web APIs
14. **Window.alert Bridge**: Implement a bridge to NetSurf's native alert dialogs.
15. **Build System Bindings**: Add `nsgenbind` invocation rules to `quickjs/Makefile`.
