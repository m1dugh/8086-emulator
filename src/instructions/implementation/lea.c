#include "lea.h"
#include "utils.h"

void lea_exec(emulator_t *emulator, params_t params)
{
    unsigned short effective_addr
        = emulator_get_effective_addr(emulator, params);
    emulator_set_reg(emulator, params.reg, effective_addr);
}
