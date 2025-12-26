#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <stddef.h>

typedef struct {
    char *input;
    size_t input_len;
    int position;
    int read_position;
    char ch;
} Lexer;

Lexer *new_lexer(char *input, size_t n);
void free_lexer(Lexer **l);
Token *lexer_next_token(Lexer *l);
void read_char(Lexer *l);
Token *read_word(Lexer *l);
Token *read_num(Lexer *l);
char peek_char(Lexer *l);
int is_letter(char ch);
int is_num(char ch);
void skip_whitespace(Lexer *l);
void slice(const char *str, char *result, size_t start, size_t end);

#endif
