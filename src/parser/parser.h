/*
    Made by Noah Van Miert
    7/12/2022

    Apollo Compiler
*/

#ifndef __PARSER_H_
#define __PARSER_H_

#include "ast/ast.h"
#include "../core.h"
#include "../lexer/lexer.h"
#include "../scope/scope.h"


struct Parser {
    struct Token *current;
    struct Token *prev;
    struct Lexer *lexer;

    bool entry_point_found;
    bool in_function;

    size_t string_addr;
    size_t var_offset;
};


struct Parser *create_parser(struct Lexer *lexer);

struct Ast *parser_parse(struct Scope *scope, struct Parser *parser);
struct Ast *parser_parse_statements(struct Scope *scope, struct Parser *parser);
struct Ast *parser_parse_statement(struct Scope *scope, struct Parser *parser);
struct Ast *parser_parse_expression(struct Scope *scope, struct Parser *parser);

struct Ast *parser_parse_fn_def(struct Scope *scope, struct Parser *parser);
struct Ast *parser_parse_fn_call(struct Scope *scope, struct Parser *parser);

struct Ast *parser_parse_var_def(struct Scope *scope, struct Parser *parser);
struct Ast *parser_parse_var(struct Scope *scope, struct Parser *parser);

struct Ast *parser_parse_word(struct Scope *scope, struct Parser *parser);
struct Ast *parser_parse_string(struct Scope *scope, struct Parser *parser);
struct Ast *parser_parse_int(struct Scope *scope, struct Parser *parser);
struct Ast *parser_parse_bool(struct Scope *scope, struct Parser *parser);

void parser_type_check_syscall(struct Ast *ast, struct Location *loc);
void parser_type_check_sys_write(struct Ast *ast, struct Location *loc);
void parser_type_check_sys_exit(struct Ast *ast, struct Location *loc);


#endif
