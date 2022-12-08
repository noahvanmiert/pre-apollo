/*
    Made by Noah Van Miert
    8/12/2022

    Apollo Compiler
*/

#ifndef __LOGGING_H_
#define __LOGGING_H_

void apo_error(char *fmt, ...);
void apo_warning(char *fmt, ...);
void apo_hint(char *fmt, ...);

#endif