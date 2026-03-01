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

## 5. Next Steps for Full Implementation
1. Install `bison`, `flex`, and `gperf` in the build environment to enable full library compilation.
2. Complete the `nsgenbind` QuickJS backend implementation.
3. Gradually migrate `.bnd` files to use the compatibility layer or a new generic JS API.
4. Port all NetSurf "Dukky" features (Event mapping, Window timers, Console integration) to the QuickJS handler.
