#include "evaluator.h"
#include "ast.h"
#include "util.h"
#include <errno.h>
#include <math.h>
#include <string.h>

char keywords[NUM_KEYWORDS][MAX_KEYWORD_LEN] = {
    "sqrt", "rootn", "log",  "logn", "ln", "sin", "cos", "tan",
    "asin", "acos",  "atan", "sum",  "pi", "e",   "ans", "i"};
KeywordType keyword_types[NUM_KEYWORDS] = {
    SQRT, ROOTN, LOG,  LOGN,   LN, SIN, COS, TAN,
    ASIN, ACOS,  ATAN, KW_SUM, PI, E,   ANS, I};
int keyword_num_args[NUM_KEYWORDS] = {1, 2, 1, 2, 1, 1, 1, 1,
                                      1, 1, 1, 3, 0, 1, 0, 0};

KeywordType lookup_keyword(char *keyword, size_t length) {
    for (int i = 0; i < NUM_KEYWORDS; i++) {
        char *kw = keywords[i];
        if (strncmp(keyword, kw, length) == 0) {
            return keyword_types[i];
        }
    }
    return -1;
}

double eval(Expression *expr, double env_vars[NUM_ENV_VARS]) {
    assertNotNull(expr);
    switch (expr->type) {
    case NUMBER_LITERAL:
        return expr->expression.number_literal->value;
    case IDENTIFIER:
        return eval_identifier_expression(expr->expression.identifier,
                                          env_vars);
    case PREFIX_EXPRESSION:
        // only MINUS operator
        return -eval(expr->expression.prefix_expression->right, env_vars);
    case INFIX_EXPRESSION:
        return eval_infix_expression(expr->expression.infix_expression,
                                     env_vars);
    case CALL_EXPRESSION:
        return eval_call_expression(expr->expression.call_expression, env_vars);
    default:
        printf("Error: Invalid Expression node.\n");
        errno = EINVAL;
        return 0.0;
    }
}

double eval_identifier_expression(Identifier *expr,
                                  double env_vars[NUM_ENV_VARS]) {
    assertNotNull(expr);
    KeywordType kw = lookup_keyword(expr->value, expr->length);
    switch (kw) {
    case PI:
        return M_PI;
    case E:
        return M_E;
    case ANS:
        return env_vars[ENV_ANS];
    case I:
        return env_vars[ENV_I];
    default:
        printf("Error: Invalid Identifier node.\n");
        errno = EINVAL;
        return 0.0;
    }
}

double eval_infix_expression(InfixExpression *expr,
                             double env_vars[NUM_ENV_VARS]) {
    assertNotNull(expr);
    double left = eval(expr->left, env_vars);
    double right = eval(expr->right, env_vars);
    char *op = expr->op;
    int op_len = expr->token->length;
    if (strncmp(op, "+", op_len) == 0) {
        return left + right;
    } else if (strncmp(op, "-", op_len) == 0) {
        return left - right;
    } else if (strncmp(op, "*", op_len) == 0) {
        return left * right;
    } else if (strncmp(op, "/", op_len) == 0) {
        return left / right;
    } else if (strncmp(op, "^", op_len) == 0) {
        return pow(left, right);
    }
    printf("Error: Invalid infix expression.\n");
    errno = EINVAL;
    return 0.0;
}

double eval_call_expression(CallExpression *expr,
                            double env_vars[NUM_ENV_VARS]) {
    assertNotNull(expr);
    double x, n;
    KeywordType kw =
        lookup_keyword(expr->function->expression.identifier->token->literal,
                       expr->function->expression.identifier->token->length);
    if ((int)kw == -1) {
        printf("Error: Invalid call expression '%s'.\n",
               expr->function->expression.identifier->token->literal);
        errno = EINVAL;
        return 0.0;
    }
    if (expr->num_arguments != keyword_num_args[kw]) {
        printf("Error: Invalid number of arguments in %s call expression.\n",
               keywords[kw]);
        errno = EINVAL;
        return 0.0;
    }

    switch (kw) {
    case SQRT:
        return sqrt(eval(expr->arguments[0], env_vars));
    case ROOTN:
        x = eval(expr->arguments[0], env_vars);
        n = eval(expr->arguments[1], env_vars);
        return pow(x, 1 / n);
    case LOG:
        return log(eval(expr->arguments[0], env_vars)) / log(10);
    case LOGN:
        x = eval(expr->arguments[0], env_vars);
        n = eval(expr->arguments[1], env_vars);
        return log(x) / log(n);
    case LN:
        return log(eval(expr->arguments[0], env_vars));
    case E:
        return pow(M_E, eval(expr->arguments[0], env_vars));
    case SIN:
        return sin(eval(expr->arguments[0], env_vars));
    case COS:
        return cos(eval(expr->arguments[0], env_vars));
    case TAN:
        return tan(eval(expr->arguments[0], env_vars));
    case ASIN:
        return asin(eval(expr->arguments[0], env_vars));
    case ACOS:
        return acos(eval(expr->arguments[0], env_vars));
    case ATAN:
        return atan(eval(expr->arguments[0], env_vars));
    case KW_SUM:
        x = 0;
        int start = (int)eval(expr->arguments[0], env_vars);
        int end = (int)eval(expr->arguments[1], env_vars);
        for (int i = start; i <= end; i++) {
            env_vars[ENV_I] = i;
            x += eval(expr->arguments[2], env_vars);
        }
        return x;
    default:
        printf("Error: Invalid call expression node.\n");
        errno = EINVAL;
        return 0.0;
    }
}
