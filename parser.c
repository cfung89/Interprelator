#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "util.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

Parser *new_parser(Lexer *l) {
    Parser *p = (Parser *)calloc(1, sizeof(Parser));
    assertNotNull(p);
    p->l = l;
    parser_next_token(p);
    parser_next_token(p);

    register_prefix(p, IDENT, parse_identifier);
    register_prefix(p, NUMBER, parse_number_literal);
    register_prefix(p, MINUS, parse_prefix_expression);
    register_prefix(p, LPAREN, parse_grouped_expression);

    register_infix(p, PLUS, parse_infix_expression);
    register_infix(p, MINUS, parse_infix_expression);
    register_infix(p, ASTERISK, parse_infix_expression);
    register_infix(p, SLASH, parse_infix_expression);
    register_infix(p, CARET, parse_infix_expression);
    register_infix(p, LPAREN, parse_call_expression);
    return p;
}

void print_parser_errors(Parser *p, FILE *out) {
    assertNotNull(p);
    for (int i = 0; i < p->num_errors; i++) {
        fprintf(out, "%s\n", p->errors[i]);
    }
}

void free_parser(Parser **p) {
    if (p == NULL || *p == NULL) {
        return;
    }
    free_lexer(&(*p)->l);
    // cur_token is freed by free_expression on root node
    if (parser_peek_token_is(*p, TOKEN_EOF)) {
        free_token(&(*p)->peek_token); // free EOF token
    }
    for (int i = 0; i < (*p)->num_errors; i++) {
        safe_free((void **)&(*p)->errors[i]);
    }
    safe_free((void **)&(*p)->errors);
    safe_free((void **)p);
}

void free_argument_array(ArgumentArray **array) {
    if (array == NULL || *array == NULL) {
        return;
    }
    ArgumentArray *arr = *array;
    if (arr->arguments == NULL) {
        safe_free((void **)array);
        return;
    }
    for (int i = 0; i < arr->num_arguments; i++) {
        free_expression(&arr->arguments[i]);
    }
    safe_free((void **)&arr->arguments);
    safe_free((void **)array);
}

void parser_next_token(Parser *p) {
    assertNotNull(p);
    p->cur_token = p->peek_token;
    p->peek_token = lexer_next_token(p->l);
}

int parser_cur_token_is(Parser *p, TokenType t) {
    assertNotNull(p);
    assertNotNull(p->cur_token);
    return p->cur_token->type == t;
}

int parser_peek_token_is(Parser *p, TokenType t) {
    assertNotNull(p);
    assertNotNull(p->peek_token);
    return p->peek_token->type == t;
}

int parser_expect_peek(Parser *p, TokenType t) {
    if (parser_peek_token_is(p, t)) {
        parser_next_token(p);
        return 1;
    }
    return 0;
}

Precedence cur_prec(Parser *p) {
    assertNotNull(p);
    assertNotNull(p->cur_token);
    return precedences[p->cur_token->type];
}

Precedence peek_prec(Parser *p) {
    assertNotNull(p);
    assertNotNull(p->peek_token);
    return precedences[p->peek_token->type];
}

void register_prefix(Parser *p, TokenType token_type, prefix_parse_fn *fn) {
    assertNotNull(p);
    p->prefix_parse_fns[token_type] = fn;
}
void register_infix(Parser *p, TokenType token_type, infix_parse_fn *fn) {
    assertNotNull(p);
    p->infix_parse_fns[token_type] = fn;
}

double atod(char *str) {
    assertNotNull(str);
    char *end;
    errno = 0;
    double val = strtod(str, &end);
    if (end == str) {
        errno = ERANGE;
    }
    return val;
}

Expression *parse_expression_statement(Parser *p) {
    assertNotNull(p);
    Expression *expr = parse_expression(p, LOWEST);
    if (p != NULL && expr == NULL && errno != EINVAL) {
        free_token(&p->cur_token);
    }
    return expr;
}

Expression *parse_expression(Parser *p, Precedence precedence) {
    assertNotNull(p);
    assertNotNull(p->cur_token);
    prefix_parse_fn *prefix = p->prefix_parse_fns[p->cur_token->type];
    if (prefix == NULL) {
        return NULL;
    }
    Expression *left_expression = prefix(p);
    if (left_expression == NULL) {
        return NULL;
    }
    while (!parser_peek_token_is(p, TOKEN_EOF) && precedence < peek_prec(p)) {
        infix_parse_fn *infix = p->infix_parse_fns[p->peek_token->type];
        if (infix == NULL) {
            return NULL;
        }
        parser_next_token(p);
        left_expression = infix(p, left_expression);
        if (left_expression == NULL) {
            return NULL;
        }
    }
    if (parser_cur_token_is(p, RPAREN)) {
        free_token(&p->cur_token);
    }
    return left_expression;
}

Expression *parse_number_literal(Parser *p) {
    assertNotNull(p);
    double value = atod(p->cur_token->literal);
    if (errno == ERANGE) {
        return NULL;
    }

    NumberLiteral *literal = (NumberLiteral *)malloc(sizeof(NumberLiteral));
    assertNotNull(literal);
    literal->token = p->cur_token;
    literal->value = value;

    Expression *expr = (Expression *)malloc(sizeof(Expression));
    assertNotNull(expr);
    expr->expression.number_literal = literal;
    expr->type = NUMBER_LITERAL;
    return expr;
}

