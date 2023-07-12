#include <unistd.h>
#include "syscalls.h"

short syscall_write(emulator_t *emulator)
{
    unsigned short fd
        = emulator_stack_get(emulator, emulator->processor->bx + 4);
    unsigned short len
        = emulator_stack_get(emulator, emulator->processor->bx + 6);
    unsigned short address = emulator->processor->dx;
    char *str = emulator_data_addr(emulator, address);
    if (emulator->verbose)
    {
        printf("<write(%d, 0x%04x, %d)", fd, address, len);
    }
    unsigned short res = write(fd, str, len);
    emulator_stack_set(emulator, emulator->processor->bx + 2, res);
    if (emulator->verbose)
    {
        printf(" => %d>\n", res);
    }
    return res;
}

short syscall_exit(emulator_t *emulator)
{
    unsigned short ret_code
        = emulator_stack_get(emulator, emulator->processor->bx + 4);
    if (emulator->verbose)
    {
        printf("<exit (%d)>\n", ret_code);
    }
    exit(ret_code);
}
