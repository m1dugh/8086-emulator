#include "arithmetic.h"
#include "utils.h"

char *add_rm_with_reg(binary_stream_t *data) {
    return format_dw_rm_to_reg("add", data);
}
