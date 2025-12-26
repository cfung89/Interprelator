#ifndef REPL_H
#define REPL_H

#include <stdio.h>

#define MAX_BUFFER_SIZE 100

extern const char *PROMPT;

int start(FILE *in, FILE *out);
int parser_repl(FILE *in, FILE *out);
int lexer_repl(FILE *in, FILE *out);

char *get_input(FILE *in, FILE *out);

#endif
