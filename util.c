#include "util.h"
#include <assert.h>
#include <stdlib.h>

void safe_free(void **ptr) {
    if (ptr == NULL || *ptr == NULL) {
        return;
    }
    free(*ptr);
    *ptr = NULL;
}

void assertNotNull(void *val) { assert(val != NULL); }
