/*
    Made by Noah Van Miert
    6/12/2022

    Apollo Compiler
*/

#ifndef __TOKEN_H_
#define __TOKEN_H_

#include <stddef.h>

struct CToken {
    const char *filepath;
    size_t line, col;
    char value;
};


struct Location {
    const char *filepath;
    size_t line, col;
};


enum TokenType {
    TOKEN_WORD,
    TOKEN_STRING,
    TOKEN_INT,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LCURL,
    TOKEN_RCURL,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_EQ,
    TOKEN_END
};


struct Token {
    const char *filepath;
    size_t line, col;

    enum TokenType type;
    const char *value;
};


struct Token *create_token(enum TokenType type, struct Location *loc, const char *value);
const char *get_type_str(enum TokenType type);

#endif
