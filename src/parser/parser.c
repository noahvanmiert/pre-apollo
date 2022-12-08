/*
    Made by Noah Van Miert
    7/12/2022

    Apollo Compiler
*/

#include "parser.h"

#include "../core.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct Parser *create_parser(struct Lexer *lexer)
{
    struct Parser *parser = malloc(sizeof(struct Parser));
    check_memory(parser);

    parser->lexer = lexer;
    parser->current = lexer_get_token(lexer);
    parser->prev = parser->current;

    return parser;
}


static void consume(struct Parser *parser, enum TokenType type)
{
    if (parser->current->type != type) {
        fprintf(stderr, "ERROR: unexpected token '%s'\n", parser->current->value);
        exit(EXIT_FAILURE);
    }

    parser->prev = parser->current;
    parser->current = lexer_get_token(parser->lexer);
}


struct Ast *parser_parse(struct Parser *parser)
{
    return parser_parse_statements(parser);
}


struct Ast *parser_parse_statements(struct Parser *parser)
{
    struct Ast *compound = create_ast(AST_COMPOUND);
    struct Ast *statement = parser_parse_statement(parser);
    
    compound_add(compound, statement);

    while (parser->current->type == TOKEN_SEMICOLON) {
        consume(parser, TOKEN_SEMICOLON);

        statement = parser_parse_statement(parser);
        compound_add(compound, statement);
    }

    return compound;
}


struct Ast *parser_parse_statement(struct Parser *parser)
{
    switch (parser->current->type)
    {
        case TOKEN_WORD: return parser_parse_word(parser);
        case TOKEN_STRING: return parser_parse_string(parser);
        case TOKEN_RCURL: return create_ast(AST_NOP);
        case TOKEN_END: return create_ast(AST_NOP);

        default: assert(0);
    }
}


struct Ast *parser_parse_expression(struct Parser *parser)
{
    switch (parser->current->type)
    {
        case TOKEN_STRING: return parser_parse_string(parser);

        default: {
            printf("type: '%d'\n", parser->current->type);
            assert(0);
        }
    }
}


static void check_end_block(struct Parser *parser)
{
    if (parser->current->type != TOKEN_RCURL) {
        fprintf(stderr, "ERROR: unexpected token '%s'\n", parser->current->value);
        exit(1);
    }

    parser->current->type = TOKEN_SEMICOLON;
}


struct Ast *parser_parse_fn_def(struct Parser *parser)
{
    struct Ast *fn_def = create_ast(AST_FUNCTION_DEF);

    // 'fun'
    consume(parser, TOKEN_WORD);

    fn_def->fn_name = parser->current->value;

    // name
    consume(parser, TOKEN_WORD);

    consume(parser, TOKEN_LPAREN);
    consume(parser, TOKEN_RPAREN);

    consume(parser, TOKEN_LCURL);

    fn_def->fn_body = parser_parse_statements(parser);

    check_end_block(parser);

    return fn_def;
}


struct Ast *parser_parse_fn_call(struct Parser *parser)
{
    struct Ast *fn_call = create_ast(AST_FUNCTION_CALL);

    fn_call->fn_call_name = parser->current->value;

    consume(parser, TOKEN_WORD);
    consume(parser, TOKEN_LPAREN);
    consume(parser, TOKEN_RPAREN);

    return fn_call;
}


struct Ast *parser_parse_word(struct Parser *parser)
{
    if (strcmp(parser->current->value, "fun") == 0)
        return parser_parse_fn_def(parser);

    return parser_parse_fn_call(parser);
}


struct Ast *parser_parse_string(struct Parser *parser)
{
    struct Ast *string = create_ast(AST_STRING);
    
    string->string_value = parser->current->value;

    return string;
}