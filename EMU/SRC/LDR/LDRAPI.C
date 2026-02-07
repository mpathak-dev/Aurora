/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    ldrapi.c

Abstract:

    This module implements loader APIs for binaries.

Author:

    Mayank Pathak (mpathak) 7-Feb-2026

Revision History:

--*/

#include "AUR32.H"

VOID
EiLoadProgram (
	PUCPU Processor,
	UINT *Program,
	size_t Size
	)
{
    if (PiGetMachineType() == TYPE_AUR32)
	    memcpy(Processor->Aur32->Memory, Program, Size);
    else if (PiGetMachineType() == TYPE_AUR128)
        memcpy(Processor->Aur128->Memory, Program, Size);
}

VOID
EiLoadBinary (
    PUCPU Processor,
    const char *Filename,
    UINT Address
)
{
    FILE *f = fopen(Filename, "rb");

    if (!f)
    {
        printf("Failed to open %s\n", Filename);
        exit(1);
    }

    if (PiGetMachineType() == TYPE_AUR32) {
        fread(Processor->Aur32->Memory + Address, 1, MEMORY_SIZE - Address, f);

        fclose(f);

        Processor->Aur32->PC = Address;
    } else if (PiGetMachineType() == TYPE_AUR128) {
        fread(Processor->Aur128->Memory + Address, 1, MEMORY_SIZE - Address, f);

        fclose(f);

        Processor->Aur128->PC.Low = Address;
    }
}
