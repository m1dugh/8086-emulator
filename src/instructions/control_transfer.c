#include "control_transfer.h"
#include "utils.h"
#include <malloc.h>

char *jnb(binary_stream_t *data) {
    return format_byte_displacement("jnb", data);
}

char *jne(binary_stream_t *data) {
    return format_byte_displacement("jne", data);
}

char *jnl(binary_stream_t *data) {
    return format_byte_displacement("jnl", data);
}

char *jnle(binary_stream_t *data) {
    return format_byte_displacement("jnle", data);
}

char *je(binary_stream_t *data) {
    return format_byte_displacement("je", data);
}

char *jl(binary_stream_t *data) {
    return format_byte_displacement("jl", data);
}

char *call_direct_seg(binary_stream_t *data) {
    return format_word_displacement("call", data);
}

char *jmp_direct_seg(binary_stream_t *data) {
    return format_word_displacement("jmp", data);
}

char *jmp_direct_seg_short(binary_stream_t *data) {
    return format_byte_displacement("jmp short", data);
}
