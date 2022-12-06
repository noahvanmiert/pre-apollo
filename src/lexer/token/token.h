#ifndef __TOKEN_H_
#define __TOKEN_H_

#include <stddef.h>

struct Location {
    const char *filepath;
    size_t line;
    size_t column;
};


enum TokenType {
    TOKEN_WORD,
    TOKEN_STRING,
    TOKEN_END
};


struct Token {
    struct Location location;

    enum TokenType type;
    const char *value;
};

struct Token *create_token(struct Location location, enum TokenType type, const char *value);
const char *get_type_str(enum TokenType type);

#endif