#include "lexer.h"
#include "token.h"
#include "util.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Lexer *new_lexer(char *input, size_t n) {
    Lexer *l = (Lexer *)calloc(1, sizeof(Lexer));
    assertNotNull(l);
    l->input = input;
    l->input_len = strnlen(input, n);
    read_char(l);
    return l;
}

void free_lexer(Lexer **l) {
    if (l == NULL || *l == NULL) {
        return;
    }
    safe_free((void **)&(*l)->input);
    safe_free((void **)l);
}

Token *lexer_next_token(Lexer *l) {
    assertNotNull(l);
    Token *token;
    skip_whitespace(l);
    switch (l->ch) {
    case '+':
        token = new_token(PLUS, l->ch);
        break;
    case '-':
        token = new_token(MINUS, l->ch);
        break;
    case '*':
        token = new_token(ASTERISK, l->ch);
        break;
    case '/':
        token = new_token(SLASH, l->ch);
        break;
    case '^':
        token = new_token(CARET, l->ch);
        break;
    case '(':
        token = new_token(LPAREN, l->ch);
        break;
    case ')':
        token = new_token(RPAREN, l->ch);
        break;
    case '{':
        token = new_token(LBRACE, l->ch);
        break;
    case '}':
        token = new_token(RBRACE, l->ch);
        break;
    case ',':
        token = new_token(COMMA, l->ch);
        break;
    case 0:
        token = new_token(TOKEN_EOF, 0);
        break;
    default:
        if (is_letter(l->ch)) {
            return read_word(l);
        } else if (is_num(l->ch)) {
            return read_num(l);
        } else {
            token = new_token(ILLEGAL, l->ch);
        }
        break;
    }
    read_char(l);
    return token;
}
// Reads next character into Lexer.
void read_char(Lexer *l) {
    assertNotNull(l);
    if (l->read_position >= 0 && (size_t)l->read_position >= l->input_len) {
        l->ch = 0;
    } else {
        l->ch = l->input[l->read_position];
    }
    l->position = l->read_position;
    l->read_position++;
}

Token *read_word(Lexer *l) {
    assertNotNull(l);
    int position = l->position;
    while (is_letter(l->ch)) {
        read_char(l);
    }
    char *out = (char *)malloc((l->position - position + 1) * sizeof(char));
    assertNotNull(out);
    slice(l->input, out, position, l->position);

    Token *token = (Token *)malloc(sizeof(Token));
    assertNotNull(token);
    token->literal = out;
    int length = strnlen(token->literal, l->position - position);
    token->type = IDENT;
    token->length = length;
    return token;
}

Token *read_num(Lexer *l) {
    assertNotNull(l);
    int position = l->position;
    while (is_num(l->ch)) {
        read_char(l);
    }
    char *out = (char *)malloc((l->position - position + 1) * sizeof(char));
    assertNotNull(out);
    slice(l->input, out, position, l->position);

    Token *token = (Token *)malloc(sizeof(Token));
    assertNotNull(token);
    token->literal = out;
    token->length = strnlen(token->literal, l->position - position);
    token->type = NUMBER;
    return token;
}

char peek_char(Lexer *l) {
    assertNotNull(l);
    if (l->read_position >= 0 && (size_t)l->read_position >= l->input_len) {
        return 0;
    } else {
        return l->input[l->read_position];
    }
}

void skip_whitespace(Lexer *l) {
    assertNotNull(l);
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r') {
        read_char(l);
    }
}

int is_letter(char ch) {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}

int is_num(char ch) { return ('0' <= ch && ch <= '9') || ch == '.'; }

void slice(const char *str, char *result, size_t start, size_t end) {
    int n = end - start;
    strncpy(result, str + start, n);
    result[n] = '\0';
}
