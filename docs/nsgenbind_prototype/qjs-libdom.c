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
#include <errno.h>

#include "options.h"
#include "utils.h"
#include "nsgenbind-ast.h"
#include "webidl-ast.h"
#include "ir.h"
#include "output.h"
#include "qjs-libdom.h"
#include "duk-libdom.h"

#define QLPFX "qjsky"

static struct opctx *qjs_open_header(struct ir *ir, const char *name)
{
    char *fname;
    int fnamel;
    struct opctx *hdrc;
    int res;

    fnamel = strlen(name) + 4;
    fname = malloc(fnamel);
    snprintf(fname, fnamel, "%s.h", name);

    res = output_open(fname, &hdrc);
    free(fname);
    if (res != 0) return NULL;

    output_tool_preface(hdrc);
    output_method_cdata(hdrc, ir->binding_node, GENBIND_METHOD_TYPE_PREFACE);

    outputf(hdrc, "\n#ifndef %s_%s_h\n", QLPFX, name);
    outputf(hdrc, "#define %s_%s_h\n\n", QLPFX, name);

    return hdrc;
}

static int qjs_close_header(struct ir *ir, struct opctx *hdrc)
{
    outputf(hdrc, "\n#endif\n");
    output_method_cdata(hdrc, ir->binding_node, GENBIND_METHOD_TYPE_POSTFACE);
    output_close(hdrc);
    return 0;
}

static int qjs_output_private_header(struct ir *ir)
{
    int idx;
    struct opctx *privc = qjs_open_header(ir, "private");
    if (!privc) return -1;

    outputf(privc, "#include \"utils/nsurl.h\"\n");
    outputf(privc, "#include <dom/dom.h>\n\n");

    for (idx = 0; idx < ir->entryc; idx++) {
        struct ir_entry *interfacee = ir->entries + idx;
        if (interfacee->type == IR_ENTRY_TYPE_INTERFACE && interfacee->u.interface.noobject) continue;

        outputf(privc, "/* Private data for %s */\n", interfacee->name);
        outputf(privc, "typedef struct %s_private_s {\n", interfacee->class_name);

        struct ir_entry *inherite = ir_inherit_entry(ir, interfacee);
        if (inherite != NULL) {
            outputf(privc, "\t%s_private_t parent;\n", inherite->class_name);
        }

        struct genbind_node *priv_node = genbind_node_find_type(genbind_node_getnode(interfacee->class), NULL, GENBIND_NODE_TYPE_PRIVATE);
        while (priv_node != NULL) {
            outputc(privc, '\t');
            output_ctype(privc, priv_node, true);
            outputf(privc, ";\n");
            priv_node = genbind_node_find_type(genbind_node_getnode(interfacee->class), priv_node, GENBIND_NODE_TYPE_PRIVATE);
        }
        outputf(privc, "} %s_private_t;\n\n", interfacee->class_name);
    }
    qjs_close_header(ir, privc);
    return 0;
}

static int qjs_output_prototype_header(struct ir *ir)
{
    struct opctx *protoc = qjs_open_header(ir, "prototype");
    if (!protoc) return -1;

    outputf(protoc, "#include <quickjs.h>\n\n");

    for (int i = 0; i < ir->entryc; i++) {
        struct ir_entry *ife = ir->entries + i;
        if (ife->type == IR_ENTRY_TYPE_INTERFACE && !ife->u.interface.noobject) {
            outputf(protoc, "void %s_init_%s(JSContext *ctx);\n", QLPFX, ife->class_name);
        }
    }
    qjs_close_header(ir, protoc);
    return 0;
}

static int qjs_output_method(struct opctx *outc, struct ir_entry *ife, struct ir_operation_entry *ope)
{
    outputf(outc, "static JSValue %s_%s_%s(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {\n",
            QLPFX, ife->class_name, ope->name);
    outputf(outc, "\t%s_private_t *priv = JS_GetOpaque(this_val, %s_%s_class_id);\n",
            ife->class_name, QLPFX, ife->class_name);
    outputf(outc, "\tif (!priv) return JS_EXCEPTION;\n");
    outputf(outc, "\tdukky_method_preamble();\n");

    int cdatac = output_ccode(outc, ope->method);
    if (cdatac == 0) {
        outputf(outc, "\treturn JS_UNDEFINED;\n");
    } else {
        outputf(outc, "\tdukky_method_epilogue();\n");
    }
    outputf(outc, "}\n\n");
    return 0;
}

