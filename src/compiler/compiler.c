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


/*
    This is the nasm setup code,
    here we call main and after main
    is finished running we exit
    with a return code of 0.
    which means the program ran
    succesfully.
*/
static const char *nasm_setup_code = "segment .text\n"
                                     "global _start:\n"
                                     "_start:\n"
                                     "\tcall main\n"
                                     "\tmov rax, 60\n"
                                     "\tmov rdi, 0\n"
                                     "\tsyscall\n\n";

static const char *nasm_setup_code_data = "segment .data\n";


static char *text_segment = "";
static char *data_segment = "";


void nasm_init()
{
    /* 
        Copy the setup nasm_setup_code_text
        into the text segment.
    */
    text_segment = xcalloc(strlen(nasm_setup_code) + 1, sizeof(char));
    strncpy(text_segment, nasm_setup_code, strlen(nasm_setup_code));

    /* 
        Copy the setup nasm_setup_code_data
        into the data segment.
    */
    data_segment = xcalloc(strlen(nasm_setup_code_data) + 1, sizeof(char));
    strncpy(data_segment, nasm_setup_code_data, strlen(nasm_setup_code_data));
}


void text_segment_add(const char *str)
{
    size_t size = strlen(str);

    /* allocate an extra byte '+ 1' for the '\0' character */
    text_segment = xrealloc(text_segment, (strlen(text_segment) + size + 1) * sizeof(char));
    strcat(text_segment, str);

    text_segment[strlen(text_segment)] = '\0';  
}


void data_segment_add(const char *str)
{
    size_t size = strlen(str);

    /* allocate an extra byte '+ 1' for the '\0' character */
    data_segment = xrealloc(data_segment, (strlen(data_segment) + size + 1) * sizeof(char));

    strcat(data_segment, str);
    data_segment[strlen(data_segment)] = '\0';  
}


const char *nasm_compile(struct Ast *node)
{
    nasm_init();
    nasm_compile_statements(node);

    /*
        Allocate enough space to concatenate
        all the segments into one string,
        the extra one byte is for the '\0'
        character.
    */
    char *_asm = xcalloc(strlen(text_segment) + strlen(data_segment) + 1, sizeof(char));

    /*
        Merge all the assembly segments
        into one string so we can write
        that whole string into the output
        assembly file.
    */
    strcat(_asm, text_segment);
    strcat(_asm, data_segment);

    /*
        Clean the memory for the segments.
    */
    free(text_segment);
    free(data_segment);

    return (const char *) _asm;
}


void nasm_compile_statements(struct Ast *node)
{
    switch (node->type)
    {
        case AST_COMPOUND:          nasm_compile_compound(node); break;
        case AST_FUNCTION_DEF:      nasm_compile_fn_def(node); break;
        case AST_FUNCTION_CALL:     nasm_compile_fn_call(node); break;
        case AST_VARIABLE_DEF:      nasm_compile_var_def(node); break;
        case AST_VARIABLE:          nasm_compile_var_def(node); break;
        case AST_VARIABLE_REDEF:    nasm_compile_var_redef(node); break;
        case AST_STRING:            nasm_compile_string(node); break;
        case AST_INT:               nasm_compile_int(node); break;
		case AST_BOOL:			    nasm_compile_bool(node); break;
		
        case AST_NOP: break;

        default: assert(0);
    }
}


void nasm_compile_compound(struct Ast *node)
{
    /*
        Go trough all the statements that
        are in the node's compound list.
    */
    for (size_t i = 0; i < node->compound_size; i++)
        nasm_compile_statements(node->compound_value[i]);
}


