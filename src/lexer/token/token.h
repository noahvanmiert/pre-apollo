/*
    Made by Noah Van Miert
    6/12/2022

    Apollo Compiler
*/

#ifndef __TOKEN_H_
#define __TOKEN_H_

#include <stddef.h>


enum TokenType {
    TOKEN_WORD,
    TOKEN_STRING,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LCURL,
    TOKEN_RCURL,
    TOKEN_SEMICOLON,
    TOKEN_END
};


struct Token {
    enum TokenType type;
    const char *value;
};


struct Token *create_token(enum TokenType type, const char *value);
const char *get_type_str(enum TokenType type);

#endif
