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


static struct CToken *read_file(const char *filepath)
{
    FILE *fptr = fopen(filepath, "r");
    
    if (!fptr)
        apo_error("ERROR: could not open file '%s'", filepath);

    struct CToken *buffer = xmalloc(FILE_BUFFER_SIZE * sizeof(struct CToken));
	size_t buffer_size = FILE_BUFFER_SIZE;
	size_t buffer_index = 0;

	char c = ' ';
    size_t line = 1;
    size_t col = 0;

	while (c != EOF) {
		if (buffer_index >= buffer_size - 1) {
			buffer = xrealloc(buffer, buffer_size + FILE_BUFFER_SIZE);
			buffer_size += FILE_BUFFER_SIZE;
		}

		c = fgetc(fptr);
        col++;
        
        if (c == '\n') {
            line++;
            col = 1;
        }
		
		if (c != EOF) {
            buffer[buffer_index].filepath = filepath;
            buffer[buffer_index].line = line;
            buffer[buffer_index].col = col;
			buffer[buffer_index].value = c;

			buffer_index += 1;
		}
	}

    if (buffer_index > 1) {
        buffer = xrealloc(buffer, (buffer_index + 1) * sizeof(struct CToken));
        buffer[buffer_index].value = '\0';

        return buffer;
    }

    return &(struct CToken) { .filepath = filepath, .line = 0, .col = 0, .value = '\0' };
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
    while (is_white_space(lexer->current.value)) 
        advance(lexer);
}


static struct Token *collect_word(struct Lexer *lexer) 
{
    char *word = calloc(1, sizeof(char));

    struct Location loc = (struct Location) {
        .filepath = lexer->current.filepath,
        .line = lexer->current.line,
        .col = lexer->current.col
    };

    while (isalpha(lexer->current.value) || isalnum(lexer->current.value) || lexer->current.value == '_') {
        word = realloc(word, (strlen(word) + 2) * sizeof(char));

        strcat(word, (char[]) {lexer->current.value, '\0'});
        advance(lexer);
    }

    return create_token(TOKEN_WORD, &loc, (const char *) word);
}


static struct Token *collect_string(struct Lexer *lexer)
{
    char *string = xcalloc(1, sizeof(char));

    // for the first '"'
    advance(lexer);

    struct Location loc = (struct Location) {
        .filepath = lexer->current.filepath,
        .line = lexer->current.line,
        .col = lexer->current.col
    };

    while (lexer->current.value != '"') {
        string = xrealloc(string, (strlen(string) + 2) * sizeof(char));

        strcat(string, (char[]) {lexer->current.value, '\0'});
        advance(lexer);
    }

    // for the last '"'
    advance(lexer);

    return create_token(TOKEN_STRING, &loc, (const char *) string);
}


static struct Token *collect_int(struct Lexer *lexer)
{
    char *_int = xcalloc(1, sizeof(char));

    struct Location loc = (struct Location) {
        .filepath = lexer->current.filepath,
        .line = lexer->current.line,
        .col = lexer->current.col
    };

    while (isalnum(lexer->current.value)) {
        _int = xrealloc(_int, (strlen(_int) + 2) * sizeof(char));

        strcat(_int, (char[]) {lexer->current.value, '\0'});
        advance(lexer);
    }

    return create_token(TOKEN_INT, &loc, (const char *) _int);
}


#define return_token(type, loc, value) advance(lexer); \
                                return create_token(type, loc, value);


static struct Token *collect_special_chr(struct Lexer *lexer)
{
    struct Location loc = (struct Location) {
        .filepath = lexer->current.filepath,
        .line = lexer->current.line,
        .col = lexer->current.col
    };

    switch (lexer->current.value)
    {
        case '(': return_token(TOKEN_LPAREN, &loc, "(");
        case ')': return_token(TOKEN_RPAREN, &loc, ")");
        case '{': return_token(TOKEN_LCURL, &loc, "{");
        case '}': return_token(TOKEN_RCURL, &loc, "}");
        case ';': return_token(TOKEN_SEMICOLON, &loc, ";");
        case ',': return_token(TOKEN_COMMA, &loc, ",");
        case '\0': return create_token(TOKEN_END, NULL, NULL);

        default: {
            apo_compiler_error(
                loc.filepath,
                loc.line,
                loc.col,
                "error: unkown character '%c'", lexer->current.value
            );
        }
    }
}


struct Token *lexer_get_token(struct Lexer *lexer)
{
    skip_white_space(lexer);
    if (lexer->current.value == '\0' || lexer->current.value == EOF) 
        return create_token(TOKEN_END, NULL, NULL);

    if (isalpha(lexer->current.value) || lexer->current.value == '_')
        return collect_word(lexer);

    if (isalnum(lexer->current.value))
        return collect_int(lexer);

    if (lexer->current.value == '"')
        return collect_string(lexer);

    return collect_special_chr(lexer);
}