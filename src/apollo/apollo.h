/*
    Made by Noah Van Miert
    8/12/2022

    Apollo Compiler
*/

#ifndef __APOLLO_H_
#define __APOLLO_H_


struct ArgManager {
    const char *filepath;
    char help_f;
    char run_f;
    char asm_f;
};


struct ArgManager parse_args(int argc, char **argv);

void apollo_start(int argc, char **argv);
void run_assembler(struct ArgManager args);
void write_assembly(const char *data);

#endif