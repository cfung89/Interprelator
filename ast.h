#ifndef AST_H
#define AST_H

#include "token.h"
#include <stddef.h>
#include <stdio.h>

struct Expression;

typedef struct {
    Token *token;
    double value;
} NumberLiteral;

typedef struct {
    Token *token;
    char *value;
    size_t length;
} Identifier;

typedef struct {
    Token *token; // operator token
    char *op;     // operator
    struct Expression *right;
} PrefixExpression;

typedef struct {
    Token *token; // operator token
    struct Expression *left;
    char *op; // operator
    struct Expression *right;
} InfixExpression;

typedef struct {
    Token *token;                // '(' token
    struct Expression *function; // identifier
    struct Expression **arguments;
    int num_arguments;
} CallExpression;

typedef enum {
    NUMBER_LITERAL,
    IDENTIFIER,
    PREFIX_EXPRESSION,
    INFIX_EXPRESSION,
    CALL_EXPRESSION
} ExpressionType;

// Expression node tagged union
typedef struct Expression {
    union {
        NumberLiteral *number_literal;
        Identifier *identifier;
        PrefixExpression *prefix_expression;
        InfixExpression *infix_expression;
        CallExpression *call_expression;
    } expression;
    ExpressionType type;
} Expression;

void free_expression(Expression **expression);
void free_number_literal(NumberLiteral **expression);
void free_identifier(Identifier **expression);
void free_prefix_expression(PrefixExpression **expression);
void free_infix_expression(InfixExpression **expression);
void free_call_expression(CallExpression **expression);
void print_expression(FILE *out, Expression *expr);

#endif
