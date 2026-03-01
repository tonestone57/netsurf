# Implementation Plan for Swapping Duktape with QuickJS in NetSurf

## 1. Audit Phase (Completed)
Detailed audit of Duktape API usage in NetSurf bindings has been performed. Key findings:
- ~21,000 calls to `duk_*` APIs across 67 `.bnd` files.
- Common patterns identified for stack manipulation and private data access.

## 2. nsgenbind Backend (Prototype Created)
A new backend has been prototyped for the `nsgenbind` tool.
- File: `docs/nsgenbind_prototype/qjs-libdom.c`
- Deliverables:
    - Logic for `JSClassDef` and `JSCFunction` generation.
    - Opaque pointer mapping for private data.

## 3. NetSurf Integration (Phase 1 & 2 Framework)
- Directory: `content/handlers/javascript/quickjs/`
- Engine: `quickjs.c` implements `js.h` with:
    - Runtime and Context management.
    - Global intrinsic objects and console integration.
    - Window.alert and timer system foundations.
- Layer: `qjsky.c` handles node mapping and string conversions.
- Web APIs: `xhr.c` provides a skeleton for XMLHttpRequest.

## 4. Build System (Integrated)
- `NETSURF_USE_QUICKJS` option added for engine selection.
- Automated `nsgenbind` invocation for QuickJS target included in Makefiles.

## 5. Implementation Roadmap (Next Steps)
1. **Functional Timer System**: Complete the bridge between NetSurf's scheduler and QuickJS execution.
2. **Event Dispatch Logic**: Finalize the conversion of DOM events into JS function calls.
3. **Binding Regeneration**: Use the new nsgenbind backend to generate the full DOM API.
4. **Refcount Bridge**: Ensure robust memory management across the C/JS boundary.
