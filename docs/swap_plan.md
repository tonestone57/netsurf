# Implementation Plan for Swapping Duktape with QuickJS in NetSurf

## 1. Audit Phase (Completed)
Detailed audit of Duktape API usage in NetSurf bindings has been performed. Key findings:
- ~21,000 calls to `duk_*` APIs across 67 `.bnd` files.
- Common patterns identified for stack manipulation and private data access.

## 2. nsgenbind Backend (Prototype Created)
A new backend has been prototyped for the `nsgenbind` tool.
- File: `docs/nsgenbind_prototype/qjs-libdom.c`
- Deliverables:
    - Generated `JSClassDef` and `JSCFunction` templates.
    - Opaque pointer mapping for private data.
    - Constant export pattern.

## 3. NetSurf Integration (Functional Framework Implemented)
- Directory: `content/handlers/javascript/quickjs/`
- Engine: `quickjs.c` implements the spec-compliant `js.h` adapter layer with:
    - Runtime/Context lifecycle (mapping NetSurf `jsheap`/`jsthread` to QuickJS).
    - Memory limits and intrinsic initialization.
    - [COMPLETED] Event dispatch and element attribute scanning adapter.
- [COMPLETED] Window.alert bridge.
- [COMPLETED] Timer system implementation (setTimeout/setInterval).
    - [COMPLETED] Fixed UTF-8 string encoding across all binding layers.
- Layer: `qjsky.c` handles:
    - [COMPLETED] Robust DOM node memoization (using BigUint64 for pointer safety).
    - String conversion between `JSValue` and `dom_string`.
    - Native `console` routing to `NSLOG`.
- Web APIs: `xhr.c` provides constructor and prototype framework for `XMLHttpRequest`.

## 4. Build System (Integrated)
- `NETSURF_USE_QUICKJS` option added for engine selection.
- Automated `nsgenbind` invocation for QuickJS target included in Makefiles.

## 5. Implementation Roadmap (Next 12 Tasks)

### Phase 1: Binding Generation (Backend Completion)
1.  **IR Mapping**: Implement full IR-to-QuickJS mapping in `nsgenbind` for interfaces and dictionaries.
2.  **Parameter Unpacking**: Implement C-side coercion for QuickJS `argv` to libdom types in generated code.
3.  **Return Conversion**: Implement conversion of libdom return values back to `JSValue` (with proper ref-counting).
4.  **Backend Integration**: Merge the `qjs_libdom` backend into the official `nsgenbind` repository.

### Phase 2: Core Browser API (Glue Layer Completion)
5.  **[COMPLETED] Event Listeners**: Implement `addEventListener` and `removeEventListener` in `qjsky.c`.
6.  **Event Construction**: Port the `Event` class hierarchy (UIEvent, MouseEvent) to the new engine.
7.  **Timer Refinement**: Implement `clearTimeout`/`clearInterval` and robust timer tracking.
8.  **Location API**: Port the `Location` object and navigation hooks to QuickJS.

### Phase 3: Web Features & Refinement
9.  **Asynchronous XHR**: Connect `xhr.c` to NetSurf's asynchronous fetcher and implement readyState transitions.
10. **Cookie/Storage API**: Implement the `Document.cookie` and `localStorage` bridges.
11. **Memory Safety**: Migrate the memoization map to a native-managed registry to eliminate the current strong-reference leak.
12. **IDL Migration**: Systematic migration of all 67 `.bnd` binding definitions to the new engine-neutral API.