Expression *parse_identifier(Parser *p) {
    assertNotNull(p);
    Identifier *ident = (Identifier *)malloc(sizeof(Identifier));
    assertNotNull(ident);
    ident->token = p->cur_token;
    ident->value = p->cur_token->literal;
    ident->length = p->cur_token->length;

    Expression *expr = (Expression *)malloc(sizeof(Expression));
    assertNotNull(expr);
    expr->expression.identifier = ident;
    expr->type = IDENTIFIER;
    return expr;
}

Expression *parse_prefix_expression(Parser *p) {
    assertNotNull(p);
    assertNotNull(p->cur_token);
    PrefixExpression *prefix =
        (PrefixExpression *)malloc(sizeof(PrefixExpression));
    assertNotNull(prefix);
    prefix->token = p->cur_token;
    prefix->op = p->cur_token->literal;

    parser_next_token(p);
    Expression *right = parse_expression(p, PREFIX);
    if (right == NULL) {
        free_prefix_expression(&prefix);
        return NULL;
    }
    prefix->right = right;

    Expression *expr = (Expression *)malloc(sizeof(Expression));
    assertNotNull(expr);
    expr->expression.prefix_expression = prefix;
    expr->type = PREFIX_EXPRESSION;
    return expr;
}

Expression *parse_infix_expression(Parser *p, Expression *left_expression) {
    assertNotNull(p);
    assertNotNull(p->cur_token);
    assertNotNull(left_expression);
    InfixExpression *infix = (InfixExpression *)malloc(sizeof(InfixExpression));
    assertNotNull(infix);
    infix->token = p->cur_token;
    infix->op = p->cur_token->literal;
    infix->left = left_expression;

    Precedence prec = cur_prec(p);
    parser_next_token(p);
    Expression *right = parse_expression(p, prec);
    if (right == NULL) {
        free_infix_expression(&infix);
        return NULL;
    }
    infix->right = right;

    Expression *expr = (Expression *)malloc(sizeof(Expression));
    assertNotNull(expr);
    expr->expression.infix_expression = infix;
    expr->type = INFIX_EXPRESSION;
    return expr;
}

Expression *parse_grouped_expression(Parser *p) {
    assertNotNull(p);
    assert(p->cur_token->type == LPAREN);
    free_token(&p->cur_token);
    parser_next_token(p);
    Expression *expr = parse_expression(p, LOWEST);
    if (!parser_expect_peek(p, RPAREN)) {
        free_expression(&expr);
        return NULL;
    }
    return expr;
}

Expression *parse_call_expression(Parser *p, Expression *function) {
    assertNotNull(p);
    assertNotNull(p->cur_token);
    assertNotNull(function);
    CallExpression *call_expression =
        (CallExpression *)calloc(1, sizeof(CallExpression));
    assertNotNull(call_expression);
    call_expression->token = p->cur_token;
    call_expression->function = function;
    ArgumentArray *arr = parse_call_arguments(p);
    if (arr == NULL) {
        free_call_expression(&call_expression);
        return NULL;
    }
    call_expression->arguments = arr->arguments;
    call_expression->num_arguments = arr->num_arguments;
    safe_free((void **)&arr);

    Expression *expr = (Expression *)malloc(sizeof(Expression));
    assertNotNull(expr);
    expr->expression.call_expression = call_expression;
    expr->type = CALL_EXPRESSION;
    return expr;
}

ArgumentArray *parse_call_arguments(Parser *p) {
    assertNotNull(p);
    errno = 0;
    int cap = 4;
    ArgumentArray *arr = (ArgumentArray *)malloc(sizeof(ArgumentArray));
    assertNotNull(arr);
    Expression **arguments = (Expression **)malloc(cap * sizeof(Expression *));
    assertNotNull(arguments);
    arr->arguments = arguments;
    arr->num_arguments = 0;

    if (parser_peek_token_is(p, RPAREN)) {
        parser_next_token(p);
        return arr;
    }

    parser_next_token(p);
    arguments[0] = parse_expression(p, LOWEST);
    if (arguments[0] == NULL) {
        free_argument_array(&arr);
        return NULL;
    }
    arr->num_arguments++;

    while (parser_peek_token_is(p, COMMA)) {
        parser_next_token(p);
        free_token(&p->cur_token); // free COMMA token
        parser_next_token(p);
        if (arr->num_arguments >= cap) {
            cap *= 2;
            arguments = (Expression **)realloc(arguments, cap);
            assertNotNull(arguments);
        }
        Expression *expr = parse_expression(p, LOWEST);
        if (expr == NULL) {
            free_argument_array(&arr);
            return NULL;
        }
        arguments[arr->num_arguments] = expr;
        arr->num_arguments++;
    }
    if (!parser_expect_peek(p, RPAREN)) {
        errno = EINVAL;
        free_argument_array(&arr);
        return NULL;
    }
    return arr;
}
