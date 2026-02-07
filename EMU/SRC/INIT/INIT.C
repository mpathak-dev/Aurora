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
	    (OP_ADDI << 26) | (1 << 21) | (0 << 16) | 10,
	    (OP_ADDI << 26) | (2 << 21) | (0 << 16) | 20,
	    (OP_ADD  << 26) | (3 << 21) | (1 << 16) | (2 << 11),
//		(0xFC000000),
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
