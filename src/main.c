/*
    Made by Noah Van Miert
    6/12/2022

    Apollo Compiler
*/

#include "parser/parser.h"

#include "core.h"
#include "compiler/compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
    struct Lexer *lexer = create_lexer("fun main() {}");
    struct Parser *parser = create_parser(lexer);

    struct Ast *root = parser_parse(parser);

    clean_memory(lexer);
    clean_memory(parser);

    const char *_nasm = nasm_compile(root);
    clean_memory(root);

    FILE *f = fopen("output.asm", "w");
    fputs(_nasm, f);
    fclose(f);
}