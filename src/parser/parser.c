/*
    Made by Noah Van Miert
    7/12/2022

    Apollo Compiler
*/

#include "parser.h"

#include "../core.h"
#include <stdlib.h>


struct Parser *create_parser(struct Lexer *lexer)
{
    struct Parser *parser = malloc(sizeof(struct Parser));
    check_memory(parser);

    parser->lexer = lexer;
    parser->current = lexer_get_token(lexer);
    parser->prev = parser->current;

    return parser;
}


struct Ast *parser_parse(struct Parser *parser)
{
    return NULL;
}


struct Ast *parser_parse_statements(struct Parser *parser)
{
    return NULL;
}


struct Ast *parser_parse_statement(struct Parser *parser)
{
    return NULL;
}


struct Ast *parser_parse_expression(struct Parser *parser)
{
    return NULL;
}


struct Ast *parser_parse_fn_def(struct Parser *parser)
{
    return NULL;
}


struct Ast *parser_parse_fn_call(struct Parser *parser)
{
    return NULL;
}
