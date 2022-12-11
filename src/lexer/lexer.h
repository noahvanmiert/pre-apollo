/*
    Made by Noah Van Miert
    6/12/2022

    Apollo Compiler
*/

#ifndef __LEXER_H_
#define __LEXER_H_

#include "token/token.h"

#include <stddef.h>

struct Lexer {
    struct CToken *data;
    size_t index;
    struct CToken current;
};


struct Lexer *create_lexer(const char *data);
struct Token *lexer_get_token(struct Lexer *lexer);

#endif