void nasm_compile_fn_def(struct Ast *node) 
{
    /* allocate 4 extra bytes '+ 4' for '%s:\n' and the '\0' character */
    char *template = xcalloc(strlen(node->fn_name) + 4, sizeof(char));
    sprintf(template, "%s:\n", node->fn_name);

    /*
        The stackframe is placed in every 
        function because this will prevent
        us from writing data on the stack
        were other data is already defined.
    */
    char *stack_frame = "\tpush rbp\n"
                        "\tmov rbp, rsp\n";

    text_segment_add(template);
    text_segment_add(stack_frame);
    free(template);

    nasm_compile_statements(node->fn_body);

    /*
        This is the end of the stackframe
        here we will restore the old stackframe,
        all the stack allocated objects 
        will be removed.
    */
    text_segment_add("\tpop rbp\n"
                     "\tret\n\n");
}


void nasm_compile_fn_call(struct Ast *node)
{
    if (node->fn_call_syscall == SYSCALL_NONE) {
        /* allocate 8 extra bytes '+ 8' for '\tcall %s\n' and the '\0' character */
        char *template = xcalloc(strlen(node->fn_call_name) + 8, sizeof(char));
        sprintf(template, "\tcall %s\n", node->fn_call_name);

        text_segment_add(template);
        free(template);

        return;
    }

    /*
        If the function we are calling
        is a syscall we need to write
        some different code.
    */
    nasm_compile_fn_call_syscall(node);
}


void nasm_compile_var_def_from_var(struct Ast *node)
{
    /*
        This function is called when we want
        to define a variable an put the value
        of another variable in it.
    */
    
    const char *template = "\tmov rax, [rbp-%d]\n"
                           "\tmov [rbp-%d], rax\n";

    char *t = xcalloc(strlen(template) + 8 + 8 + 1, sizeof(char));
    sprintf(t, template, node->var_def_value->var_offset, node->var_offset);
    text_segment_add(t);    

    free(t);
}


void nasm_compile_var_def(struct Ast *node)
{ 
    /*
        If we want to define a variable
        with the value of another variable.
    */
    if (node->var_def_value->type == AST_VARIABLE)
        return nasm_compile_var_def_from_var(node);


    /*
        If the variable if of type int.
    */
    if (node->var_def_value->type == AST_INT) {
        const char *template = "\tmov dword [rbp-%d], %d\n";

        char *t = xcalloc(strlen(template) + 8 + 8 + 1, sizeof(char));
        sprintf(t, template, node->var_offset, node->var_def_value->int_value);
        text_segment_add(t);

        /* free t, else we get a memory leak */
        free(t);

        return;
    }
    
    
    /*
        If the variable is of type string.
    */
    if (node->var_def_value->type == AST_STRING) {
        const char *template = "\tmov qword [rbp-%d], str_%d\n";

        nasm_compile_statements(node->var_def_value);

        char *t = xcalloc(strlen(template) + 8 + 8 + 1, sizeof(char));
        sprintf(t, template, node->var_offset, node->var_def_value->string_addr);
        text_segment_add(t);
        
        /* free t, else we get a memory leak */
        free(t);

        return;
    }


    /*
        IIf the variable is of type bool.
    */
    if (node->var_def_value->type == AST_BOOL) {
		const char *template = "\tmov byte [rbp-%d], %d\n";

		char *t = xcalloc(strlen(template) + 8 + 8 + 1, sizeof(char));
        sprintf(t, template, node->var_offset, node->var_def_value->bool_value);
        text_segment_add(t);
		
		/* free t, else we get a memory leak */
		free(t);
	}
}


/*
    This function doesn't need to do anything
    for now.
*/
void nasm_compile_var(struct Ast *node) {}


void nasm_comile_var_redef_from_var(struct Ast *node)
{
    /*
        This function is called when we wan't
        to define a variable an put the value
        of another variable in it.
    */
    
    const char *template = "\tmov rax, [rbp-%d]\n"
                          "\tmov [rbp-%d], rax\n";

    char *t = xcalloc(strlen(template) + 8 + 8 + 1, sizeof(char));
    sprintf(t, template, node->var_redef_offset, node->var_redef_value->var_offset);
    text_segment_add(t);

    free(t);
}


