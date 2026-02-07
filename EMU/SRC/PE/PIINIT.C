/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    piinit.c

Abstract:

    This module implements architecture independent processor initialization.

Author:

    Mayank Pathak (mpathak) 7-Feb-2026

Revision History:

--*/

#include "AUR32.H"

//
// Define global static data.
//

UCHAR Memory[MEMORY_SIZE];
UCHAR MachineType;

BOOLEAN
PiInitializeProcessor (
	PUCPU Processor,
	PLOADER_BLOCK LoaderBlock
	)

/*++

Routine Description:

    This routine is called from the startup initialization routine during
    bootstrap to initialize the CPU and all of its subcomponents.
    
Arguments:

    Processor - Supplies a pointer to the CPU to initialize.

Return Value:

    None.

--*/
	
{
	MachineType = LoaderBlock->MachineType;

	if (LoaderBlock->MachineType == TYPE_AUR32) {
		PiInitializeMachineA32(Processor->Aur32);
		return TRUE;
	} else if (LoaderBlock->MachineType == TYPE_AUR128) {
		PiInitializeMachineA128(Processor->Aur128);
		return TRUE;
	} else {
		printf("Machine type not supported\n");
		return FALSE;
	}
}

VOID
PeStepProcessor (
	PUCPU Processor
	)

/*++

Routine Description:

    This routine steps the system CPU.
    
Arguments:

    Processor - Supplies a pointer to the CPU to step.

Return Value:

    None.

--*/
	
{
	if (PiGetMachineType() == TYPE_AUR32) {
		PiStepProcessorA32(Processor);
	} else if (PiGetMachineType() == TYPE_AUR128) {
		PiStepProcessorA128(Processor);
	}
}

VOID
PeDumpMachineState (
	PUCPU Processor
	)

/*++

Routine Description:

    This routine dumps the system state.
    
Arguments:

    Processor - Supplies a pointer to the CPU to dump state.

Return Value:

    None.

--*/

{
	if (PiGetMachineType() == TYPE_AUR32) {
		PiDumpMachineStateA32(Processor);
	} else if (PiGetMachineType() == TYPE_AUR128) {
		PiDumpMachineStateA128(Processor);
	}
}

UCHAR
PiGetMachineType (
	VOID
	)
{
	return MachineType;
}