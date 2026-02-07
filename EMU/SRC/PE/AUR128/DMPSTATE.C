/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    dmpstate.c

Abstract:

    This module implements the architecture specific routine that dumps
    the machine state.

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
	machine state to the console.
    
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
	
	printf("\n\nDumping machine state...\n");

	//
	// Dump the CPU registers R0-R15.
	//

	for (i=0; i<10; i++) {
		printf("R%lu =  LOW(%u), MIDLOW(%u), MIDHIGH(%u), HIGH(%u)\n", i, Processor->Aur128->R[i].Low, Processor->Aur128->R[i].MidLow, Processor->Aur128->R[i].MidHigh, Processor->Aur128->R[i].High);
	}

	for (i=10; i<16; i++) {
		printf("R%lu = LOW(%u), MIDLOW(%u), MIDHIGH(%u), HIGH(%u)\n", i, Processor->Aur128->R[i].Low, Processor->Aur128->R[i].MidLow, Processor->Aur128->R[i].MidHigh, Processor->Aur128->R[i].High);
	}
}
