#include "processor_control.h"

#include <malloc.h>
#include <stdio.h>

char *hlt() {
    char *res = malloc(4);
    snprintf(res, 4, "hlt");
    return res;
}
