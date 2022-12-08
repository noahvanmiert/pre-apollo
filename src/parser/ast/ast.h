/*
    Made by Noah Van Miert
    7/12/2022

    Apollo Compiler
*/

#ifndef __AST_H_
#define __AST_H_

#include <stddef.h>

enum AstType {
    AST_STRING,
    AST_FUNCTION_DEF,
    AST_FUNCTION_CALL,
    AST_COMPOUND,
    AST_NOP
};


struct Ast {
    enum AstType type;

    /* AST_STRING */
    const char *string_value;

    /* AST_FUNCTION_DEF */
    const char *fn_name;
    struct Ast *fn_body;

    /* AST_FUNCTION_CALL */
    const char *fn_call_name;

    /* AST_COMPOUND */
    struct Ast **compound_value;
    size_t compound_size;
};


struct Ast *create_ast(enum AstType type);
void compound_add(struct Ast *ast, struct Ast *item);

#endif