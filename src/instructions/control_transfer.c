#include "control_transfer.h"
#include "utils.h"
#include <malloc.h>

char *jnb(binary_stream_t *data) {
    return format_byte_displacement("jnb", data);
}

char *jne(binary_stream_t *data) {
    return format_byte_displacement("jne", data);
}

char *call_direct_seg(binary_stream_t *data) {
    return format_word_displacement("call", data);
}