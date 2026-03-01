/* QuickJS binding generation implementation
 *
 * This file is part of nsgenbind.
 * Licensed under the MIT License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "options.h"
#include "utils.h"
#include "nsgenbind-ast.h"
#include "webidl-ast.h"
#include "ir.h"
#include "output.h"
#include "qjs-libdom.h"

#define QLPFX "qjsky"

/**
 * Generate QuickJS Class Definition
 */
static int output_qjs_class_def(struct opctx *outc, struct ir_entry *ife) {
    outputf(outc, "static JSClassID %s_%s_class_id;\n", QLPFX, ife->class_name);
    outputf(outc, "static JSClassDef %s_%s_class_def = {\n", QLPFX, ife->class_name);
    outputf(outc, "    .class_name = \"%s\",\n", ife->name);
    outputf(outc, "    .finalizer = %s_%s_finalizer,\n", QLPFX, ife->class_name);
    outputf(outc, "};\n\n");
    return 0;
}

/**
 * Generate QuickJS Method Wrapper
 */
static int output_qjs_method(struct opctx *outc, struct ir_entry *ife, struct ir_operation_entry *ope) {
    outputf(outc, "static JSValue %s_%s_%s(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {\n",
            QLPFX, ife->class_name, ope->name);
    outputf(outc, "    %s_private_t *priv = JS_GetOpaque(this_val, %s_%s_class_id);\n",
            ife->class_name, QLPFX, ife->class_name);
    outputf(outc, "    if (!priv) return JS_EXCEPTION;\n");
    outputf(outc, "    /* Implementation generated from .bnd */\n");
    outputf(outc, "    return JS_UNDEFINED;\n");
    outputf(outc, "}\n\n");
    return 0;
}

int qjs_libdom_output(struct ir *ir)
{
    int idx;
    struct opctx *outc;

    /* Logic to iterate over IR and generate files */
    for (idx = 0; idx < ir->entryc; idx++) {
        struct ir_entry *ife = ir->entries + idx;
        if (ife->type != IR_ENTRY_TYPE_INTERFACE) continue;

        /* In a real implementation, we would open a file here */
        // output_open(ife->filename, &outc);

        // output_qjs_class_def(outc, ife);
        // ... generate methods and attributes ...
    }

    printf("QuickJS binding generator prototype updated with ClassDef and Method templates.\n");
    return 0;
}
