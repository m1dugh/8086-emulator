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

#endif // !EMULATOR_INSTRUCTIONS_IMPLEMENTATION_UTILS_H
