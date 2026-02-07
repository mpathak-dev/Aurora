/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    mmalloc.c

Abstract:

    This module implements the memory system for the Aurora Emulator.

Author:

    Mayank Pathak (mpathak) 7-Feb-2026

Revision History:

--*/

#include "AUR32.H"

UINT
PmRead32 (
	PUCPU Processor,
	UINT Address
	)

/*++

Routine Description:

    This routine reads a value from memory and returns it.
    
Arguments:

    Processor - Supplies a pointer to the CPU.
    Address - Supplies the address to read from.

Return Value:

    Value read from address.

--*/

{
	UINT128 val = {0,0,0,0};

	if (Address >= MEMORY_SIZE) {
		MmFaultHandler(Address, MM_FAULT_READ);
	}

	//
	// NOTE: The below code works great on x86 machines because it allows
	// unaligned memory access. However, on other architectures, it may be
	// a bit risky and may crash if Address is not a multiple of 4. Adding
	// an alignment check is the way to go here.
	//
	
	if (PiGetMachineType() == TYPE_AUR32)
		return *(UINT *)(Processor->Aur32->Memory + Address);
	else
		return *(UINT *)(Processor->Aur128->Memory + Address);
}

VOID
PmWrite32 (
	PUCPU Processor,
	UINT Address,
	UINT Value
	)

/*++

Routine Description:

    This routine writes a value to memory.
    
Arguments:

    Processor - Supplies a pointer to the CPU.
    Address - Supplies the address to write to.
    Value - Supplies the value to write.

Return Value:

    None.

--*/

{
	if (Address >= MEMORY_SIZE) {
		MmFaultHandler(Address, MM_FAULT_WRITE);
	}

	//
	// NOTE: The below code works great on x86 machines because it allows
	// unaligned memory access. However, on other architectures, it may be
	// a bit risky and may crash if Address is not a multiple of 4. Adding
	// an alignment check is the way to go here.
	//

	if (PiGetMachineType() == TYPE_AUR32)
		*(UINT *)(Processor->Aur32->Memory + Address) = Value;
	else
		*(UINT *)(Processor->Aur128->Memory + Address) = Value;

	//
	// Intercept any writes to the screen and display them.
	//
		
	if (Address >= SCREEN_BASE && Address < SCREEN_BASE + SCREEN_SIZE) {
		UCHAR Character = Value & 0xFF;
		putchar(Character);
		fflush(stdout);
	}
}

UINT128
PmRead128 (
	PCPU128 Processor,
	UINT Address
	)
{
    UINT128 val = {0,0,0,0};

    if (Address + 15 >= MEMORY_SIZE) {
        MmFaultHandler(Address, MM_FAULT_READ);
        return val;
    }

    val.Low     = *(UINT *)(Processor->Memory + Address);
    val.MidLow  = *(UINT *)(Processor->Memory + Address + 4);
    val.MidHigh = *(UINT *)(Processor->Memory + Address + 8);
    val.High    = *(UINT *)(Processor->Memory + Address + 12);

    return val;
}

VOID
PmWrite128 (
    PCPU128 Processor,
    UINT Address,
    UINT128 Value
    )
{
    if (Address + 15 >= MEMORY_SIZE) {
        MmFaultHandler(Address, MM_FAULT_WRITE);
        return;
    }

    *(UINT *)(Processor->Memory + Address)       = Value.Low;
    *(UINT *)(Processor->Memory + Address + 4)   = Value.MidLow;
    *(UINT *)(Processor->Memory + Address + 8)   = Value.MidHigh;
    *(UINT *)(Processor->Memory + Address + 12)  = Value.High;

    if (Address >= SCREEN_BASE && Address < SCREEN_BASE + SCREEN_SIZE) {
        UCHAR Character = (UCHAR)(Value.Low & 0xFF);
        putchar(Character);
        fflush(stdout);
    }
}
