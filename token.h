#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

#define MAX_TOKEN_TYPE_LEN 9
#define NUM_TOKEN_TYPES 14

typedef enum {
    ILLEGAL,
    TOKEN_EOF,
    COMMA,

    // Operands
    IDENT,
    NUMBER,

    // Operators
    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    CARET,

    // Brackets
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
} TokenType;

typedef enum {
    LOWEST,
    SUM,
    PRODUCT,
    EXPONENT,
    PREFIX,
    CALL,
} Precedence;

typedef struct {
    TokenType type;
    char *literal;
    size_t length;
} Token;

extern char tokentype_names[NUM_TOKEN_TYPES][MAX_TOKEN_TYPE_LEN];
extern Precedence precedences[NUM_TOKEN_TYPES];

Token *new_token(TokenType type, char literal);
void free_token(Token **tok);

#endif
