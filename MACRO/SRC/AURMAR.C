/*++

Copyright (c) 2026 The Aurora Project

Module Name:

    aurmar.c

Abstract:

    Main source file for the AURORA MACRO Assembler initialization subcomponent.

Author:

    Mayank Pathak (mpathak) 6-Feb-2026

Revision History:

--*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_MAX 256

#include <ctype.h>

void expand_macro(const char *line, FILE *out)
{
    char r1[16], r2[16];
    char immhex[32];
    int imm;

    // Skip leading spaces/tabs
    while (*line == ' ' || *line == '\t') line++;

    // Case 1: MOVL #<hex>h, <reg>
    if (sscanf(line, "MOVL #%[0-9A-Fa-f]h, %15s", immhex, r1) == 2) {
        fprintf(out, "ADDI %s, R0, 0x%s\n", r1, immhex);
        return;
    }

    // Case 2: MOVL #<dec>, <reg>
    if (sscanf(line, "MOVL #%d, %15s", &imm, r1) == 2) {
        fprintf(out, "ADDI %s, R0, %d\n", r1, imm);
        return;
    }

    // Case 3: MOVL <reg>, 0xADDR  --> STORE <reg>, R0, 0xADDR
    if (sscanf(line, "MOVL %15[^,], %31s", r1, immhex) == 2) {
        // Detect if second operand is hex (0x...) or decimal
        if ((immhex[0] == '0' && (immhex[1] == 'x' || immhex[1] == 'X')) || isdigit(immhex[0])) {
            fprintf(out, "STORE %s, R0, %s\n", r1, immhex);
            return;
        }
    }

    // Otherwise, copy line as-is
    fprintf(out, "%s\n", line);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    FILE *in = fopen(input_file, "r");
    if (!in) {
        perror("fopen input");
        return 1;
    }

    FILE *out = fopen("OUT.ASM", "w");
    if (!out) {
        perror("fopen OUT.ASM");
        fclose(in);
        return 1;
    }

    char line[LINE_MAX];
    while (fgets(line, sizeof(line), in)) {
        size_t len = strlen(line);
        if (len && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[len-1] = '\0';
        expand_macro(line, out);
    }

    fclose(in);
    fclose(out);

    // Build the command dynamically
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "AURASM OUT.ASM %s", output_file);

    // Append extra args if any
    for (int i = 3; i < argc; i++) {
        strncat(cmd, " ", sizeof(cmd) - strlen(cmd) - 1);
        strncat(cmd, argv[i], sizeof(cmd) - strlen(cmd) - 1);
    }

    system(cmd);
    system("rm -f OUT.ASM");

    printf("Done.\n");
    return 0;
}
