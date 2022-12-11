/*
    Made by Noah Van Miert
    6/12/2022

    Apollo Compiler
*/

#include "lexer.h"

#include "../core.h"
#include "../parser/logging/logging.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_BUFFER_SIZE 4096


static const char *read_file(const char *filepath)
{
    FILE *fptr = fopen(filepath, "r");
    
    if (!fptr)
        apo_error("ERROR: could not open file '%s'", filepath);

    char *buffer = xmalloc(FILE_BUFFER_SIZE * sizeof(char));
	size_t buffer_size = FILE_BUFFER_SIZE;
	size_t buffer_index = 0;
	char c = ' ';

	while (c != EOF) {
		if (buffer_index >= buffer_size - 1) {
			buffer = xrealloc(buffer, buffer_size + FILE_BUFFER_SIZE);
			buffer_size += FILE_BUFFER_SIZE;
		}

		c = fgetc(fptr);
		
		if (c != EOF) {
			buffer[buffer_index] = c;
			buffer_index += 1;
		}
	}

    if (buffer_index > 1) {
        buffer = xrealloc(buffer, (buffer_index + 1) * sizeof(char));
        buffer[buffer_index] = '\0';

        return (const char *) buffer;
    }

    return "";
}


struct Lexer *create_lexer(const char *filepath)
{
    struct Lexer *lexer = xmalloc(sizeof(struct Lexer));

    lexer->data = read_file(filepath);
    lexer->index = 0;
    lexer->current = lexer->data[0];

    return lexer;
}


static void advance(struct Lexer *lexer)
{
    lexer->index++;
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
    char *word = xcalloc(1, sizeof(char));

    while (isalpha(lexer->current) || isalnum(lexer->current) || lexer->current == '_') {
        word = xrealloc(word, (strlen(word) + 2) * sizeof(char));

        strcat(word, (char[]) {lexer->current, '\0'});
        advance(lexer);
    }

    return create_token(TOKEN_WORD, (const char *) word);
}


static struct Token *collect_string(struct Lexer *lexer)
{
    char *string = xcalloc(1, sizeof(char));

    // for the first '"'
    advance(lexer);

    while (lexer->current != '"') {
        string = xrealloc(string, (strlen(string) + 2) * sizeof(char));

        strcat(string, (char[]) {lexer->current, '\0'});
        advance(lexer);
    }

    // for the last '"'
    advance(lexer);

    return create_token(TOKEN_STRING, (const char *) string);
}


static struct Token *collect_int(struct Lexer *lexer)
{
    char *_int = xcalloc(1, sizeof(char));

    while (isalnum(lexer->current)) {
        _int = xrealloc(_int, (strlen(_int) + 2) * sizeof(char));

        strcat(_int, (char[]) {lexer->current, '\0'});
        advance(lexer);
    }

    return create_token(TOKEN_INT, (const char *) _int);
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
        case ',': return_token(TOKEN_COMMA, ",");

        default: apo_error("ERROR: unkown character '%c'", lexer->current);
    }
}


struct Token *lexer_get_token(struct Lexer *lexer)
{
    skip_white_space(lexer);
    if (lexer->current == '\0' || lexer->current == EOF) 
        return create_token(TOKEN_END, NULL);

    if (isalpha(lexer->current) || lexer->current == '_')
        return collect_word(lexer);

    if (isalnum(lexer->current))
        return collect_int(lexer);

    if (lexer->current == '"')
        return collect_string(lexer);

    return collect_special_chr(lexer);
}