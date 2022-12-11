/*
    Made by Noah Van Miert
    7/12/2022

    Apollo Compiler
*/

#include "parser.h"

#include "logging/logging.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Parser *create_parser(struct Lexer *lexer)
{
    struct Parser *parser = xmalloc(sizeof(struct Parser));

    parser->lexer = lexer;
    parser->current = lexer_get_token(lexer);
    parser->prev = parser->current;

    parser->entry_point_found = false;
    parser->in_function = false;
    
    parser->string_addr = 0;
    parser->var_offset = 0;

    return parser;
}


static void parser_err(struct Parser *parser, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    printf("\033[1;31m%s (%ld, %ld): ", parser->current->filepath,  parser->current->line,  parser->current->col);
    vprintf(fmt, args);
    printf("\033[0m\n");

    exit(EXIT_FAILURE);
}


static void consume(struct Parser *parser, enum TokenType type)
{
    if (unlikely(parser->current->type != type))
        parser_err(parser, "error: unexpected token '%s'", parser->current->value);

    parser->prev = parser->current;
    parser->current = lexer_get_token(parser->lexer);
}


struct Ast *parser_parse(struct Scope *scope, struct Parser *parser)
{
    struct Ast *root = parser_parse_statements(scope, parser);

    /* 
     * if the entry point (main function) is not found,
     * which is very unlikely because everyone knows you need
     * to write a main function. The compilation should stop because
     * in the asm output file _start will call main and then we have an error.
    */
    if (unlikely(!parser->entry_point_found))
        parser_err(parser, "error: entry point not found (aka 'main' function)");

    // free scope, we don't need it anymore after
    // the code is parsed.
    free(scope);
    return root;
}


struct Ast *parser_parse_statements(struct Scope *scope, struct Parser *parser)
{
    struct Ast *compound = create_ast(AST_COMPOUND);
    struct Ast *statement = parser_parse_statement(scope, parser);
    
    ast_compound_add(compound, statement);

    while (parser->current->type == TOKEN_SEMICOLON) {
        consume(parser, TOKEN_SEMICOLON);

        statement = parser_parse_statement(scope, parser);
        ast_compound_add(compound, statement);
    }

    return compound;
}


struct Ast *parser_parse_statement(struct Scope *scope, struct Parser *parser)
{
    switch (parser->current->type)
    {
        case TOKEN_WORD:   return parser_parse_word(scope, parser);
        case TOKEN_STRING: return parser_parse_string(scope, parser);
        case TOKEN_RCURL:  return create_ast(AST_NOP);
        case TOKEN_END:    return create_ast(AST_NOP);

        default: assert(0);
    }
}


struct Ast *parser_parse_expression(struct Scope *scope, struct Parser *parser)
{
    switch (parser->current->type)
    {
        case TOKEN_STRING: return parser_parse_string(scope, parser);
        case TOKEN_INT:    return parser_parse_int(scope, parser);

        default: assert(0);
    }
}


static void check_end_block(struct Parser *parser)
{
    if (unlikely(parser->current->type != TOKEN_RCURL))
        parser_err(parser, "error: unexpected token '%s'", parser->current->value);

    parser->current->type = TOKEN_SEMICOLON;
}


struct Ast *parser_parse_fn_def(struct Scope *scope, struct Parser *parser)
{
    struct Ast *fn_def = create_ast(AST_FUNCTION_DEF);

    // 'fun'
    consume(parser, TOKEN_WORD);

    if (parser->in_function)
        parser_err(parser, "error: cannot define function inside of another function");

    fn_def->fn_name = parser->current->value;

    // name
    consume(parser, TOKEN_WORD);

    consume(parser, TOKEN_LPAREN);
    consume(parser, TOKEN_RPAREN);

    consume(parser, TOKEN_LCURL);

    struct Scope *s = copy_scope(scope);
    parser->in_function = true;
    fn_def->fn_body = parser_parse_statements(s, parser);
    parser->in_function = false;
    free(s);

    check_end_block(parser);

    if (strcmp(fn_def->fn_name, "main") == 0) {
        if (!parser->entry_point_found)
            parser->entry_point_found = true;
        else
            parser_err(parser, "error: entry point already defined");
    }

    if (scope_get_function(scope, fn_def->fn_name))
        parser_err(parser, "error: function '%s' is already defined", fn_def->fn_name);

    scope_add_function(scope, fn_def);

    return fn_def;
}


static void check_is_syscall(struct Ast *fn_call)
{
    if (strcmp(fn_call->fn_call_name, "__sys_write") == 0)
        fn_call->fn_call_syscall = SYSCALL_WRITE;
    
    if (strcmp(fn_call->fn_call_name, "__sys_exit") == 0)
        fn_call->fn_call_syscall = SYSCALL_EXIT;
}


struct Ast *parser_parse_fn_call(struct Scope *scope, struct Parser *parser)
{
    struct Ast *fn_call = create_ast(AST_FUNCTION_CALL);

    fn_call->fn_call_name = parser->current->value;

    struct Location loc = (struct Location) {
        .filepath = parser->current->filepath,
        .line = parser->current->line,
        .col = parser->current->col
    };

    check_is_syscall(fn_call);

    if (fn_call->fn_call_syscall == SYSCALL_NONE) {
        if (scope_get_function(scope, fn_call->fn_call_name) == NULL)
            parser_err(parser, "error: function '%s' is not defined", fn_call->fn_call_name);
    }

    consume(parser, TOKEN_WORD);
    consume(parser, TOKEN_LPAREN);

