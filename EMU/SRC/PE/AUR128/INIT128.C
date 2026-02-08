/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    init128.c

Abstract:

    This module implements Aurora128-specific processor initialization.

Author:

    Mayank Pathak (mpathak) 7-Feb-2026

Revision History:

--*/

#include "AUR32.H"

VOID
PiInitializeMachineA128 (
	PCPU128 Processor
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
	
	memset(Processor, 0, sizeof(CPU128));

	//
	// Set the global memory to the processor's memory and
	// set it to RUNNING.
	//
	
	Processor->Memory = Memory;
	Processor->Running = 1;

	//
    // Initialize stack pointer (R30) to top of memory.
    // Only low 32 bits needed initially.
    //

    Processor->R[30].Low = MEMORY_SIZE - 4;
    Processor->R[30].MidLow = 0;
    Processor->R[30].MidHigh = 0;
    Processor->R[30].High = 0;

	//
	// Set the processor program counter to address $00.
	//

    Processor->PC.Low = 0;
    Processor->PC.MidLow = 0;
    Processor->PC.MidHigh = 0;
    Processor->PC.High = 0;

    Processor->R[0] = (UINT128){0,0,0,0};

    Processor->IE = 1;
    Processor->Pending = 0;

	for(int i = 0; i < VECTOR_COUNT; i++) {
        Processor->VectorPC[i].Low     = VECTOR_BASE + i * VECTOR_SIZE;
        Processor->VectorPC[i].MidLow  = 0;
        Processor->VectorPC[i].MidHigh = 0;
        Processor->VectorPC[i].High    = 0;

        //
        // Initialize vector instructions:
        // INT_INVALID -> HALT, others -> RETI
        //

        UINT instr;
        if(i == INT_INVALID) {
            instr = (OP_HALT << 26);
        } else {
            instr = (OP_RETI << 26);
        }

        *(UINT *)(Processor->Memory + VECTOR_BASE + i * VECTOR_SIZE) = instr;
    }
}