static int qjs_output_attribute(struct opctx *outc, struct ir_entry *ife, struct ir_attribute_entry *ate)
{
    outputf(outc, "static JSValue %s_%s_%s_get(JSContext *ctx, JSValueConst this_val) {\n",
            QLPFX, ife->class_name, ate->name);
    outputf(outc, "\t%s_private_t *priv = JS_GetOpaque(this_val, %s_%s_class_id);\n",
            ife->class_name, QLPFX, ife->class_name);
    outputf(outc, "\tif (!priv) return JS_EXCEPTION;\n");
    outputf(outc, "\tdukky_getter_preamble();\n");
    if (output_ccode(outc, ate->getter) == 0) {
        outputf(outc, "\treturn JS_UNDEFINED;\n");
    } else {
        outputf(outc, "\tdukky_getter_epilogue();\n");
    }
    outputf(outc, "}\n\n");

    if (!(ate->modifier & WEBIDL_TYPE_MODIFIER_READONLY) || ate->putforwards) {
        outputf(outc, "static JSValue %s_%s_%s_set(JSContext *ctx, JSValueConst this_val, JSValueConst val) {\n",
                QLPFX, ife->class_name, ate->name);
        outputf(outc, "\t%s_private_t *priv = JS_GetOpaque(this_val, %s_%s_class_id);\n",
                ife->class_name, QLPFX, ife->class_name);
        outputf(outc, "\tif (!priv) return JS_EXCEPTION;\n");
        outputf(outc, "\tdukky_setter_preamble();\n");
        if (output_ccode(outc, ate->setter) == 0) {
            outputf(outc, "\treturn JS_UNDEFINED;\n");
        } else {
            outputf(outc, "\tdukky_setter_epilogue();\n");
        }
        outputf(outc, "}\n\n");
    }
    return 0;
}

static int qjs_output_interface(struct ir *ir, struct ir_entry *ife)
{
    struct opctx *ifop;
    if (output_open(ife->filename, &ifop) != 0) return -1;

    output_tool_preface(ifop);
    output_method_cdata(ifop, ir->binding_node, GENBIND_METHOD_TYPE_PREFACE);
    output_method_cdata(ifop, ife->class, GENBIND_METHOD_TYPE_PREFACE);
    output_tool_prologue(ifop);
    output_method_cdata(ifop, ir->binding_node, GENBIND_METHOD_TYPE_PROLOGUE);
    output_method_cdata(ifop, ife->class, GENBIND_METHOD_TYPE_PROLOGUE);

    outputf(ifop, "#include <quickjs.h>\n");
    outputf(ifop, "#include \"private.h\"\n");
    outputf(ifop, "#include \"prototype.h\"\n");
    outputf(ifop, "#include \"binding.h\"\n");
    outputf(ifop, "#include \"qjs_internal.h\"\n");
    outputf(ifop, "#include \"duktape_compat.h\"\n\n");

    outputf(ifop, "static JSClassID %s_%s_class_id;\n\n", QLPFX, ife->class_name);

    /* Finalizer */
    outputf(ifop, "static void %s_%s_finalizer(JSRuntime *rt, JSValue val) {\n", QLPFX, ife->class_name);
    outputf(ifop, "\t%s_private_t *priv = JS_GetOpaque(val, %s_%s_class_id);\n", ife->class_name, QLPFX, ife->class_name);
    outputf(ifop, "\tif (priv) {\n");
    struct genbind_node *fini_node = genbind_node_find_method(ife->class, NULL, GENBIND_METHOD_TYPE_FINI);
    output_cdata(ifop, fini_node, GENBIND_NODE_TYPE_CDATA);
    outputf(ifop, "\t\tfree(priv);\n\t}\n}\n\n");

    /* Class Def */
    outputf(ifop, "static JSClassDef %s_%s_class_def = {\n\t\"%s\",\n\t.finalizer = %s_%s_finalizer,\n};\n\n",
            QLPFX, ife->class_name, ife->name, QLPFX, ife->class_name);

    /* Methods */
    for (int i = 0; i < ife->u.interface.operationc; i++) {
        qjs_output_method(ifop, ife, ife->u.interface.operationv + i);
    }

    /* Attributes */
    for (int i = 0; i < ife->u.interface.attributec; i++) {
        qjs_output_attribute(ifop, ife, ife->u.interface.attributev + i);
    }

    /* Prototype Init */
    outputf(ifop, "static const JSCFunctionListEntry %s_%s_proto_funcs[] = {\n", QLPFX, ife->class_name);
    for (int i = 0; i < ife->u.interface.operationc; i++) {
        struct ir_operation_entry *ope = ife->u.interface.operationv + i;
        if (ope->name) {
            outputf(ifop, "\tJS_CFUNC_DEF(\"%s\", %d, %s_%s_%s),\n", ope->name, ope->overloadv->argumentc, QLPFX, ife->class_name, ope->name);
        }
    }
    for (int i = 0; i < ife->u.interface.attributec; i++) {
        struct ir_attribute_entry *ate = ife->u.interface.attributev + i;
        bool readonly = (ate->modifier & WEBIDL_TYPE_MODIFIER_READONLY) && !ate->putforwards;
        char setter_name[256];
        if (!readonly) {
            snprintf(setter_name, sizeof(setter_name), "%s_%s_%s_set", QLPFX, ife->class_name, ate->name);
        } else {
            strcpy(setter_name, "NULL");
        }
        outputf(ifop, "\tJS_CGETSET_DEF(\"%s\", %s_%s_%s_get, %s),\n",
                ate->name, QLPFX, ife->class_name, ate->name, setter_name);
    }
    outputf(ifop, "};\n\n");

    outputf(ifop, "void %s_init_%s(JSContext *ctx) {\n", QLPFX, ife->class_name);
    outputf(ifop, "\tJSRuntime *rt = JS_GetRuntime(ctx);\n");
    outputf(ifop, "\tif (%s_%s_class_id == 0) JS_NewClassID(&%s_%s_class_id);\n", QLPFX, ife->class_name, QLPFX, ife->class_name);
    outputf(ifop, "\tJS_NewClass(rt, %s_%s_class_id, &%s_%s_class_def);\n", QLPFX, ife->class_name, QLPFX, ife->class_name);
    outputf(ifop, "\tJSValue proto = JS_NewObject(ctx);\n");
    outputf(ifop, "\tJS_SetPropertyFunctionList(ctx, proto, %s_%s_proto_funcs, sizeof(%s_%s_proto_funcs)/sizeof(%s_%s_proto_funcs[0]));\n",
            QLPFX, ife->class_name, QLPFX, ife->class_name, QLPFX, ife->class_name);
    outputf(ifop, "\tJS_SetClassProto(ctx, %s_%s_class_id, proto);\n", QLPFX, ife->class_name);
    outputf(ifop, "}\n\n");

    output_close(ifop);
    return 0;
}

