#ifndef UTILS_FORMAT_H
#define UTILS_FORMAT_H

#include <stddef.h>

void print_byte(char c);
void print_hex(char c);

int printf_instruction(unsigned short address, char *buffer, size_t buffer_len,
    char *instruction);

#endif // !UTILS_FORMAT_H
