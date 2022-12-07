/*
    Made by Noah Van Miert
    6/12/2022

    Apollo Compiler
*/

#include "lexer.h"

#include "../core.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static void advance(struct Lexer *lexer)
{
    lexer->index += 1;
    lexer->current = lexer->data[lexer->index];
}


static inline char is_white_space(char chr) 
{
    return chr == '\t' || chr == '\v' || chr == '\n' || chr == ' ';
}


static inline void skip_white_space(struct Lexer *lexer)
{
    while (is_white_space(lexer->current)) 
        advance(lexer);
}


static struct Token *collect_word(struct Lexer *lexer) 
{
    char *word = calloc(1, sizeof(char));
    check_memory(word);

    while (isalpha(lexer->current) || isalnum(lexer->current) || lexer->current == '_') {
        word = realloc(word, (strlen(word) + 2) * sizeof(char));
        check_memory(word);

        strcat(word, (char[]) {lexer->current, '\0'});
        advance(lexer);
    }

    return create_token(TOKEN_WORD, (const char *) word);
}


static struct Token *collect_string(struct Lexer *lexer)
{
    char *string = calloc(1, sizeof(char));
    check_memory(string);

    // for the first '"'
    advance(lexer);

    while (lexer->current != '"') {
        string = realloc(string, (strlen(string) + 2) * sizeof(char));
        check_memory(string);

        strcat(string, (char[]) {lexer->current, '\0'});
        advance(lexer);
    }

    // for the last '"'
    advance(lexer);

    return create_token(TOKEN_STRING, (const char *) string);
}


#define return_token(type, value) advance(lexer); \
                                return create_token(type, value);


static struct Token *collect_special_chr(struct Lexer *lexer)
{
    switch (lexer->current)
    {
    case '(': return_token(TOKEN_LPAREN, "(");
    case ')': return_token(TOKEN_RPAREN, ")");
    case '{': return_token(TOKEN_LCURL, "{");
    case '}': return_token(TOKEN_RCURL, "}");
    case ';': return_token(TOKEN_SEMICOLON, ";");

    default: {
        fprintf(stderr, 
            "ERROR: unkown character '%c'\n",
            lexer->current
        );

        exit(EXIT_FAILURE);
    };
    }
}


struct Token *lexer_get_token(struct Lexer *lexer)
{
    if (lexer->current == '\0')
        return create_token(TOKEN_END, NULL);

    skip_white_space(lexer);

    if (isalpha(lexer->current))
        return collect_word(lexer);

    if (lexer->current == '"')
        return collect_string(lexer);

    return collect_special_chr(lexer);
}