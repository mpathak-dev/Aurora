/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    init32.c

Abstract:

    This module implements Aurora32-specific processor initialization.

Author:

    Mayank Pathak (mpathak) 7-Feb-2026

Revision History:

--*/

#include "AUR32.H"

VOID
PiInitializeMachineA32 (
	PCPU Processor
	)

/*++

Routine Description:

    This routine is called from the architecture independent initialization routine
    during bootstrap to initialize the CPU and all of its subcomponents.
    
Arguments:

    Processor - Supplies a pointer to the CPU to initialize.

Return Value:

    None.

--*/
	
{
	//
	// Empty the processor structure entirely.
	//
	
	memset(Processor, 0, sizeof(CPU));

	//
	// Set the global memory to the processor's memory and
	// set it to RUNNING.
	//
	
	Processor->Memory = Memory;
	Processor->Running = 1;

	//
	// Set the downward growing stack.
	//

	Processor->R[30] = MEMORY_SIZE - 4;

	//
	// Set the processor program counter to address $00.
	//

	Processor->PC = 0;
}
