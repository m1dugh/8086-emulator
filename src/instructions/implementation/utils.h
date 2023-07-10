#ifndef EMULATOR_INSTRUCTIONS_IMPLEMENTATION_UTILS_H
#define EMULATOR_INSTRUCTIONS_IMPLEMENTATION_UTILS_H

unsigned char get_parity(unsigned short value);
unsigned char get_carry(unsigned int value);
unsigned char get_sign(unsigned short value);
unsigned char get_zero(unsigned short value);

unsigned char get_parity_byte(unsigned char value);
unsigned char get_carry_byte(unsigned short value);
unsigned char get_sign_byte(unsigned char value);
unsigned char get_zero_byte(unsigned char value);

unsigned char get_overflow_plus_byte(unsigned char v1, unsigned char v2);
unsigned char get_overflow_minus_byte(unsigned char v1, unsigned char v2);

unsigned char get_overflow_plus(unsigned short v1, unsigned short v2);
unsigned char get_overflow_minus(unsigned short v1, unsigned short v2);

#endif // !EMULATOR_INSTRUCTIONS_IMPLEMENTATION_UTILS_H
