/*
    Made by Noah Van Miert
    6/12/2022

    Apollo Compiler
*/

#ifndef __CORE_H_
#define __CORE_H_

#include <assert.h>

#define true 1
#define false 0

#define check_memory(ptr) assert(ptr && "apollo: could not allocate enough memory")

#endif