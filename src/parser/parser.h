/*
    Made by Noah Van Miert
    7/12/2022

    Apollo Compiler
*/

#ifndef __PARSER_H_
#define __PARSER_H_

#include "ast/ast.h"
#include "../lexer/lexer.h"


struct Parser {
    struct Token *current;
    struct Token *prev;
    struct Lexer *lexer;

    char entry_point_found;
};


struct Parser *create_parser(struct Lexer *lexer);

struct Ast *parser_parse(struct Parser *parser);
struct Ast *parser_parse_statements(struct Parser *parser);
struct Ast *parser_parse_statement(struct Parser *parser);
struct Ast *parser_parse_expression(struct Parser *parser);

struct Ast *parser_parse_fn_def(struct Parser *parser);
struct Ast *parser_parse_fn_call(struct Parser *parser);

struct Ast *parser_parse_word(struct Parser *parser);
struct Ast *parser_parse_string(struct Parser *parser);


#endif