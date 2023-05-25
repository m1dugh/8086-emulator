#ifndef INSTRUCTION_STRING_MANIPULATION_H
#define INSTRUCTION_STRING_MANIPULATION_H

#include "../utils/binary_stream.h"

char *rep_string(binary_stream_t *data);

char *get_string_instruction(char instruction, binary_stream_t *data);

#endif // !INSTRUCTION_STRING_MANIPULATION_H
