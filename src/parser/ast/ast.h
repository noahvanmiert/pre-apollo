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
    AST_INT,
    AST_VARIABLE_DEF,
    AST_VARIABLE,
    AST_FUNCTION_DEF,
    AST_FUNCTION_CALL,
    AST_COMPOUND,
    AST_NOP
};


enum Syscall {
    SYSCALL_NONE = -1,
    SYSCALL_WRITE = 1,
    SYSCALL_EXIT = 60
};


struct Ast {
    enum AstType type;

    /* AST_INT */
    int int_value;
    
    /* AST_STRING */
    const char *string_value;
    size_t string_addr;

    /* AST_VARIABLE_DEF */
    const char *var_def_name;
    struct Ast *var_def_value;

    /* AST_VARIABLE */
    const char *var_name;

    /* AST_FUNCTION_DEF */
    const char *fn_name;
    struct Ast *fn_body;

    /* AST_FUNCTION_CALL */
    const char *fn_call_name;
    struct Ast **fn_call_args;
    size_t fn_call_args_size;
    enum Syscall fn_call_syscall;


    /* AST_COMPOUND */
    struct Ast **compound_value;
    size_t compound_size;
};


struct Ast *create_ast(enum AstType type);
void ast_compound_add(struct Ast *ast, struct Ast *item);

#endif