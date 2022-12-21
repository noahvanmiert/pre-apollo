/*
    Made by Noah Van Miert
    9/12/2022

    Apollo Compiler
*/

#include "scope.h"

#include <string.h>


struct Scope *create_scope()
{
    struct Scope *scope = xmalloc(sizeof(struct Scope));

    scope->variable_defs = NULL;
    scope->function_defs = NULL;

    scope->variable_defs_size = 0;
    scope->function_defs_size = 0;

    return scope;
}


void scope_add_variable(struct Scope *scope, struct Ast *var)
{ 
    scope->variable_defs_size += 1;

    if (scope->variable_defs == NULL) {
        scope->variable_defs = xmalloc(sizeof(struct Ast *));
    } else {
        scope->variable_defs = xrealloc(
            scope->variable_defs,
            sizeof(struct Ast *) * scope->variable_defs_size
        );
    }

    scope->variable_defs[scope->variable_defs_size - 1] = var;
}


void scope_add_function(struct Scope *scope, struct Ast *fn)
{
    scope->function_defs_size += 1;

    if (scope->function_defs == NULL) {
        scope->function_defs = xmalloc(sizeof(struct Ast *));
    } else {
        scope->function_defs = xrealloc(
            scope->function_defs,
            sizeof(struct Ast *) * scope->function_defs_size
        );
    }

    scope->function_defs[scope->function_defs_size - 1] = fn;
}


struct Scope *copy_scope(struct Scope *s)
{
    /*
        This function will return a copy
        of the given scope s.
    */
   
    struct Scope *scope = create_scope();

    for (size_t i = 0; i < s->function_defs_size; i++)
        scope_add_function(scope, s->function_defs[i]);

    for (size_t i = 0; i < s->variable_defs_size; i++)
        scope_add_variable(scope, s->variable_defs[i]);

    scope->function_defs_size = s->function_defs_size;
    scope->variable_defs_size = s->variable_defs_size;

    return scope;
}


struct Ast *scope_get_variable(struct Scope *scope, const char *name)
{
    for (size_t i = 0; i < scope->variable_defs_size; i++) {
        if (strcmp(scope->variable_defs[i]->var_def_name, name) == 0)
            return scope->variable_defs[i];
    }

    return NULL;
}


struct Ast *scope_get_function(struct Scope *scope, const char *name)
{
    for (size_t i = 0; i < scope->function_defs_size; i++) {
        if (strcmp(scope->function_defs[i]->fn_name, name) == 0)
            return scope->function_defs[i];
    }

    return NULL;
}