/*
    Made by Noah Van Miert
    11/12/2022

    Apollo Compiler
*/

#include "variable.h"

#include <string.h>

enum VariableType get_var_type_from_str(const char *t)
{
    if (strcmp(t, "int") == 0)    return TYPE_INT;
    if (strcmp(t, "string") == 0) return TYPE_STRING;

    return TYPE_UNKOWN;
}  