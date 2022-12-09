/*
    Made by Noah Van Miert
    9/12/2022

    Apollo Compiler
*/

#ifndef __SCOPE_H
#define __SCOPE_H_

#include "../core.h"
#include "../parser/ast/ast.h"


struct Scope {
    struct Ast **variable_defs;
    struct Ast **function_defs;

    size_t variable_defs_size;
    size_t function_defs_size;
};


struct Scope *create_scope();
struct Scope *copy_scope(struct Scope *s);

void scope_add_variable(struct Scope *scope, struct Ast *var);
void scope_add_function(struct Scope *scope, struct Ast *fn);

struct Ast *scope_get_variable(struct Scope *scope, const char *name);
struct Ast *scope_get_function(struct Scope *scope, const char *name);

#endif