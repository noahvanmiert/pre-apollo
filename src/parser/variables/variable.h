/*
    Made by Noah Van Miert
    11/12/2022

    Apollo Compiler
*/

#ifndef __VARIABLES_H_
#define __VARIABLES_H_


#define TYPE_INT_SIZE      4
#define TYPE_BOOL_SIZE     1

/* 
   TYPE_STRING is actualy just a const char *, so it has a size of 8
   because all pointer on a 64-bit machine are 8 bytes
*/
#define TYPE_STRING_SIZE   8


enum VariableType {
    TYPE_UNKOWN,
	TYPE_BOOL,
    TYPE_INT,
    TYPE_STRING
};


enum VariableType get_var_type_from_str(const char *t);
const char *get_str_from_var_type(enum VariableType t);


#endif // __VARIABLES_H_
