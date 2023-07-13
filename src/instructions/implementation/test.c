#include "test.h"
#include "utils.h"

void test_immediate_rm_exec(emulator_t *emulator, params_t params)
{
    emulator->processor->flags.o = 0;
    emulator->processor->flags.c = 0;
    if (params.w)
    {
        unsigned short val1 = emulator_get_rm(emulator, params);
        unsigned short val2 = params.data;
        unsigned short res = val1 & val2;
        emulator->processor->flags.s = get_sign(res);
        emulator->processor->flags.z = get_zero(res);
        emulator->processor->flags.p = get_parity(res);
    }
    else
    {
        unsigned char val1 = emulator_get_rm_byte(emulator, params);
        unsigned char val2 = params.data_low;
        unsigned char res = (unsigned char)val1 & val2;
        emulator->processor->flags.s = get_sign_byte(res);
        emulator->processor->flags.z = get_zero_byte(res);
        emulator->processor->flags.p = get_parity_byte(res);
    }
}
