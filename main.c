#include "repl.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("Calculator\n");
    start(stdin, stdout);
    return 0;
}
