#ifndef EMULATOR_MODELS_EMULATOR_H
#define EMULATOR_MODELS_EMULATOR_H

#include <stdlib.h>
#include "../utils/binary_stream.h"
#include "../utils/trie.h"
#include "instruction.h"
#include "memory_segment.h"
#include "processor.h"

#define ADDITIONAL_SIZE 129

struct exec_header
{
    unsigned char a_magic[2]; /* magic number */
    unsigned char a_flags;    /* flags */
    unsigned char a_cpu;      /* cpu id */
    unsigned char a_hdrlen;   /* length of header */
    unsigned char a_unused;   /* reserved */
    unsigned short a_version; /* version stamp (not used) */
    unsigned long a_text;     /* The length of the text segment */
    unsigned long a_data;     /* The length of the data segment */
    unsigned long a_bss;      /* The length of the bss segment */
    unsigned long a_entry;    /* The entry point */
    unsigned long a_total;    /* The total memory allocated */
    unsigned long a_syms;     /* size of symbol table */

    /* LONG VERSION */
    long a_trsize; /* text relocation size */
    long a_drsize; /* data relocation size */
    long a_tbase;  /* text relocation base */
    long a_dbase;  /* data relocation base */
};

typedef struct emulator_t
{
    binary_stream_t *stream;
    processor_t *processor;
    memory_segment_t *extra;
    unsigned char stack[STACK_SIZE];
    code_segment_t *code;
    memory_segment_t *data;
    struct exec_header header;
    FILE *file;
    unsigned char verbose;
    /// An additional string to display at the end of the instruction in
    /// verbose mode This buffer is preallocated with length ADDITIONAL_SIZE
    char *_additionals;
    unsigned char _has_additionals;
} emulator_t;

emulator_t *emulator_new(FILE *file);
void emulator_free(emulator_t *);

int emulator_load_header(emulator_t *emulator);
void emulator_prepare(emulator_t *, char **environment, int argc, char **argv);

unsigned short emulator_push_data(emulator_t *, unsigned char);

unsigned short emulator_push_bss(emulator_t *, unsigned char);

unsigned short emulator_push_data_str(emulator_t *, char *str);

unsigned char emulator_get_reg_byte(emulator_t *, char reg);
void emulator_set_reg_byte(emulator_t *, char reg, unsigned char);

unsigned short emulator_get_reg(emulator_t *, char reg);
void emulator_set_reg(emulator_t *, char reg, unsigned short);

unsigned int emulator_get_physical_addr(emulator_t *, params_t params);

unsigned char emulator_get_mem_byte(
    emulator_t *emulator, unsigned int address);
void emulator_set_mem_byte(
    emulator_t *emulator, unsigned int address, unsigned char value);

unsigned short emulator_get_mem_word(
    emulator_t *emulator, unsigned int address);
void emulator_set_mem_word(
    emulator_t *emulator, unsigned int address, unsigned short value);

unsigned char emulator_get_rm_byte(emulator_t *, params_t params);
void emulator_set_rm_byte(emulator_t *, params_t params, unsigned char);

unsigned short emulator_get_rm(emulator_t *, params_t params);
void emulator_set_rm(emulator_t *, params_t params, unsigned short);

void emulator_stack_push(emulator_t *, unsigned short value);
unsigned short emulator_stack_pop(emulator_t *);

void emulator_stack_display(emulator_t *emulator);

/// if ret is < 0, the address is not valid
/// otherwise, the address is an unsigned short in lower part of the return
int emulator_stack_get_relative_address(
    emulator_t *emulator, unsigned int address);

unsigned char emulator_stack_get_byte(
    emulator_t *emulator, unsigned short address);
unsigned short emulator_stack_get(
    emulator_t *emulator, unsigned short address);

void emulator_stack_set_byte(
    emulator_t *emulator, unsigned short address, unsigned char value);
void emulator_stack_set(
    emulator_t *emulator, unsigned short address, unsigned short value);

instruction_t *emulator_load_instruction(emulator_t *emulator);
void emulator_load_data(emulator_t *emulator);

void emulator_syscall(emulator_t *emulator);
void *emulator_data_addr(emulator_t *emulator, unsigned short address);
void *emulator_stack_addr(emulator_t *emulator, unsigned short address);

void emulator_stack_set_dword(
    emulator_t *emulator, unsigned short address, unsigned int value);

// TODO: remove this function and use proper segment selectors
int emulator_addr_in_data(emulator_t *emulator, unsigned short address);

#endif // !EMULATOR_MODELS_EMULATOR_H
