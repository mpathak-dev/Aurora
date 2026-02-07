/*++

Copyright (c) 2026 The Aurora Project

Module Name:

    aurbli.c

Abstract:

    Main source file for the AURORA BLISS Compiler.

Author:

    Mayank Pathak (mpathak) 6-Feb-2026

Revision History:
	
--*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LINE_SIZE 256

char* trim(char* s) {
    while (isspace(*s)) s++;
    if (*s == 0) return s;
    char* end = s + strlen(s) - 1;
    while (end > s && isspace(*end)) *end-- = 0;
    return s;
}

void strupper(char* s) {
    for (int i=0; s[i]; i++) s[i] = toupper(s[i]);
}

char* preprocess_line(char* line) {
    char* p = strchr(line, '!');
    if (p) *p = 0;
    p = strstr(line, "/*");
    if (p) *p = 0;
    return trim(line);
}

int parse_hex(char* token) {
    if(strncmp(token, "%X'", 3) != 0) {
        fprintf(stderr, "Invalid hex literal: %s\n", token);
        exit(1);
    }
    return (int)strtol(token + 3, NULL, 16);
}

void compile_uplit(FILE* out, char* line) {
    char clean[128]; int j=0;
    for(int i=0; line[i] && j<127; i++) if(!isspace(line[i])) clean[j++] = line[i];
    clean[j] = 0;

    char addr[64], val[64];
    if (sscanf(clean, "UPLIT(%63[^)])=%63[^;];", addr, val) != 2 &&
        sscanf(clean, "UPLIT(%63[^)])=%63[^;]", addr, val) != 2) {
        fprintf(stderr, "Syntax error in UPLIT line: %s\n", line);
        exit(1);
    }

    fprintf(out, "\tADDI R1, R0, 0x%X\n", parse_hex(val));
    fprintf(out, "\tSTORE R1, R0, 0x%X\n\n", parse_hex(addr));
}

void compile_return(FILE* out, char* line, int is_entry) {
    int val = 0;
    // Look for return value in the string
    char* p = strstr(line, "RETURN");
    if (p) sscanf(p + 6, "%d", &val);

    fprintf(out, "\tADDI R15, R0, %d\n", val);
    if (is_entry) {
        fprintf(out, "\tHALT\n\n");
    } else {
        fprintf(out, "\tRET\n\n");
    }
}

void compile_call(FILE* out, char* line) {
    char name[64];
    // Simple parse: find the name before the opening parenthesis
    int i = 0;
    while (line[i] && isspace(line[i])) i++; // skip lead space
    int j = 0;
    while (line[i] && line[i] != '(' && !isspace(line[i]) && j < 63) {
        name[j++] = line[i++];
    }
    name[j] = 0;

    if (strlen(name) > 0) {
        fprintf(out, "\tCALL %s_routine\n\n", name);
    }
}

// New function to handle direct memory assignments like %X'400' = %X'41';
void compile_assignment(FILE* out, char* line) {
    char addr_str[64], val_str[64];
    
    // Simple parse for %X'...' = %X'...';
    // We look for the first %X and the second %X
    char* first_x = strstr(line, "%X'");
    char* equals = strchr(line, '=');
    char* second_x = (equals) ? strstr(equals, "%X'") : NULL;

    if (first_x && equals && second_x) {
        // Extract the hex strings
        sscanf(first_x, "%%X'%63[^']'", addr_str);
        sscanf(second_x, "%%X'%63[^']'", val_str);

        int addr = (int)strtol(addr_str, NULL, 16);
        int val = (int)strtol(val_str, NULL, 16);

        fprintf(out, "\tADDI R1, R0, 0x%X\n", val);
        fprintf(out, "\tSTORE R1, R0, 0x%X\n\n", addr);
    } else {
        fprintf(stderr, "Syntax error in assignment: %s\n", line);
    }
}

int main(int argc, char** argv) {
    if (argc < 2) { printf("Usage: %s input.b32\n", argv[0]); return 1; }

    FILE* in = fopen(argv[1], "r");
    if (!in) { perror("fopen"); return 1; }
    FILE* out = fopen("OUT.ASM", "w");
    if (!out) { perror("fopen"); return 1; }

    fprintf(out, ";++\n;\n; GENERATED AURORA ASM FROM BLISS\n;\n;--\n\n");

    char line[LINE_SIZE];
    char entry_routine[64] = {0};
    int in_routine = 0;
    char current_routine[64] = {0};
    int jmp_emitted = 0;

    while(fgets(line, LINE_SIZE, in)) {
        char* tline = preprocess_line(line);
        if(strlen(tline) == 0) continue;

        char upper[LINE_SIZE]; 
        strncpy(upper, tline, LINE_SIZE); 
        strupper(upper);

        // --- MODULE / ENTRY ROUTINE PARSING ---
        if (strncmp(upper, "MODULE", 6) == 0) {
            char* main_pos = strstr(upper, "MAIN"); 
            if (main_pos) {
                main_pos = strchr(main_pos, '='); 
                if (main_pos) {
                    main_pos++; 
                    while (*main_pos && (isspace(*main_pos) || *main_pos == '(')) main_pos++;
                    
                    int i = 0;
                    while (main_pos[i] && !isspace(main_pos[i]) && 
                           main_pos[i] != ')' && main_pos[i] != '=' && main_pos[i] != ',') {
                        entry_routine[i] = main_pos[i];
                        i++;
                        if (i >= sizeof(entry_routine) - 1) break;
                    }
                    entry_routine[i] = 0;

                    // Emit JMP to entry routine at top of file only once
                    if(strlen(entry_routine) > 0 && !jmp_emitted) {
                        fprintf(out, "\tJMP %s\n\n", entry_routine);
                        jmp_emitted = 1;
                    }
                }
            }
            continue;
        }

        // --- ROUTINE PARSING ---
        if (strncmp(upper, "ROUTINE", 7) == 0) {
            char* name_start = tline + 7;
            while (isspace(*name_start)) name_start++;
        
            int i = 0;
            while (name_start[i] && !isspace(name_start[i]) && name_start[i] != '=') {
                current_routine[i] = name_start[i];
                i++;
                if (i >= sizeof(current_routine) - 1) break;
            }
            current_routine[i] = 0;
        
            in_routine = 1;
        
            // Case-insensitive comparison
            char temp_curr[64], temp_entry[64];
            strcpy(temp_curr, current_routine); strupper(temp_curr);
            strcpy(temp_entry, entry_routine);  strupper(temp_entry);
        
            if (strlen(temp_entry) > 0 && strcmp(temp_curr, temp_entry) == 0) {
                fprintf(out, "%s:\n", current_routine); 
            } else {
                fprintf(out, "%s_routine:\n", current_routine);
            }
            continue;
        }

        if(in_routine) {
        	if(strstr(upper, "%X") && strchr(upper, '=')) {
                compile_assignment(out, tline);
            }
			else if(strstr(upper,"RETURN")) {
                // Determine if we should emit HALT or RET
                char temp_curr[64], temp_entry[64];
                strcpy(temp_curr, current_routine); strupper(temp_curr);
                strcpy(temp_entry, entry_routine);  strupper(temp_entry);

                // Extract return value to R15
                int val = 0;
                sscanf(upper, "RETURN %d", &val);
                fprintf(out, "\tADDI R15, R0, %d\n", val);

                if (strcmp(temp_curr, temp_entry) == 0) {
                    fprintf(out, "\tHALT\n\n");
                } else {
                    fprintf(out, "\tRET\n\n");
                }
            }
            else if(strstr(upper,"END")) in_routine = 0;
			else {
		        // Detect BLISS function call: IDENTIFIER(...)
		        char name[64];
		        int i = 0;

		        while (isspace(tline[i])) i++;

		        int j = 0;
		        while (tline[i] &&
		               tline[i] != '(' &&
		               !isspace(tline[i]) &&
		               tline[i] != ';' &&
		               j < 63)
		        {
		            name[j++] = tline[i++];
		        }

		        name[j] = 0;

		        if (tline[i] == '(' && strlen(name) > 0) {
		            fprintf(out, "\tCALL %s_routine\n\n", name);
		        }
		    }
        }
    }

    fclose(in);
    fclose(out);
    system("AURASM OUT.ASM OUT.BIN");
    system("rm OUT.ASM");
    printf("Compilation done!\n");
    return 0;
}

