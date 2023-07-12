#include <err.h>
#include <malloc.h>
#include <stdio.h>
#include "env.h"
#include "models/emulator.h"
#include "models/memory_segment.h"
#include "utils/binary_stream.h"
#include "utils/format.h"
#include "utils/trie.h"
#include "utils/vector.h"

void execute_instructions(
    instruction_t *data, unsigned long address, emulator_t *emulator)
{
    if (emulator->verbose)
    {
        char *proc_display = processor_display(emulator->processor);
        printf("%s ", proc_display);
        free(proc_display);
        printf_instruction(
            address, data->instruction, data->instruction_len, data->display);
    }

    emulator->processor->ip += data->instruction_len;
    data->callback(emulator, data->params);
}

extern char **environ;

int main(int argc, char **argv)
{
    int res = 0;
    if (argc < 2)
    {
        errx(-1, "Usage: %s <file> <arg1> ... <argn>", argv[0]);
    }

    FILE *f = fopen(argv[1], "rb");
    if (f == NULL)
    {
        errx(-1, "Could not open file %s", argv[0]);
    }
    emulator_t *emulator = emulator_new(f);
    emulator_load_header(emulator);
    emulator_load_data(emulator);
    char *env_str = "PATH=/usr:/usr/bin";
    char *env[] = {env_str, NULL};
    emulator_prepare(emulator, env, argc - 1, argv + 1);

    printf("%s IP \n", PROCESSOR_HEADER);

    instruction_t *instruction = emulator_load_instruction(emulator);
    mem_seg_display(emulator->data, "DATA");
    for (; instruction != NULL;
         instruction = emulator_load_instruction(emulator))
    {
        execute_instructions(instruction, emulator->processor->ip, emulator);
    }

    if (!bs_finished(emulator->stream))
    {
        fprintf(stderr, "Instruction not found: ");
        for (size_t i = 0; i < emulator->stream->instruction_buffer_len; i++)
        {
            fprintf(stderr, "%02x", emulator->stream->instruction_buffer[i]);
        }
        fprintf(stderr, "\n");
        emulator_stack_display(emulator);

        res = -1;
        goto exit;
    }
    printf("===== END OF .TEXT SECTION =====\n");

exit:
    emulator_free(emulator);
    fclose(f);
    return res;
}
