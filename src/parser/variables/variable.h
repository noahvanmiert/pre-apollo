/*
    Made by Noah Van Miert
    11/12/2022

    Apollo Compiler
*/

#ifndef __VARIABLES_H_
#define __VARIABLES_H_


#define TYPE_INT_SIZE      4

/* 
   TYPE_STRING is actualy just a const char *, so it has a size of 8
   because all pointer on a 64-bit machine or 8 bytes
*/
#define TYPE_STRING_SIZE   8


enum VariableType {
    TYPE_UNKOWN,
    TYPE_INT,
    TYPE_STRING
};


enum VariableType get_var_type_from_str(const char *t);


#endif // __VARIABLES_H_