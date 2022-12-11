/*
    Made by Noah Van Miert
    8/12/2022

    Apollo Compiler
*/

#include "logging.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


void apo_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    printf("\033[1;31m");
    vprintf(fmt, args);
    printf("\033[0m\n");

    exit(EXIT_FAILURE);
}


void apo_warning(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    printf("\033[1;33m");
    vprintf(fmt, args);
    printf("\033[0m\n");

    va_end(args);
}


void apo_hint(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    printf("\033[1;32m");
    vprintf(fmt, args);
    printf("\033[0m\n");

    va_end(args);
}


void apo_compiler_error(const char *filepath,
                        size_t line,
                        size_t col,
                        const char *fmt,
                        ...)
{
    va_list args;
    va_start(args, fmt);

    printf("\033[1;31m%s (%ld, %ld): ", filepath, line, col);
    vprintf(fmt, args);
    printf("\033[0m\n");

    exit(EXIT_FAILURE);
}