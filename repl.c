#include "repl.h"
#include "ast.h"
#include "evaluator.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include "util.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *PROMPT = ">> ";

int start(FILE *in, FILE *out) {
    assertNotNull(in);
    assertNotNull(out);
    double ans = 0.0;
    while (1) {
        fprintf(out, "%s", PROMPT);
        char *buffer = get_input(in, out);
        assertNotNull(buffer);
        Lexer *l = new_lexer(buffer, MAX_BUFFER_SIZE);
        Parser *p = new_parser(l);
        Expression *ptr = parse_expression_statement(p);
        if (ptr == NULL) {
            fprintf(out, "Invalid calculator input.\n");
        } else {
            errno = 0;
            double env_vars[NUM_ENV_VARS] = {ans, 0.0};
            ans = eval(ptr, env_vars);
            if (errno != EINVAL) {
                fprintf(out, "%.8g\n", ans);
            }
            free_expression(&ptr);
        }
        free_parser(&p);
    }
    return 0;
}

int parser_repl(FILE *in, FILE *out) {
    assertNotNull(in);
    assertNotNull(out);
    while (1) {
        fprintf(out, "%s", PROMPT);
        char *buffer = get_input(in, out);
        assertNotNull(buffer);
        Lexer *l = new_lexer(buffer, MAX_BUFFER_SIZE);
        Parser *p = new_parser(l);
        Expression *ptr = parse_expression_statement(p);
        if (ptr == NULL) {
            fprintf(out, "Illegal input.");
        } else {
            print_expression(out, ptr);
            free_expression(&ptr);
        }
        fprintf(out, "\n");
        safe_free((void **)&buffer);
        safe_free((void **)&l);
        safe_free((void **)&p);
    }
    return 0;
}

int lexer_repl(FILE *in, FILE *out) {
    assertNotNull(in);
    assertNotNull(out);
    while (1) {
        fprintf(out, "%s", PROMPT);
        char *buffer = get_input(in, out);
        assertNotNull(buffer);
        Lexer *l = new_lexer(buffer, MAX_BUFFER_SIZE);
        Token *tok;
        for (tok = lexer_next_token(l); tok->type != TOKEN_EOF;
             tok = lexer_next_token(l)) {
            fprintf(out, "{ Type: %s, Literal: '%s' }\n",
                    tokentype_names[tok->type], tok->literal);
            free_token(&tok);
        }
        safe_free((void **)&buffer);
        safe_free((void **)&l);
    }
    return 0;
}

char *get_input(FILE *in, FILE *out) {
    char *buffer = (char *)calloc(MAX_BUFFER_SIZE, sizeof(char));
    assertNotNull(buffer);
    char *result = fgets(buffer, MAX_BUFFER_SIZE, in);
    if (result == NULL) {
        safe_free((void **)&buffer);
        return NULL;
    }
    if (strncmp(result, "exit", 4) == 0) {
        safe_free((void **)&buffer);
        fprintf(out, "Exiting...\n");
        exit(0);
    }
    return buffer;
}
