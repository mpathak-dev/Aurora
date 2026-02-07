/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    aemu.c

Abstract:

    Main source file for initialization stubs.

Author:

    Mayank Pathak (mpathak) 6-Feb-2026

Revision History:

	Mayank Pathak    7 February 2026
		Added CPU type option.

--*/

#include "AUR32.H"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int
main (
    int argc,
    char *argv[]
)
{
    LOADER_BLOCK EmuLoaderBlock;

    //
    // Initialize loader block to zero.
    //
    
    memset(&EmuLoaderBlock, 0, sizeof(LOADER_BLOCK));

    //
    // Default values.
    //
    
    EmuLoaderBlock.LoadAddress = 0x1000;
    EmuLoaderBlock.ProgramString = NULL;
    EmuLoaderBlock.LoadTestProgram = 0;
    EmuLoaderBlock.MachineType = TYPE_AUR32;

    //
    // Parse arguments.
    //
    
    for (int i = 1; i < argc; i++)
    {
        //
        // -bin filename
        //
        
        if (strcmp(argv[i], "-bin") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("ERROR: -bin requires filename\n");
                return 1;
            }

            EmuLoaderBlock.ProgramString = argv[i + 1];
            i++;
        }

        //
        // -addr address
        //
        
        else if (strcmp(argv[i], "-addr") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("ERROR: -addr requires value\n");
                return 1;
            }

            //
            // strtoul handles hex (0x...), decimal, etc.
            //
            
            EmuLoaderBlock.LoadAddress =
                (UINT)strtoul(argv[i + 1], NULL, 0);

            i++;
        }

		//
        // -cpu aur32 | aur128
        //
        
        else if (strcmp(argv[i], "-cpu") == 0)
        {
            if (i + 1 >= argc)
            {
                printf("ERROR: -cpu requires value (aur32 or aur128)\n");
                return 1;
            }

            if (strcmp(argv[i + 1], "aur32") == 0)
            {
                EmuLoaderBlock.MachineType = TYPE_AUR32;
            }
            else if (strcmp(argv[i + 1], "aur128") == 0)
            {
                EmuLoaderBlock.MachineType = TYPE_AUR128;
            }
            else
            {
                printf("ERROR: unknown CPU type '%s'\n", argv[i + 1]);
                printf("Valid types: aur32, aur128\n");
                return 1;
            }

            i++;
        }

        //
        // Test mode
        //
        
        else if (strcmp(argv[i], "-test") == 0)
        {
            EmuLoaderBlock.LoadTestProgram = 1;
        }

        else
        {
            printf("Unknown argument: %s\n", argv[i]);
            return 1;
        }
    }

    //
    // If no binary specified, fall back to test program.
    //
    
    if (EmuLoaderBlock.ProgramString == NULL &&
        EmuLoaderBlock.LoadTestProgram == 0)
    {
        EmuLoaderBlock.LoadTestProgram = 1;
    }

    //
    // Start emulator.
    //
    
    EiSystemStartup(&EmuLoaderBlock);

    return 0;
}