/*
    This function looks almost exactly
    the same as nasm_compile_var_def()
    the only difference is we get the
    information from other variables
    int the ast.
*/
void nasm_compile_var_redef(struct Ast *node)
{
    /*
        If we're redefining a variable
        with the value of another variable.
    */
    if (node->var_redef_value->type == AST_VARIABLE)
        nasm_comile_var_redef_from_var(node);
    

    /*
        If the new value is of type int.
    */
    if (node->var_redef_value->type == AST_INT) {
        const char *template = "\tmov dword [rbp-%d], %d\n";

        char *t = xcalloc(strlen(template) + 8 + 8 + 1, sizeof(char));
        sprintf(t, template, node->var_redef_offset, node->var_redef_value->int_value);
        text_segment_add(t);

        /* free t, else we get a memory leak */
        free(t);

        return;
    }


    /*
        If the new value is of type string.
    */
    if (node->var_redef_value->type == AST_STRING) {
        const char *template = "\tmov qword [rbp-%d], str_%d\n";

        nasm_compile_statements(node->var_def_value);

        char *t = xcalloc(strlen(template) + 8 + 8 + 1, sizeof(char));
        sprintf(t, template, node->var_redef_offset, node->var_redef_value->string_addr);
        text_segment_add(t);
        
        /* free t, else we get a memory leak */
        free(t);

        return;
    }
    
    
    /*
        If the new value is of type bool.
    */
    if (node->var_redef_value->type == AST_BOOL) {
		const char *template = "\tmov byte [rbp-%d], %d\n";

		char *t = xcalloc(strlen(template) + 8 + 8 + 1, sizeof(char));
        sprintf(t, template, node->var_redef_offset, node->var_redef_value->bool_value);
        text_segment_add(t);
		
		/* free t, else we get a memory leak */
		free(t);
	}
}


void nasm_compile_string(struct Ast *node)
{
    const char *template = "str_%d: db \"%s\"\n";

    /* TODO: implement escape characters '\n', '\r', '\t' */
    
    /*
        We allocte 8 extra bytes for the string number.
    */
    char *t = xcalloc(strlen(template) + 8 + strlen(node->string_value) + 1, sizeof(char));
    sprintf(t, template, node->string_addr, node->string_value);

    data_segment_add(t);

    /*
        We don't need the variable t anymore
        because it is copied to the text_segment
        string, so we can just free it.
    */
    free(t);

    return;
}


void nasm_compile_int(struct Ast *node) {}
void nasm_compile_bool(struct Ast *node) {}


void nasm_compile_fn_call_syscall(struct Ast *node)
{
    /*
        We actually don't need this but
        to be extra save.
    */
    assert(node->fn_call_syscall != SYSCALL_NONE);

    /*
        This well call the correct function
        to compile the syscall.
    */
    switch (node->fn_call_syscall) {
        case SYSCALL_WRITE: nasm_syscall_write(node); break;
        case SYSCALL_EXIT:  nasm_syscall_exit(node);  break;

        default: assert(0 && "unkown syscall");
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

    /*
        We don't need the variable t anymore
        because it is copied to the text_segment
        string, so we can just free it.
    */
    free(t);
}


void nasm_syscall_exit(struct Ast *node)
{
    const char *template = "\tmov rdi, %d\n"    // Exit code
                           "\tmov rax, 60\n"    // Exit syscall code
                           "\tsyscall\n";       // Call syscall exit

    /* the extra bytes (8 + 1) are for the exit code and the '\0' character */
    char *t = xcalloc(strlen(template) + 8 + 1, sizeof(char));
    sprintf(t, template, node->fn_call_args[0]->int_value);
    text_segment_add(t);


    /*
        We don't need the variable t anymore
        because it is copied to the text_segment
        string, so we can just free it.
    */
    free(t);
}
