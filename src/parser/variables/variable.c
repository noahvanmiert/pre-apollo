/*
    Made by Noah Van Miert
    11/12/2022

    Apollo Compiler
*/

#include "variable.h"

#include <string.h>

enum VariableType get_var_type_from_str(const char *t)
{
	if (strcmp(t, "bool") == 0)   return TYPE_BOOL;
    if (strcmp(t, "int") == 0)    return TYPE_INT;
    if (strcmp(t, "string") == 0) return TYPE_STRING;

    return TYPE_UNKOWN;
}  


const char *get_str_from_var_type(enum VariableType t)
{
    switch (t) {
        case TYPE_BOOL:   return "bool";
        case TYPE_INT:    return "int";
        case TYPE_STRING: return "string";

        default:          return TYPE_UNKOWN;
    }
}