/*
    Made by Noah Van Miert
    6/12/2022

    Apollo Compiler
*/

#ifndef __CORE_H_
#define __CORE_H_

#include <assert.h>
#include <stddef.h>

#define todo() assert(0 && "not implemented")

void *xmalloc(size_t size);
void *xcalloc(size_t nmemb, size_t size);
void *xrealloc(void *_ptr, size_t size);

#endif