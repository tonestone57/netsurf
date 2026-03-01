# QuickJS Mapping for NetSurf WebIDL Bindings

This document outlines how WebIDL constructs should be mapped to the QuickJS API by the new `nsgenbind` backend.

## 1. Class and Instance Representation

### Class Definition
Each WebIDL interface maps to a QuickJS `JSClassID`.
- **Duktape**: Used a shared "magic property" for private data.
- **QuickJS**: Uses `JS_NewClassID`, `JSClassDef`, and `JS_NewClass`.
  ```c
  static JSClassID nsjs_window_class_id;
  static JSClassDef nsjs_window_class_def = {
      "Window",
      .finalizer = nsjs_window_finalizer,
  };
  ```

### Private Data
- **Duktape**: Stored in a hidden property accessed via `duk_get_pointer`.
- **QuickJS**: Stored using `JS_SetOpaque` and retrieved via `JS_GetOpaque`.
  ```c
  window_private_t *priv = JS_GetOpaque(this_val, nsjs_window_class_id);
  ```

## 2. Methods and Attributes

### Methods
- **Duktape**: `duk_ret_t func(duk_context *ctx)`
- **QuickJS**: `JSValue func(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)`
  - Parameter checking is done using `argc` and `JS_To...` functions.

### Attributes (Getters/Setters)
- **Duktape**: Separate `duk_ret_t` functions.
- **QuickJS**: `JSValue getter(JSContext *ctx, JSValueConst this_val)` and `JSValue setter(JSContext *ctx, JSValueConst this_val, JSValueConst val)`.

## 3. Prototype and Inheritance
Inheritance is implemented by setting the prototype of the subclass constructor/prototype object to the prototype of the superclass.
```c
JS_SetPrototype(ctx, sub_proto, super_proto);
```

## 4. Parameter Coercion Mapping

| WebIDL Type | QuickJS API |
|-------------|-------------|
| `boolean`   | `JS_ToBool` |
| `long`      | `JS_ToInt32` |
| `double`    | `JS_ToFloat64` |
| `DOMString` | `JS_ToCString` / `JS_FreeCString` |
| `object`    | `JS_IsObject` |

## 5. Return Value Handling
Functions must return a `JSValue`. Reference counting (`JS_DupValue` / `JS_FreeValue`) must be strictly followed to prevent memory leaks, especially when returning objects or strings.

## 6. Support Layer (Qjsky)
A support layer will provide common functionality:
- `qjsky_push_node(ctx, dom_node)`: Wraps a libdom node in a QuickJS object.
- `qjsky_get_node(ctx, js_val)`: Unwraps a QuickJS object to get the libdom node.
