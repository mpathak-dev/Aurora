/*++

Copyright (c) 2026 The Aurora Project

Module Name:

    aurasm.c

Abstract:

    Main source file for the AURORA Assembler.

Author:

    Mayank Pathak (mpathak) 6-Feb-2026

Revision History:
	
--*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define OP_NOP   0
#define OP_ADD   1
#define OP_SUB   2
#define OP_ADDI  3
#define OP_LOAD  4
#define OP_STORE 5
#define OP_JMP   6
#define OP_BEQ   7
#define OP_HALT  8
#define OP_CALL  9
#define OP_RET   10
#define OP_RETI    11
#define OP_SYSCALL 12
#define OP_CLZ     14
#define OP_CAS     16

#define MAX_LABELS 1024
#define MAX_LINES  4096

typedef struct
{
    char name[64];
    uint32_t address;
} LABEL;

LABEL labels[MAX_LABELS];
int label_count = 0;

char *lines[MAX_LINES];
int line_count = 0;

uint32_t base_address = 0;

//
// Encoding
//

uint32_t encode_r(uint32_t op, uint32_t rd, uint32_t rs1, uint32_t rs2)
{
    return (op << 26) |
           (rd << 21) |
           (rs1 << 16) |
           (rs2 << 11);
}

uint32_t encode_i(uint32_t op, uint32_t rd, uint32_t rs1, int16_t imm)
{
    return (op << 26) |
           (rd << 21) |
           (rs1 << 16) |
           ((uint16_t)imm);
}

uint32_t encode_j(uint32_t op, uint32_t addr)
{
    return (op << 26) |
           (addr & 0x03FFFFFF);
}

//
// Utils
//

void trim(char *str)
{
    while (isspace(*str))
        memmove(str, str + 1, strlen(str));

    char *end = str + strlen(str) - 1;

    while (end >= str && isspace(*end))
    {
        *end = 0;
        end--;
    }
}

int parse_register(const char *text)
{
    if (toupper(text[0]) != 'R')
    {
        printf("Invalid register: %s\n", text);
        exit(1);
    }

    return atoi(text + 1);
}

//
// Label handling
//

void add_label(const char *name, uint32_t address)
{
    if (label_count >= MAX_LABELS)
    {
        printf("Too many labels\n");
        exit(1);
    }

    strcpy(labels[label_count].name, name);
    labels[label_count].address = address;
    label_count++;
}

uint32_t find_label(const char *name)
{
    for (int i = 0; i < label_count; i++)
    {
        if (strcmp(labels[i].name, name) == 0)
            return labels[i].address;
    }

    printf("Undefined label: %s\n", name);
    exit(1);
}

int is_label(const char *line)
{
    int len = strlen(line);

    return len > 0 && line[len - 1] == ':';
}

//
// First pass: collect labels
//

void first_pass()
{
    uint32_t address = base_address;

    for (int i = 0; i < line_count; i++)
    {
        char line[256];
        strcpy(line, lines[i]);

        trim(line);

        if (line[0] == 0 || line[0] == ';')
            continue;

        if (is_label(line))
        {
            line[strlen(line) - 1] = 0;
            trim(line);

            add_label(line, address);
        }
        else
        {
            address += 4;
        }
    }
}

//
// Parse address or label
//

uint32_t parse_address(const char *text)
{
    if (isdigit(text[0]))
        return strtoul(text, NULL, 0);

    return find_label(text);
}

//
// Assemble instruction
//

uint32_t assemble_line(char *line, uint32_t current_address)
{
    char op[32] = {0};
    char a[32] = {0};
    char b[32] = {0};
    char c[32] = {0};

    // Note: sscanf might fail for zero-argument ops like SYSCALL, 
    // so we check the opcode string first.
    sscanf(line, "%s %[^,], %[^,], %s", op, a, b, c);

    for (int i = 0; op[i]; i++)
        op[i] = toupper(op[i]);

    // --- New System Opcodes ---

    if (strcmp(op, "SYSCALL") == 0)
        return encode_j(OP_SYSCALL, 0);

    if (strcmp(op, "RETI") == 0)
        return encode_j(OP_RETI, 0);

    if (strcmp(op, "CLZ") == 0)
        // Format: CLZ Rd, Rs1
        return encode_r(OP_CLZ, parse_register(a), parse_register(b), 0);

    if (strcmp(op, "CAS") == 0)
        // Format: CAS Rd, Rs1, Rs2 
        // (If [Rs1] == Rs2, then [Rs1] = Rd)
        return encode_r(OP_CAS, parse_register(a), parse_register(b), parse_register(c));

    // --- Original Opcodes ---

    if (strcmp(op, "ADD") == 0)
        return encode_r(OP_ADD, parse_register(a), parse_register(b), parse_register(c));

    if (strcmp(op, "SUB") == 0)
        return encode_r(OP_SUB, parse_register(a), parse_register(b), parse_register(c));

    if (strcmp(op, "ADDI") == 0)
        return encode_i(OP_ADDI, parse_register(a), parse_register(b), strtol(c, NULL, 0));

    if (strcmp(op, "LOAD") == 0)
        return encode_i(OP_LOAD, parse_register(a), parse_register(b), strtol(c, NULL, 0));

    if (strcmp(op, "STORE") == 0)
        return encode_i(OP_STORE, parse_register(a), parse_register(b), strtol(c, NULL, 0));

    if (strcmp(op, "BEQ") == 0) {
        uint32_t target = parse_address(c);
        int32_t offset = ((int32_t)target - (int32_t)(current_address + 4)) / 4;
        return encode_i(OP_BEQ, parse_register(a), parse_register(b), offset);
    }

    if (strcmp(op, "JMP") == 0)
        return encode_j(OP_JMP, parse_address(a));

    if (strcmp(op, "CALL") == 0)
        return encode_j(OP_CALL, parse_address(a));

    if (strcmp(op, "RET") == 0)
        return encode_j(OP_RET, 0);

    if (strcmp(op, "HALT") == 0)
        return encode_j(OP_HALT, 0);

    if (strcmp(op, "NOP") == 0)
        return encode_j(OP_NOP, 0);

    printf("Unknown instruction: %s\n", op);
    exit(1);
}

//
// Second pass: generate binary
//

void second_pass(const char *output)
{
    FILE *out = fopen(output, "wb");

    if (!out)
    {
        printf("Cannot create %s\n", output);
        exit(1);
    }

    uint32_t address = base_address;

    for (int i = 0; i < line_count; i++)
    {
        char line[256];
        strcpy(line, lines[i]);

        trim(line);

        if (line[0] == 0 || line[0] == ';')
            continue;

        if (is_label(line))
            continue;

        uint32_t instr = assemble_line(line, address);

        fwrite(&instr, sizeof(instr), 1, out);

        address += 4;
    }

    fclose(out);
}

//
// Load file into memory
//

void load_file(const char *filename)
{
    FILE *f = fopen(filename, "r");

    if (!f)
    {
        printf("Cannot open %s\n", filename);
        exit(1);
    }

    char buffer[256];

    while (fgets(buffer, sizeof(buffer), f))
    {
        lines[line_count] = strdup(buffer);
        line_count++;
    }

    fclose(f);
}

//
// Main
//

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Usage: aurasm input.asm output.bin [-addr base]\n");
        return 1;
    }

    const char *input = argv[1];
    const char *output = argv[2];

    base_address = 0;

    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "-addr") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("-addr requires value\n");
                return 1;
            }

            base_address = strtoul(argv[i + 1], NULL, 0);
            i++;
        }
    }

    load_file(input);

    first_pass();

    second_pass(output);

    printf("Assembled %s to %s (base 0x%X)\n",
        input, output, base_address);

    return 0;
}
