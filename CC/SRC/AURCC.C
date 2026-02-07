/*++

Copyright (c) 2026 The Aurora Project

Module Name:

    aurcc.c

Abstract:

    Main source file for the AURORA C Compiler.

Author:

    Mayank Pathak (mpathak) 7-Feb-2026

Revision History:

--*/

#include "AUR32.H"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 512
#define MAX_NAME 128

//
// Define global static data.
//

ULONG CcMajorVersion = 1;
ULONG CcMinorVersion = 0;

char EntryName[MAX_NAME] = {0};
char CurrentArgs[16][MAX_NAME];
int CurrentArgCount = 0;
int parsingArgs = 0;

//
// Trim whitespace
//

char* trim(char* s)
{
    while (isspace(*s)) s++;

    if (*s == 0)
        return s;

    char* end = s + strlen(s) - 1;

    while (end > s && isspace(*end))
        *end-- = 0;

    return s;
}

//
// Check if line starts function definition
//

int is_function(char* line, char* outName)
{
    char type[MAX_NAME];
    char name[MAX_NAME];

    if (sscanf(line, "%s %[^ (](", type, name) == 2)
    {
        strcpy(outName, name);
        return 1;
    }

    return 0;
}

int is_valid_identifier(const char* s)
{
    if (!isalpha(*s) && *s != '_')
        return 0;

    s++;

    while (*s)
    {
        if (!isalnum(*s) && *s != '_')
            return 0;
        s++;
    }

    return 1;
}

int parse_arguments(const char* argStr, char args[4][MAX_NAME])
{
    int count = 0;
    const char* p = argStr;

    while (*p && count < 4)
    {
        while (isspace(*p)) p++;

        if (*p == 0)
            break;

        int i = 0;

        while (*p && *p != ',' && *p != ')' && i < MAX_NAME - 1)
        {
            args[count][i++] = *p++;
        }

        args[count][i] = 0;
        trim(args[count]);

        count++;

        while (*p && *p != ',')
        {
            if (*p == ')')
                break;
            p++;
        }

        if (*p == ',')
            p++;
    }

    return count;
}

int find_argument(const char* name)
{
    for (int i = 0; i < CurrentArgCount; i++)
    {
        if (strcmp(CurrentArgs[i], name) == 0)
            return i;
    }

    return -1;
}

//
// Compile C to ASM
//

