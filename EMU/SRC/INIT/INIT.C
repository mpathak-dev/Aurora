/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    init.c

Abstract:

    Main source file for the AURORA Emulator initialization subcomponent.

Author:

    Mayank Pathak (mpathak) 6-Feb-2026

Revision History:

	Mayank Pathak    7 February 2026
		Refactored for modularization of the source tree.
	
--*/

#include "AUR32.H"

VOID
EiStepProcessor (
	PUCPU Processor
	)

/*++

Routine Description:

    This routine steps the CPU and executes code.
    
Arguments:

    Processor - Supplies a pointer to the CPU to step.

Return Value:

    None.

--*/

{
	PeStepProcessor(Processor);
}

VOID
EiRunSystem (
	PUCPU Processor
	)

/*++

Routine Description:

    This routine runs the CPU and executes code.
    
Arguments:

    Processor - Supplies a pointer to the CPU to run.

Return Value:

    None.

--*/

{
	while (
		(PiGetMachineType() == TYPE_AUR32 && Processor->Aur32->Running) ||
		(PiGetMachineType() == TYPE_AUR128 && Processor->Aur128->Running)
	)
	{
		EiStepProcessor(Processor);
	}
}

VOID
EiDumpMachineState (
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
	PeDumpMachineState(Processor);
}

VOID
EiSystemStartup (
	PLOADER_BLOCK LoaderBlock
	)

/*++

Routine Description:

    This routine initializes the CPU and it's subcomponents
    and starts the emulator.
    
Arguments:

    None.

Return Value:

    None.

--*/
	
{
	//printf("Aurora32, it's happenin'!\n");

	uint32_t Program[] =
	{
	    // ADDI: Initialize R1 with a memory address (0x500) for our lock
	    (OP_ADDI << 26) | (1 << 21) | (0 << 16) | 0x500,
	
	    // ADDI: Set R2 to 0 (The 'Unlocked' state)
	    (OP_ADDI << 26) | (2 << 21) | (0 << 16) | 0,
	
	    // ADDI: Set R3 to 1 (Our 'Lock Request' value)
	    (OP_ADDI << 26) | (3 << 21) | (0 << 16) | 1,
	
	    // CAS: Attempt to take the lock. 
	    // If [R1] == R2 (0), then [R1] = R3 (1). 
	    // Result (old value) is stored back in R3.
	    (OP_CAS << 26) | (3 << 21) | (1 << 16) | (2 << 11),
	
	    // CLZ: Check for highest priority interrupt in R10 (Simulating Pending bitmask)
	    // Assume R10 was set by an external trigger to 0x00008000
	    (OP_ADDI << 26) | (10 << 21) | (0 << 16) | 0x8000, 
	    (OP_CLZ  << 26) | (4 << 21) | (10 << 16), // R4 will now contain the leading zero count
	
	    // SYSCALL: Transition to kernel mode to perform an I/O task
	    // R5 holds the function code (e.g., 0x05 for 'Print')
	    (OP_ADDI << 26) | (5 << 21) | (0 << 16) | 5,
	    (OP_SYSCALL << 26),
	
	    // HALT: Execution complete
	    (OP_HALT << 26)
	};

	UCPU Processor;
	CPU Aur32Cpu;
	CPU128 Aur128Cpu;

	Processor.Aur32 = &Aur32Cpu;
	Processor.Aur128 = &Aur128Cpu;

	if(!PiInitializeProcessor(&Processor, LoaderBlock)) {
		exit(1);
	}

	if (!LoaderBlock->LoadTestProgram)
		EiLoadBinary(&Processor, LoaderBlock->ProgramString, LoaderBlock->LoadAddress);
	else
		EiLoadProgram(&Processor, Program, sizeof(Program));
		
	EiRunSystem(&Processor);

	EiDumpMachineState(&Processor);
	
	return;
}
