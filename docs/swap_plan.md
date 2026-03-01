# Implementation Plan for Swapping Duktape with QuickJS in NetSurf

## 1. Audit Phase (Completed)
Detailed audit of Duktape API usage in NetSurf bindings has been performed. Key findings:
- ~21,000 calls to `duk_*` APIs across 67 `.bnd` files.
- Common patterns: stack manipulation (`duk_push_*`, `duk_get_*`, `duk_pop`), object property access (`duk_get_prop_string`), and private data storage (`duk_get_pointer`).

## 2. nsgenbind Backend (Required)
A new backend must be added to the `nsgenbind` tool.
- File: `nsgenbind/src/qjs-libdom.c`
- Tasks:
    - Implement `qjs_libdom_output` to generate `binding.c/h`, `private.h`, and individual interface `.c` files.
    - Map WebIDL interfaces to `JSClassID` and `JSClassDef`.
    - Generate `JSCFunction` wrappers for methods and `JSGetterSetter` for attributes.

## 3. NetSurf Integration (Prototype Framework Created)
- Directory: `content/handlers/javascript/quickjs/`
- Compatibility: `duktape_compat.h` created to provide a macro-based mapping for common Duktape calls.
- Engine: `quickjs.c` implements the `js.h` interface.
- Layer: `qjsky.c` implements the DOM node memoization and life-cycle management.

## 4. Build System (Integrated)
- `NETSURF_USE_QUICKJS` option added to `Makefile.defaults`.
- Conditional inclusion in `content/handlers/javascript/Makefile`.
- QuickJS Makefile template created.

## 5. Next Steps for Full Implementation (25-Task Roadmap)

### Phase 1: nsgenbind Backend Completion
1.  **Refine Backend Structure**: Finalize the code generation framework in `qjs-libdom.c` for headers and common binding code.
2.  **Interface Generation**: Implement logic to generate QuickJS `JSClassDef` and `JS_NewClass` calls for each WebIDL interface.
3.  **Method Wrapping**: Implement generation of `JSCFunction` wrappers that map QuickJS `argv` to C parameters.
4.  **Attribute Accessors**: Implement generation of getter/setter functions for WebIDL attributes.
5.  **Constant Export**: Add logic to export WebIDL-defined constants to QuickJS prototypes.

### Phase 2: NetSurf QuickJS Handler (Qjsky)
6.  **DOM Node Memoization**: Implement a robust mapping between `dom_node*` pointers and `JSValue` objects in `qjsky.c`.
7.  **Event Target Support**: Port the event listener registration and dispatch logic to QuickJS.
8.  **Window Timer API**: Implement `setTimeout`/`setInterval` using NetSurf's scheduler with QuickJS callback handles.
9.  **Console Integration**: Implement the `Console` IDL by routing JS calls to NetSurf's `NSLOG` system.

### Phase 3: Migration and Testing
10. **Engine Abstraction Layer**: Introduce engine-neutral macros in `duktape_compat.h` (to be renamed to `nsjs.h`) to decouple `.bnd` files.
11. **Refcount Integration**: Finalize the garbage collection bridge between QuickJS reference counting and libdom refcounting.
12. **Integration Testing**: Execute end-to-end JavaScript tests using the `monkey` frontend and verify against existing benchmarks.

### Phase 4: Additional Integration and Optimization (Extended Tasks)
13. **XMLHttpRequest Support**: Implement base XHR functionality for QuickJS integration.
14. **Document cookie**: Implement getter/setter for document.cookie in QuickJS context.
15. **Window self/window/top**: Implement these standard window properties.
16. **History API**: Implement basic go/back/forward methods.
17. **Location API**: Implement protocol/host/hostname/port/pathname/search/hash accessors.
18. **Navigator API**: Implement userAgent and platform properties.
19. **Performance.now()**: Implement high-resolution timer for performance measurement.
20. **Console log/warn/error**: Map standard console methods to NSLOG levels.
21. **Attribute Node access**: Implement getAttributeNode/setAttributeNode for Elements.
22. **Text Content**: Implement textContent property for Node base class.
23. **Inner HTML**: Implement innerHTML (read-only initially) for Elements.
24. **Base URL handling**: Ensure JS-triggered fetches respect the document base URL.
25. **Memory leak audit**: Use QuickJS's memory tracking to identify and fix leaks in the bridge.
