/*
    Made by Noah Van Miert
    6/12/2022

    Apollo Compiler
*/

#include "token.h"

#include "../../core.h"

#include <stdlib.h>


struct Token *create_token(enum TokenType type, struct Location *loc, const char *value)
{
    struct Token *token = xmalloc(sizeof(struct Token));

    if (likely(loc != NULL)) {
        token->filepath = loc->filepath;
        token->line = loc->line;
        token->col = loc->col;
    }

    token->type = type;
    token->value = value;

    return token;
}


const char *get_type_str(enum TokenType type)
{
    switch (type)
    {
        case TOKEN_WORD:      return "TOKEN_WORD";
        case TOKEN_STRING:    return "TOKEN_STRING";
        case TOKEN_INT:       return "TOKEN_INT";
		case TOKEN_BOOL:	  return "TOKEN_BOOL";
		case TOKEN_LPAREN:    return "TOKEN_LPAREN";
        case TOKEN_RPAREN:    return "TOKEN_RPAREN";
        case TOKEN_LCURL:     return "TOKEN_LCURL";
        case TOKEN_RCURL:     return "TOKEN_RCURL";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_EQ:        return "TOKEN_EQ";
        case TOKEN_COLON:     return "TOKEN_COLON";
        case TOKEN_END:       return "TOKEN_END";

        default: assert(0);
    }
}
