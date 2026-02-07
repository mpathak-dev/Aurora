/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    dmpstate.c

Abstract:

    This module implements the architecture specific routine that dumps
    the machine state in a hexadecimal format.

Author:

    Mayank Pathak (mpathak) 7-Feb-2026

Revision History:

--*/

#include "AUR32.H"

VOID
PiDumpMachineStateA128 (
    PUCPU Processor
    )

/*++

Routine Description:

    This routine dumps the current CPU and other components
    machine state to the console using hexadecimal notation.
    
Arguments:

    Processor - Supplies a pointer to the CPU.

Return Value:

    None.

--*/
    
{
    ULONG i;

    //
    // Announce state dump.
    //
    
    printf("\n--- Aurora-128 Machine State Dump ---\n");

    //
    // Dump the CPU registers R0-R31.
    // Note: Standardized to 8 hex digits per segment.
    //

    for (i = 0; i < 32; i++) {
        printf("R%-2lu = %08X-%08X-%08X-%08X\n", 
               i, 
               Processor->Aur128->R[i].High, 
               Processor->Aur128->R[i].MidHigh, 
               Processor->Aur128->R[i].MidLow, 
               Processor->Aur128->R[i].Low);
               
        // Add a newline every 8 registers for better readability
        if ((i + 1) % 8 == 0) printf("\n");
    }

    //
    // Dump the Program Counter.
    //

    printf("PC  = %08X-%08X-%08X-%08X\n",
           Processor->Aur128->PC.High,
           Processor->Aur128->PC.MidHigh,
           Processor->Aur128->PC.MidLow,
           Processor->Aur128->PC.Low);

    printf("------------------------------------\n\n");
}