    if (parser->current->type != TOKEN_RPAREN) {
        struct Ast *arg = parser_parse_expression(scope, parser);

        fn_call->fn_call_args = xrealloc(
            fn_call->fn_call_args,
            (fn_call->fn_call_args_size + 1) * sizeof(struct Ast *)
        );

        fn_call->fn_call_args[fn_call->fn_call_args_size] = arg;
        fn_call->fn_call_args_size++;

        while (parser->current->type == TOKEN_COMMA) {
            consume(parser, TOKEN_COMMA);

            arg = parser_parse_expression(scope, parser);

            fn_call->fn_call_args = xrealloc(
                fn_call->fn_call_args,
                (fn_call->fn_call_args_size + 1) * sizeof(struct Ast *)
            );

            fn_call->fn_call_args[fn_call->fn_call_args_size] = arg;
            fn_call->fn_call_args_size++;
        }
    }

    consume(parser, TOKEN_RPAREN);

    if (fn_call->fn_call_syscall != SYSCALL_NONE)
        parser_type_check_syscall(fn_call, &loc);

    return fn_call;
}


static bool types_match(enum VariableType type, struct Ast *value)
{
    switch (type) {
        case TYPE_INT: return value->type == AST_INT;
        case TYPE_STRING: return value->type == AST_STRING;
    
        default: assert(0);
    }
}


static void add_variable_offset_to_parser(struct Parser *parser, enum VariableType type) 
{
    switch (type)
    {
    case TYPE_INT:    parser->var_offset += TYPE_INT_SIZE; break;
    case TYPE_STRING: parser->var_offset += TYPE_STRING_SIZE; break;
    
    default: assert(0);
    }
}


struct Ast *parser_parse_var_def(struct Scope *scope, struct Parser *parser)
{
    struct Ast *var = create_ast(AST_VARIABLE_DEF);
    
    // 'var'
    consume(parser, TOKEN_WORD);

    var->var_def_name = parser->current->value;

    consume(parser, TOKEN_WORD);
    consume(parser, TOKEN_COLON);

    var->var_def_type = get_var_type_from_str(parser->current->value);
    const char *type = parser->current->value;

    if (unlikely(var->var_def_type == TYPE_UNKOWN))
        parser_err(parser, "error: unkown type '%s'\n", parser->current->value);

    // the variable type
    consume(parser, TOKEN_WORD);
    consume(parser, TOKEN_EQ);

    var->var_def_value = parser_parse_expression(scope, parser);

    /*
        Now we need to check if the given type matches
        the given value.
    */
    if (unlikely(!types_match(var->var_def_type, var->var_def_value)))
        parser_err(parser, "error: given type '%s' does not match with the given value.", type);

    add_variable_offset_to_parser(parser, var->var_def_type);
    var->var_offset = parser->var_offset;

    scope_add_variable(scope, var);

    return var;
}


struct Ast *parser_parse_var(struct Scope *scope, struct Parser *parser)
{

}


struct Ast *parser_parse_word(struct Scope *scope, struct Parser *parser)
{
    if (strcmp(parser->current->value, "fun") == 0)
        return parser_parse_fn_def(scope, parser);

    else if (strcmp(parser->current->value, "let") == 0)
        return parser_parse_var_def(scope, parser);

    return parser_parse_fn_call(scope, parser);
}


struct Ast *parser_parse_string(struct Scope *scope, struct Parser *parser)
{
    struct Ast *string = create_ast(AST_STRING);
    
    string->string_value = parser->current->value;
    string->string_addr = parser->string_addr;
    
    parser->string_addr += 1;

    consume(parser, TOKEN_STRING);

    return string;
}


struct Ast *parser_parse_int(struct Scope *scope, struct Parser *parser)
{
    struct Ast *_int = create_ast(AST_INT);
    
    _int->int_value = atoi(parser->current->value);

    consume(parser, TOKEN_INT);

    return _int;
}


void parser_type_check_syscall(struct Ast *ast, struct Location *loc)
{
    assert(ast->type == AST_FUNCTION_CALL);

    switch (ast->fn_call_syscall) {
        case SYSCALL_NONE: assert(0);

        case SYSCALL_WRITE: parser_type_check_sys_write(ast, loc); break;
        case SYSCALL_EXIT:  parser_type_check_sys_exit(ast, loc);  break;
    
        default: assert(0);
    }
}


void parser_type_check_sys_write(struct Ast *ast, struct Location *loc)
{
    if (ast->fn_call_args_size < 3 || ast->fn_call_args_size > 3)
        apo_compiler_error(loc->filepath, loc->line,loc->col, "error: '__sys_write' expected 3 arguments but, %d were given", ast->fn_call_args_size);

    if (ast->fn_call_args[0]->type != AST_INT)
        apo_compiler_error(loc->filepath, loc->line,loc->col, "error: '__sys_write' expected an integer as first argument");

    if (ast->fn_call_args[1]->type != AST_STRING) 
        apo_compiler_error(loc->filepath, loc->line,loc->col, "error: '__sys_write' expected a string as second argument");

    if (ast->fn_call_args[2]->type != AST_INT)
        apo_compiler_error(loc->filepath, loc->line,loc->col, "error: '__sys_write' expected an integer as third argument");
}


void parser_type_check_sys_exit(struct Ast *ast, struct Location *loc)
{
    if (ast->fn_call_args_size < 1 || ast->fn_call_args_size > 1)
        apo_compiler_error(loc->filepath, loc->line,loc->col, "error: '__sys_exit' expected 1 argument but, %d were given", ast->fn_call_args_size);

    if (ast->fn_call_args[0]->type != AST_INT)
        apo_compiler_error(loc->filepath, loc->line,loc->col, "error: '__sys_exit' expected an integer as exit code");
}
