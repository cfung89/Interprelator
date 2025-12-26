#include "ast.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void free_expression(Expression **expression) {
    if (expression == NULL || *expression == NULL) {
        return;
    }
    Expression *expr = *expression;
    switch (expr->type) {
    case NUMBER_LITERAL:
        free_number_literal(&expr->expression.number_literal);
        break;
    case IDENTIFIER:
        free_identifier(&expr->expression.identifier);
        break;
    case PREFIX_EXPRESSION:
        free_prefix_expression(&expr->expression.prefix_expression);
        break;
    case INFIX_EXPRESSION:
        free_infix_expression(&expr->expression.infix_expression);
        break;
    case CALL_EXPRESSION:
        free_call_expression(&expr->expression.call_expression);
        break;
    default:
        break;
    }
    safe_free((void **)&expr);
}

void free_number_literal(NumberLiteral **expression) {
    if (expression == NULL || *expression == NULL) {
        return;
    }
    NumberLiteral *expr = *expression;
    free_token(&expr->token);
    safe_free((void **)&expr);
}

void free_identifier(Identifier **expression) {
    if (expression == NULL || *expression == NULL) {
        return;
    }
    Identifier *expr = *expression;
    free_token(&expr->token);
    safe_free((void **)&expr);
}

void free_prefix_expression(PrefixExpression **expression) {
    if (expression == NULL || *expression == NULL) {
        return;
    }
    PrefixExpression *expr = *expression;
    free_expression(&expr->right);
    free_token(&expr->token);
    safe_free((void **)&expr);
}

void free_infix_expression(InfixExpression **expression) {
    if (expression == NULL || *expression == NULL) {
        return;
    }
    InfixExpression *expr = *expression;
    free_expression(&expr->left);
    free_expression(&expr->right);
    free_token(&expr->token);
    safe_free((void **)&expr);
}

void free_call_expression(CallExpression **expression) {
    if (expression == NULL || *expression == NULL) {
        return;
    }
    CallExpression *expr = *expression;
    free_token(&expr->token);
    free_expression(&expr->function);
    for (int i = 0; i < expr->num_arguments; i++) {
        free_expression(&expr->arguments[i]);
    }
    safe_free((void **)&expr->arguments);
    safe_free((void **)&expr);
}

void print_expression(FILE *out, Expression *expr) {
    assertNotNull(out);
    assertNotNull(expr);
    switch (expr->type) {
    case NUMBER_LITERAL:
        fprintf(out, "%g", expr->expression.number_literal->value);
        break;
    case IDENTIFIER:
        fprintf(out, "%s", expr->expression.identifier->value);
        break;
    case PREFIX_EXPRESSION:
        fprintf(out, "(%s", expr->expression.prefix_expression->op);
        print_expression(out, expr->expression.prefix_expression->right);
        fprintf(out, ")");
        break;
    case INFIX_EXPRESSION:
        fprintf(out, "(");
        print_expression(out, expr->expression.infix_expression->left);
        fprintf(out, " %s ", expr->expression.infix_expression->op);
        print_expression(out, expr->expression.infix_expression->right);
        fprintf(out, ")");
        break;
    case CALL_EXPRESSION:
        print_expression(out, expr->expression.call_expression->function);
        fprintf(out, "(");
        for (int i = 0; i < expr->expression.call_expression->num_arguments;
             i++) {
            print_expression(out,
                             expr->expression.call_expression->arguments[i]);
            if (i < expr->expression.call_expression->num_arguments - 1) {
                fprintf(out, ", ");
            }
        }
        fprintf(out, ")");
        break;
    default:
        break;
    }
}
