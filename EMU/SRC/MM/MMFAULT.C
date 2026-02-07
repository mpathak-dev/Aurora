/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    mmfault.c

Abstract:

    This module implements the memory fault handlers for the Aurora Emulator.

Author:

    Mayank Pathak (mpathak) 7-Feb-2026

Revision History:

--*/

#include "AUR32.H"

VOID
MmFaultHandler (
	UINT Address,
	UCHAR Type
	)

/*++

Routine Description:

    This routine handles a memory fault error.
    
Arguments:

    Address - Supplies the address the fault was caused in.
    Type - Supplies the type of memory fault error.

Return Value:

    None.

--*/
	
{
	if (Type == MM_FAULT_WRITE) {
		printf("***MEMORY FAULT invalid write to address %u stopping execution\n", Address);
		exit(1);
	}
	
	if (Type == MM_FAULT_READ) {
		printf("***MEMORY FAULT invalid read from address %u stopping execution\n", Address);
		exit(1);
	}

	if (Type == MM_FAULT_ACCESS) {
		printf("***MEMORY FAULT invalid access to address %u stopping execution\n", Address);
		exit(1);
	}
}
