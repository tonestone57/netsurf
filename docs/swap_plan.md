# Implementation Plan for Swapping Duktape with QuickJS in NetSurf

## 1. Audit Phase (Completed)
Detailed audit of Duktape API usage in NetSurf bindings has been performed. Key findings:
- ~21,000 calls to `duk_*` APIs across 67 `.bnd` files.
- Common patterns: stack manipulation (`duk_push_*`, `duk_get_*`, `duk_pop`), object property access (`duk_get_prop_string`), and private data storage (`duk_get_pointer`).

## 2. nsgenbind Backend (Prototype Created)
A new backend has been prototyped for the `nsgenbind` tool.
- File: `docs/nsgenbind_prototype/qjs-libdom.c`
- Deliverables:
    - Generated `JSClassDef` and `JSCFunction` templates.
    - Logic for `JS_GetOpaque` mapping to private data structures.
    - Patch for `nsgenbind` main logic to recognize `qjs_libdom`.

## 3. NetSurf Integration (Phase 1 Implemented)
- Directory: `content/handlers/javascript/quickjs/`
- Compatibility: `duktape_compat.h` updated with initial engine-neutral "NSJS" macros.
- Engine: `quickjs.c` implements the `js.h` interface with intrinsic object support and memory limits.
- Layer: `qjsky.c` implements DOM node mapping, reference counting via finalizers, and basic `console` support.

## 4. Build System (Integrated)
- `NETSURF_USE_QUICKJS` option added to `Makefile.defaults`.
- Conditional inclusion in `content/handlers/javascript/Makefile`.
- QuickJS Makefile template created.

## 5. Next Steps for Full Implementation

### Phase 2: Full Binding Generation
1. Integrate the `qjs-libdom` backend fully into the `nsgenbind` repository.
2. Regenerate all DOM bindings using the new backend.
3. Fix any compilation errors in the generated `.c` files.

### Phase 3: Functionality Completion
4. Implement `qjsky_register_event_listener` and the event dispatch bridge.
5. Fully implement Window timers (`setTimeout`/`setInterval`).
6. Implement `XMLHttpRequest` and other asynchronous Web APIs.
7. Migrate more `.bnd` files to use the engine-neutral NSJS API.