void compile_file(const char* input)
{
    FILE* in = fopen(input, "r");
    FILE* out = fopen("OUT1.ASM", "w");

    if (!in || !out)
    {
        printf("error: file open failed\n");
        exit(1);
    }

    if (EntryName[0])
        fprintf(out, "JMP _%s\n\n", EntryName);

    char line[MAX_LINE];
    char pendingFunc[MAX_NAME] = {0};

    int haveType = 0;
    int haveName = 0;

    int inFunction = 0;
    int inAsm = 0;
    int isEntry = 0;
	int inComment = 0;
	int currentFuncIsVoid = 0;

    while (fgets(line, sizeof(line), in))
    {
        char* t = trim(line);
        
        //
        // Handle block comments /* */
        //
        
        // already inside comment
        if (inComment)
        {
            char* end = strstr(t, "*/");
        
            if (end)
            {
                inComment = 0;
                t = trim(end + 2);
        
                if (*t == 0)
                    continue;
            }
            else
            {
                continue;
            }
        }
        
        // comment starts here
        char* start = strstr(t, "/*");
        
        if (start)
        {
            char* end = strstr(start + 2, "*/");
        
            if (end)
            {
                // comment starts and ends on same line
                *start = 0;
        
                char* after = trim(end + 2);
        
                char combined[MAX_LINE];
                snprintf(combined, sizeof(combined), "%s %s", t, after);
        
                strcpy(line, combined);
                t = trim(line);
        
                if (*t == 0)
                    continue;
            }
            else
            {
                inComment = 1;
                *start = 0;
                t = trim(t);
        
                if (*t == 0)
                    continue;
            }
        }

		//
		// Handle single-line comments //
		//
		
		char* slash = strstr(t, "//");
		
		if (slash)
		{
		    *slash = 0;
		    t = trim(t);
		
		    if (*t == 0)
		        continue;
		}
        
        if (*t == 0)
            continue;

        //
        // Detect type line
        //

        if (!inFunction && !haveType)
        {
            if (!strcmp(t, "VOID"))
            {
                haveType = 1;
                currentFuncIsVoid = 1;
                continue;
            }
        
            if (!strcmp(t, "INT") ||
                !strcmp(t, "CHAR") ||
                !strcmp(t, "ULONG"))
            {
                haveType = 1;
                currentFuncIsVoid = 0;
                continue;
            }
        }

        //
        // Detect function name line
        //

       	if (haveType && !haveName)
       	{
       	    char name[MAX_NAME];
       	
       	    if (sscanf(t, "%[^ (]", name) == 1)
       	    {
       	        strcpy(pendingFunc, name);
       	        haveName = 1;
       	
       	        CurrentArgCount = 0;
       	
       	        if (strchr(t, '('))
       	        {
       	            parsingArgs = 1;
       	        }
       	
       	        continue;
       	    }
       	}

       	//
       	// Parse arguments across multiple lines
       	//
       	
       	if (parsingArgs)
       	{
       	    if (strchr(t, ')'))
       	    {
       	        parsingArgs = 0;
       	        continue;
       	    }
       	
       	    char type[MAX_NAME];
       	    char argName[MAX_NAME];
       	
       	    if (sscanf(t, "%s %s", type, argName) == 2)
       	    {
       	        char* end = strchr(argName, ',');
       	        if (end) *end = 0;
       	
       	        strcpy(CurrentArgs[CurrentArgCount], argName);
       	        CurrentArgCount++;
       	    }
       	
       	    continue;
       	}

        //
        // Detect function body start
        //

        if (haveType && haveName && strchr(t, '{'))
        {
            inFunction = 1;
            isEntry = (strcmp(pendingFunc, EntryName) == 0);
        
            fprintf(out, "_%s:\n", pendingFunc);
        
            haveType = 0;
            haveName = 0;
        
            continue;
        }

        //
        // Inline ASM start
        //

        if (strstr(t, "__asm"))
        {
            inAsm = 1;
            continue;
        }

        //
        // Inline ASM end
        //

        if (inAsm && strchr(t, '}'))
        {
            inAsm = 0;
            continue;
        }

        //
        // Emit ASM lines
        //

        if (inAsm)
        {
            fprintf(out, "    %s\n", t);
            continue;
        }

        //
        // Handle return keyword
        //
        
        if (inFunction && strncmp(t, "return", 6) == 0)
        {
            if (currentFuncIsVoid)
            {
                printf("error: VOID function cannot return value\n");
                exit(1);
            }
        
            char value[MAX_NAME];
        
            if (sscanf(t, "return %[^;];", value) == 1)
            {
                fprintf(out, "    MOVL #%s, R15\n", value);
                fprintf(out, "    RET\n\n");
            }
        
            continue;
        }

        //
        // Handle *(TYPE*)ADDRESS = VALUE;
        //
        
        {
            char type[MAX_NAME];
            char addr[MAX_NAME];
            char value[MAX_NAME];
        
            if (sscanf(t, "*(%[^*]*)%[^=]=%[^;];", type, addr, value) == 3)
            {
                char* type_t = trim(type);
                char* addr_t = trim(addr);
                char* value_t = trim(value);
        
                int argIndex = find_argument(value_t);
        
                if (argIndex >= 0)
                {
                    fprintf(out, "    MOVL R%d, %s\n", argIndex + 1, addr_t);
                }
                else
                {
                    fprintf(out, "    MOVL #%s, R5\n", value_t);
                    fprintf(out, "    MOVL R5, %s\n", addr_t);
                }
        
                continue;
            }
        }
        

		if (inFunction)
		{
		    char name[MAX_NAME];
		    char argStr[MAX_LINE];
		
		    //
		    // Case 1: WITH arguments (check FIRST)
		    //
		    if (sscanf(t, "%127[a-zA-Z0-9_](%[^)]);", name, argStr) == 2)
		    {
		        if (is_valid_identifier(name))
		        {
		            char args[4][MAX_NAME];
		            int count = parse_arguments(argStr, args);
		
		           	for (int i = 0; i < count; i++)
		           	{
		           	    char* a = args[i];
		           	    // check if it's a char literal like 'a'
		           	    if (strlen(a) == 3 && a[0] == '\'' && a[2] == '\'')
		           	    {
		           	        fprintf(out, "    MOVL #%d, R%d\n", (unsigned char)a[1], i + 1);
		           	    }
		           	    else
		           	    {
		           	        int argIndex = find_argument(a);
		           	        if (argIndex >= 0)
		           	            fprintf(out, "    MOVL R%d, R%d\n", argIndex + 1, i + 1);
		           	        else
		           	            fprintf(out, "    MOVL #%s, R%d\n", a, i + 1);
		           	    }
		           	}
		
		            fprintf(out, "    CALL _%s\n", name);
		            continue;
		        }
		    }
		
		    //
		    // Case 2: NO arguments
		    //
		    if (sscanf(t, "%127[a-zA-Z0-9_]();", name) == 1)
		    {
		        if (is_valid_identifier(name))
		        {
		            fprintf(out, "    CALL _%s\n", name);
		            continue;
		        }
		    }
		}
		

        //
        // Function end
        //

        if (inFunction && strchr(t, '}'))
        {
            if (!isEntry && currentFuncIsVoid)
                fprintf(out, "    RET\n");
        
            fprintf(out, "\n");
        
            inFunction = 0;
            continue;
        }
    }

    fclose(in);
    fclose(out);
}

//
// Parse command line
//

void parse_args(int argc, char** argv, char** inputFile)
{
    int i;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-entry") == 0)
        {
            if (i + 1 < argc)
            {
                strcpy(EntryName, argv[i + 1]);
                i++;
            }
        }
        else
        {
            *inputFile = argv[i];
        }
    }
}

//
// Main
//

int main(int argc, char** argv)
{
    printf("AURORA C COMPILER VERSION %ld.%ld\n",
        CcMajorVersion,
        CcMinorVersion);

    if (argc < 2)
    {
        printf("usage: aurcc input.c -entry Entry\n");
        return 1;
    }

    char* inputFile = NULL;

    parse_args(argc, argv, &inputFile);

    if (!inputFile)
    {
        printf("error: no input file\n");
        return 1;
    }

    compile_file(inputFile);

	system("AURMAR OUT1.ASM OUT.BIN -addr 0x00");
	system("rm -f OUT1.ASM");

    printf("Done\n");

    return 0;
}
