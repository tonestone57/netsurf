# nsgenbind Backend Architecture Analysis

The `nsgenbind` tool uses a modular backend system to generate JavaScript engine bindings from WebIDL and `.bnd` configuration files.

## Core Flow
1. **Parser**: Parses WebIDL and `.bnd` into an AST.
2. **Intermediate Representation (IR)**: The AST is converted into an IR (`ir.h`) that represents interfaces, dictionaries, methods, and attributes.
3. **Backend Selection**: Currently, only the `duk-libdom` backend is fully implemented.
4. **Code Generation**: The backend iterates over the IR and generates C source files.

## Backend Components (Duktape Example)
- `duk-libdom.c`: Entry point, generates common headers (`binding.h`, `private.h`, `prototype.h`) and the main `binding.c`.
- `duk-libdom-interface.c`: Handles generation of individual interface files (e.g., `Window.c`). It generates:
    - Initializers (`___init`) and Finalizers (`___fini`).
    - Constructors (`___constructor`) and Destructors (`___destructor`).
    - Methods (`duk_ret_t` functions using the Duktape stack).
    - Getters and Setters for attributes.
    - Prototype creation logic.
- `duk-libdom-dictionary.c`: Similar to interfaces but for WebIDL dictionaries.
- `duk-libdom-generated.c`: Logic for automatically generating simple getters/setters when no custom C code is provided in the `.bnd` file.

## Key Challenges for QuickJS Swap
1. **Memory Model**: Duktape uses a value stack; QuickJS uses `JSValue` (reference counted) and explicit `argc`/`argv` for function arguments.
2. **Private Data**: Duktape uses internal "magic properties" to store C pointers (`priv`). QuickJS uses `JS_SetOpaque` and `JS_GetOpaque` associated with a `JSClassID`.
3. **Prototypes**: Duktape's prototype chaining is manual via `duk_set_prototype`. QuickJS defines classes with `JSClassDef` and sets prototypes during `JS_NewClass`.
4. **Code Fragments**: The `.bnd` files contain raw C fragments. The generator must ensure the surrounding code (parameter setup, return handling) matches the engine's expected signature.

## Implementation Strategy for new Backend
A new `qjs-libdom` backend should be created by duplicating and adapting the `duk-libdom` logic. It must generate `JSCFunction` compatible wrappers instead of `duk_c_function` wrappers.
