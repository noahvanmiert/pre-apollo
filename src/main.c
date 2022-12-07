/*
    Made by Noah Van Miert
    6/12/2022

    Apollo Compiler
*/

#include <stdio.h>

#include "lexer/lexer.h"
#include "parser/ast/ast.h"

int main() {
    struct Lexer lexer = (struct Lexer) {
        .data = "fun main() { print(\"Hello, World!\"); }",
        .index = 0,
        .current = 'f'
    };

    struct Token *token = NULL;

    while ((token = lexer_get_token(&lexer))->type != TOKEN_END) {
        printf("TOKEN: '%s'\n", token->value);
    }

    struct Ast *ast = create_ast(AST_STRING);
    ast->string_value = "Hello, World!";

    printf("\nAST_STRING: '%s'\n", (const char *) ast->string_value);
}