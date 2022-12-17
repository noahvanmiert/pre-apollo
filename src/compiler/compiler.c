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

const char *nasm_setup_code_data = "segment .data\n";


char *text_segment = "";
char *data_segment = "";


void nasm_init()
{
    /* allocate a extra byte '+ 1' for the '\0' character */
    text_segment = xcalloc(strlen(nasm_setup_code) + 1, sizeof(char));
    strncpy(text_segment, nasm_setup_code, strlen(nasm_setup_code));

    /* allocate a extra byte '+ 1' for the '\0' character */
    data_segment = xcalloc(strlen(nasm_setup_code_data) + 1, sizeof(char));
    strncpy(data_segment, nasm_setup_code_data, strlen(nasm_setup_code_data));
}


void text_segment_add(const char *str)
{
    size_t size = strlen(str);

    /* allocate a extra byte '+ 1' for the '\0' character */
    text_segment = xrealloc(text_segment, (strlen(text_segment) + size + 1) * sizeof(char));
    strcat(text_segment, str);

    text_segment[strlen(text_segment)] = '\0';  
}


void data_segment_add(const char *str)
{
    size_t size = strlen(str);

    /* allocate a extra byte '+ 1' for the '\0' character */
    data_segment = xrealloc(data_segment, (strlen(data_segment) + size + 1) * sizeof(char));

    strcat(data_segment, str);
    data_segment[strlen(data_segment)] = '\0';  
}


const char *nasm_compile(struct Ast *node)
{
    nasm_init();
    nasm_compile_statements(node);

    char *_asm = xcalloc(strlen(text_segment), strlen(data_segment));

    strcat(_asm, text_segment);
    strcat(_asm, data_segment);


    return _asm;
}


void nasm_compile_statements(struct Ast *node)
{
    switch (node->type)
    {
        case AST_COMPOUND:      nasm_compile_compound(node); break;
        case AST_FUNCTION_DEF:  nasm_compile_fn_def(node); break;
        case AST_FUNCTION_CALL: nasm_compile_fn_call(node); break;
        case AST_VARIABLE_DEF:  nasm_compile_var_def(node); break;
        case AST_VARIABLE:      nasm_compile_var_def(node); break;
        case AST_STRING:        nasm_compile_string(node); break;
        case AST_INT:           nasm_compile_int(node); break;
		case AST_BOOL:			nasm_compile_bool(node); break;
		
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
    /* allocate 4 extra bytes '+ 4' for '%s:\n' and the '\0' character */
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
    if (node->fn_call_syscall == SYSCALL_NONE) {
        /* allocate a 8 extra bytes '+ 8' for '\tcall %s\n' and the '\0' character */
        char *template = xcalloc(strlen(node->fn_call_name) + 8, sizeof(char));
        sprintf(template, "\tcall %s\n", node->fn_call_name);

        text_segment_add(template);
        free(template);

        return;
    }

    nasm_compile_fn_call_syscall(node);
}


void nasm_compile_var_def(struct Ast *node)
{ 
    if (node->var_def_value->type == AST_INT) {
        const char *template = "\tmov DWORD [rbp-%d], %d\n";

        char *t = xcalloc(strlen(template) + 8 + 8 + 1, sizeof(char));
        sprintf(t, template, node->var_offset, node->var_def_value->int_value);
        text_segment_add(t);

        /* free t, else we get a memory leak */
        free(t);
    }  else if (node->var_def_value->type == AST_STRING) {
        const char *template = "\tmov DWORD [rbp-%d], str_%d\n";

        nasm_compile_statements(node->var_def_value);

        char *t = xcalloc(strlen(template) + 8 + 8 + 1, sizeof(char));
        sprintf(t, template, node->var_offset, node->var_def_value->string_addr);
        text_segment_add(t);
        
        /* free t, else we get a memory leak */
        free(t);
    } else if (node->var_def_value->type == AST_BOOL) {
		const char *template = "\tmov BYTE [rbp-%d], %d\n";

		char *t = xcalloc(strlen(template) + 8 + 8 + 1, sizeof(char));
        sprintf(t, template, node->var_offset, node->var_def_value->bool_value);
        text_segment_add(t);
		
		/* free t, else we get a memory leak */
		free(t);
	}
}


void nasm_compile_var(struct Ast *node)
{

}


void nasm_compile_string(struct Ast *node)
{
    const char *template = "str_%d: db \"%s\"\n";

    /* TODO: implement escape characters '\n', '\r', '\t' */
    
    char *t = xcalloc(strlen(template) + 8 + strlen(node->string_value) + 1, sizeof(char));
    sprintf(t, template, node->string_addr, node->string_value);

    data_segment_add(t);
    free(t);

    return;
}

void nasm_compile_int(struct Ast *node) {}
void nasm_compile_bool(struct Ast *node) {}

void nasm_compile_fn_call_syscall(struct Ast *node)
{
    switch (node->fn_call_syscall) {
        case SYSCALL_WRITE: nasm_syscall_write(node); break;
        case SYSCALL_EXIT:  nasm_syscall_exit(node);   break;
    }
}


void nasm_syscall_write(struct Ast *node)
{
    const char *template = "\tmov rdi, %d\n"        // File descriptor (0: stdin, 1: stdout, 2: stderr)
                           "\tmov rsi, str_%d\n"    // String Pointer
                           "\tmov rdx, %d\n"        // String Length
                           "\tmov rax, 1\n"         // Write syscall code
                           "\tsyscall\n";           // Call syscall write

    /* 
        The extra bytes (1 + 8 + 8 + 1) are for the file discriptor number, the string pointer, the string length
        and the '\0' character.
     */
    char *t = xcalloc(strlen(template) + 1 + 8 + 8 + 1, sizeof(char));
    nasm_compile_statements(node->fn_call_args[1]);

    sprintf(t, template, node->fn_call_args[0]->int_value, node->fn_call_args[1]->string_addr, node->fn_call_args[2]->int_value);
    text_segment_add(t);
    free(t);
}


void nasm_syscall_exit(struct Ast *node)
{
    const char *template = "\tmov rdi, %d\n"    // Exit code
                           "\tmov rax, 60"      // Exit syscall code
                           "\tsyscall\n";       // Call syscall exit

    /* the extra bytes (8 + 1) are for the exit code number and the '\0' character */
    char *t = xcalloc(strlen(template) + 8 + 1, sizeof(char));
    sprintf(t, template, node->fn_call_args[0]->int_value);
    text_segment_add(t);
    free(t);
}
