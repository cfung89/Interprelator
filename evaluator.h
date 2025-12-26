#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ast.h"

#define NUM_ENV_VARS 2
#define NUM_KEYWORDS 16
#define MAX_KEYWORD_LEN 6

typedef enum {
    SQRT,
    ROOTN,
    LOG,
    LOGN,
    LN,
    SIN,
    COS,
    TAN,
    ASIN,
    ACOS,
    ATAN,
    KW_SUM,
    PI,
    E,
    ANS,
    I,
} KeywordType;

extern char keywords[NUM_KEYWORDS][MAX_KEYWORD_LEN];
extern KeywordType keyword_types[NUM_KEYWORDS];
extern int keyword_num_args[NUM_KEYWORDS];

KeywordType lookup_keyword(char *keyword, size_t length);

double eval(Expression *expr, double env_vars[NUM_ENV_VARS]);
double eval_identifier_expression(Identifier *expr,
                                  double env_vars[NUM_ENV_VARS]);
double eval_infix_expression(InfixExpression *expr,
                             double env_vars[NUM_ENV_VARS]);
double eval_call_expression(CallExpression *expr,
                            double env_vars[NUM_ENV_VARS]);

typedef enum { ENV_ANS, ENV_I } Environment;

#endif