static int qjs_output_binding_header(struct ir *ir)
{
    struct opctx *bindc = qjs_open_header(ir, "binding");
    if (!bindc) return -1;
    outputf(bindc, "#include <quickjs.h>\n#include \"netsurf/error.h\"\n\n");
    outputf(bindc, "nserror %s_init_bindings(JSContext *ctx);\n", QLPFX);
    qjs_close_header(ir, bindc);
    return 0;
}

static int qjs_output_binding_src(struct ir *ir)
{
    struct opctx *bindc;
    if (output_open("binding.c", &bindc) != 0) return -1;

    output_tool_preface(bindc);
    output_method_cdata(bindc, ir->binding_node, GENBIND_METHOD_TYPE_PREFACE);
    output_tool_prologue(bindc);
    output_method_cdata(bindc, ir->binding_node, GENBIND_METHOD_TYPE_PROLOGUE);

    outputf(bindc, "#include \"binding.h\"\n#include \"prototype.h\"\n#include \"private.h\"\n#include \"qjs_internal.h\"\n\n");

    outputf(bindc, "nserror %s_init_bindings(JSContext *ctx) {\n", QLPFX);
    for (int i = 0; i < ir->entryc; i++) {
        struct ir_entry *ife = ir->entries + i;
        if (ife->type == IR_ENTRY_TYPE_INTERFACE && !ife->u.interface.noobject) {
            outputf(bindc, "\t%s_init_%s(ctx);\n", QLPFX, ife->class_name);
        }
    }
    outputf(bindc, "\treturn NSERROR_OK;\n}\n");

    output_close(bindc);
    return 0;
}

static int qjs_output_makefile(struct ir *ir)
{
    FILE *makef = genb_fopen_tmp("Makefile");
    if (!makef) return -1;

    fprintf(makef, "# QJS libdom makefile fragment\n\n");
    fprintf(makef, "NSGENBIND_SOURCES := binding.c ");
    for (int i = 0; i < ir->entryc; i++) {
        struct ir_entry *ife = ir->entries + i;
        if (ife->type == IR_ENTRY_TYPE_INTERFACE && !ife->u.interface.noobject) {
            fprintf(makef, "%s ", ife->filename);
        }
    }
    fprintf(makef, "\nNSGENBIND_PREFIX := %s\n", options->outdirname);
    genb_fclose_tmp(makef, "Makefile");
    return 0;
}

int qjs_libdom_output(struct ir *ir)
{
    for (int i = 0; i < ir->entryc; i++) {
        struct ir_entry *ife = ir->entries + i;
        ife->class_name = gen_idl2c_name(ife->name);
        if (ife->class_name) {
            ife->filename = malloc(strlen(ife->class_name) + 4);
            sprintf(ife->filename, "%s.c", ife->class_name);
        }
    }

    for (int i = 0; i < ir->entryc; i++) {
        struct ir_entry *ife = ir->entries + i;
        if (ife->type == IR_ENTRY_TYPE_INTERFACE && !ife->u.interface.noobject) {
            qjs_output_interface(ir, ife);
        }
    }
    qjs_output_private_header(ir);
    qjs_output_prototype_header(ir);
    qjs_output_binding_header(ir);
    qjs_output_binding_src(ir);
    qjs_output_makefile(ir);
    return 0;
}
