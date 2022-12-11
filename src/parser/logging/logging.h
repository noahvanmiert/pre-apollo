/*
    Made by Noah Van Miert
    8/12/2022

    Apollo Compiler
*/

#ifndef __LOGGING_H_
#define __LOGGING_H_

#include <stddef.h>

void apo_compiler_error(const char *filepath,
                        size_t line,
                        size_t col,
                        const char *fmt,
                        ...);

void apo_error(const char *fmt, ...);
void apo_warning(const char *fmt, ...);
void apo_hint(const char *fmt, ...);

#endif