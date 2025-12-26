#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include "token.h"

struct Parser;

typedef Expression *prefix_parse_fn(struct Parser *p);
typedef Expression *infix_parse_fn(struct Parser *p,
                                   Expression *left_expression);

typedef struct Parser {
    Lexer *l;
    Token *cur_token;
    Token *peek_token;
    char **errors;
    int num_errors;

    prefix_parse_fn *prefix_parse_fns[NUM_TOKEN_TYPES];
    infix_parse_fn *infix_parse_fns[NUM_TOKEN_TYPES];
} Parser;

typedef struct {
    Expression **arguments;
    int num_arguments;
} ArgumentArray;

Parser *new_parser(Lexer *l);
void print_parser_errors(Parser *p, FILE *out);
void free_parser(Parser **p);
void free_argument_array(ArgumentArray **arr);
void parser_next_token(Parser *p);
int parser_cur_token_is(Parser *p, TokenType t);
int parser_peek_token_is(Parser *p, TokenType t);
int parser_expect_peek(Parser *p, TokenType t);
Precedence cur_prec(Parser *p);
Precedence peek_prec(Parser *p);
double atod(char *str);

void register_prefix(Parser *p, TokenType token_type, prefix_parse_fn *fn);
void register_infix(Parser *p, TokenType token_type, infix_parse_fn *fn);

Expression *parse_expression_statement(Parser *p);
Expression *parse_expression(Parser *p, Precedence precedence);
Expression *parse_identifier(Parser *p);
Expression *parse_illegal_expression(Parser *p);
Expression *parse_prefix_expression(Parser *p);
Expression *parse_infix_expression(Parser *p, Expression *left_expression);
Expression *parse_number_literal(Parser *p);
Expression *parse_grouped_expression(Parser *p);
Expression *parse_call_expression(Parser *p, Expression *function);
ArgumentArray *parse_call_arguments(Parser *p);

#endif
