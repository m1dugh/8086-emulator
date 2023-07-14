#include "xchg.h"

void xchg_reg_exec(emulator_t *emulator, params_t params)
{

    // nop
    if (params.reg == AX)
        return;

    unsigned short reg_val = emulator_get_reg(emulator, params.reg);
    unsigned short ax_val = emulator_get_reg(emulator, AX);
    emulator_set_reg(emulator, params.reg, ax_val);
    emulator_set_reg(emulator, AX, reg_val);
}
