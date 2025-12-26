#include "token.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char tokentype_names[NUM_TOKEN_TYPES][MAX_TOKEN_TYPE_LEN] = {
    "ILLEGAL",  "EOF",   "COMMA", "IDENT",  "NUMBER", "PLUS",   "MINUS",
    "ASTERISK", "SLASH", "CARET", "LPAREN", "RPAREN", "LBRACE", "RBRACE"};
Precedence precedences[NUM_TOKEN_TYPES] = {
    LOWEST,  LOWEST,  LOWEST,   LOWEST, LOWEST, SUM,    SUM,
    PRODUCT, PRODUCT, EXPONENT, CALL,   LOWEST, LOWEST, LOWEST};

Token *new_token(TokenType type, char literal) {
    Token *token = (Token *)malloc(sizeof(Token));
    assertNotNull(token);
    token->type = type;
    char *str = calloc(2, sizeof(char));
    str[0] = literal;
    token->literal = str;
    token->length = 1;
    return token;
}

void free_token(Token **tok) {
    if (tok == NULL || *tok == NULL) {
        return;
    }
    safe_free((void **)&(*tok)->literal);
    safe_free((void **)tok);
}
