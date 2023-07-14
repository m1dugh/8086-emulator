#include <time.h>
#include <unistd.h>
#include "syscalls.h"

unsigned short get_arg(emulator_t *emulator, unsigned short offset)
{
    return emulator_stack_get(emulator, emulator->processor->bx + offset);
}

void set_res(emulator_t *emulator, unsigned short offset, unsigned short value)
{
    emulator_stack_set(emulator, emulator->processor->bx + offset, value);
}

short syscall_write(emulator_t *emulator)
{
    unsigned short fd
        = emulator_stack_get(emulator, emulator->processor->bx + 4);
    unsigned short len
        = emulator_stack_get(emulator, emulator->processor->bx + 6);
    unsigned short address
        = emulator_stack_get(emulator, emulator->processor->bx + 10);

    char *str;

    // TODO: remove and use segment selection instead
    if (emulator_addr_in_data(emulator, address))
    {
        str = emulator_data_addr(emulator, address);
    }
    else
    {
        str = emulator_stack_addr(emulator, address);
    }

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

unsigned int syscall_time(emulator_t *emulator)
{
    unsigned int seconds = (unsigned int)time(NULL);
    unsigned short bx = emulator->processor->bx;
    emulator_stack_set_dword(emulator, bx + 10, seconds);
    return seconds;
}

void syscall_ioctl(emulator_t *emulator)
{
    unsigned short fd = get_arg(emulator, 4);
    unsigned short request = get_arg(emulator, 8);
    unsigned short address = get_arg(emulator, 18);
    if (emulator->verbose)
    {
        printf("<ioctl (%d, 0x%04x, 0x%04x)>\n", fd, request, address);
    }
    set_res(emulator, 2, -EINVAL);
}

void syscall_brk(emulator_t *emulator)
{
    unsigned short address = get_arg(emulator, 10);

    if (emulator->verbose)
        printf("<brk (%04x) => ", address);

    if (address < mem_seg_size(emulator->data))
    {
        printf("high address: %04x, requested: %04x",
            mem_seg_high_addr(emulator->data), address);
        set_res(emulator, 2, -ENOMEM);
        if (emulator->verbose)
            printf("ENOMEM>\n");
    }
    else if (mem_seg_expand(emulator->data, address) != 0)
    {
        set_res(emulator, 2, -ENOMEM);
        if (emulator->verbose)
            printf("ENOMEM>\n");
    }
    else
    {
        set_res(emulator, 2, 0);
        set_res(emulator, 18, address);
        if (emulator->verbose)
            printf("0>\n");
    }
}
