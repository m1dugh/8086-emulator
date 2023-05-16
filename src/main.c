#include <stdio.h>
#include <malloc.h>
#include <err.h>
#include "utils/binary_stream.h"

struct exec_header {
    unsigned char a_magic[2];   /* magic number */
    unsigned char a_flags;      /* flags */
    unsigned char a_cpu;        /* cpu id */
    unsigned char a_hdrlen;     /* length of header */
    unsigned char a_unused;     /* reserved */
    unsigned short a_version;   /* version stamp (not used) */
    long a_text;                /* The length of the text segment */
    long a_data;                /* The length of the data segment */
    long a_bss;                 /* The length of the bss segment */
    long a_entry;               /* The entry point */
    long a_total;               /* The total memory allocated */
    long a_syms;                /* size of symbol table */

    /* LONG VERSION */
    long a_trsize;              /* text relocation size */
    long a_drsize;              /* data relocation size */
    long a_tbase;               /* text relocation base */
    long a_dbase;               /* data relocation base */
};

long to_long(char *buffer) {
    long res = buffer[3];
    for (int i = 2; i >= 0; i--)
        res = (res << 8) + buffer[i];

    return res;
}

void print_byte(char c) {
    char mask = 0x80;
    for(int i = 0; i < 8; i++, mask >>= 1) {
        if (mask & c) {
            printf("1");
        } else {
            printf("0");
        }
    }
}
void print_hex(char c) {
    printf("%x", (c & 0xf0) >> 4);
    printf("%x", (c & 0xf));
}

short to_short(char *buffer) {
    short res = buffer[1];
    res = (res << 1) + buffer[0];
    return res;
}

int read_header(FILE *stream, struct exec_header *header) {
    char c;
    if ((c = fgetc(stream)) == -1) {
        return -1;
    }
    header->a_magic[0] = (unsigned char) c;
    if ((c = fgetc(stream)) == -1) {
        return -1;
    }
    header->a_magic[1] = (unsigned char) c;

    if ((c = fgetc(stream)) == -1) {
        return -1;
    }
    header->a_flags = (unsigned char) c;

    if ((c = fgetc(stream)) == -1) {
        return -1;
    }
    header->a_cpu = (unsigned char) c;

    if ((c = fgetc(stream)) == -1) {
        return -1;
    }
    header->a_hdrlen = (unsigned char) c;

    unsigned char effective_len = header->a_hdrlen - 5;
    char *buffer = (char *) malloc(effective_len);
    for(unsigned char i = 0; i < effective_len; i++) {
        if ((c = fgetc(stream)) == -1) {
            return -1;
        }
        buffer[i] = c;
    }

    size_t current_index = 0;
    header->a_unused = (unsigned char) buffer[current_index++];
    header->a_version = to_short(buffer + current_index);
    current_index += 2;
    header->a_text = to_long(buffer + current_index);
    current_index += 4;
    // TODO: implement full header.
    
    fseek(stream, header->a_hdrlen, SEEK_SET);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        errx(-1, "Usage: %s <file>", argv[0]);
    }

    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        errx(-1, "Could not open file %s", argv[0]);
    }
    struct exec_header header;
    if(read_header(f, &header) != 0) {
        errx(-1, "Could not read header.");
    }

    binary_stream_t *stream = bs_new(f, header.a_text);

    char res;
    for (int i = 0; i < 10; i++) {
        bs_next(stream, 8, &res);
        print_byte(res);
        printf(":");
        print_hex(res);
        printf("\n");
    }

    fclose(f);
    return 0;
}
