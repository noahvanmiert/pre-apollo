/*
    Made by Noah Van Miert
    6/12/2022

    Apollo Compiler
*/

#include <stdio.h>

#include "parser/parser.h"
#include "core.h"
#include <stdlib.h>

int main() {
    struct Lexer *lexer = create_lexer("fun main() { print(\"Hello, World!\"); }");
    struct Parser *parser = create_parser(lexer);

    clean_memory(lexer);
    clean_memory(parser);
}