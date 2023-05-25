#include "processor_control.h"

#include <malloc.h>
#include <stdio.h>

#define CREATE_STRING(format)

char *hlt() {
    char *res = malloc(4);
    snprintf(res, 4, "hlt");
    return res;
}

char *cld() {
    char *res = malloc(4);
    snprintf(res, 4, "cld");
    return res;
}

char *std_instruction() {
    char *res = malloc(4);
    snprintf(res, 4, "std");
    return res;
}
