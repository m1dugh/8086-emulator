#include "processor_control.h"

#include <malloc.h>

char *hlt() {
    char *res = malloc(4);
    res = "hlt";
    return res;
}
