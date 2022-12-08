/*
    Made by Noah Van Miert
    8/12/2022

    Apollo Compiler
*/

#include "compiler.h"

#include "../core.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


const char *nasm_setup_code = "segment .text\n"
                              "global _start:\n"
                              "_start:\n"
                              "\tcall main\n"
                              "\tmov rax, 60\n"
                              "\tmov rdi, 0\n"
                              "\tsyscall\n\n";


char *text_segment = NULL;


void nasm_init()
{
    text_segment = xcalloc(strlen(nasm_setup_code) + 1, sizeof(char));
    strncpy(text_segment, nasm_setup_code, strlen(nasm_setup_code));
}


void text_segment_add(const char *str)
{
    size_t size = strlen(str);

    text_segment = xrealloc(text_segment, (strlen(text_segment) + size + 1) * sizeof(char));

    strcat(text_segment, str);
    text_segment[strlen(text_segment) + size] = '\0';  
}


const char *nasm_compile(struct Ast *node)
{
    nasm_init();
    nasm_compile_statements(node);
    return text_segment;
}


void nasm_compile_statements(struct Ast *node)
{
    switch (node->type)
    {
        case AST_COMPOUND: nasm_compile_compound(node); break;
        case AST_FUNCTION_DEF: nasm_compile_fn_def(node); break;
        case AST_FUNCTION_CALL: nasm_compile_fn_call(node); break;
        case AST_NOP: break;

        default: assert(0);
    }
}


void nasm_compile_compound(struct Ast *node)
{
    for (size_t i = 0; i < node->compound_size; i++) {
        nasm_compile_statements(node->compound_value[i]);
    }
}


void nasm_compile_fn_def(struct Ast *node) 
{
    char *template = xcalloc(strlen(node->fn_name) + 4, sizeof(char));
    sprintf(template, "%s:\n", node->fn_name);

    char *stack_frame = "\tpush rbp\n"
                        "\tmov rbp, rsp\n";

    text_segment_add(template);
    text_segment_add(stack_frame);
    free(template);

    nasm_compile_statements(node->fn_body);

    text_segment_add("\tpop rbp\n\tret\n\n");
}


void nasm_compile_fn_call(struct Ast *node)
{
    char *template = xcalloc(strlen(node->fn_call_name) + 8, sizeof(char));
    sprintf(template, "\tcall %s\n", node->fn_call_name);

    text_segment_add(template);
    free(template);
}