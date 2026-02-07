/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    test.c

Abstract:

    Main source file for the AURORA C Compiler Test Program.

Author:

    Mayank Pathak (mpathak) 7-Feb-2026

Revision History:

--*/

INT
DisplayCharacter (
	CHAR Character
	)

/*++

Routine Description:


	This routine is called from the program startup routine to check if
	function calls properly work or not.
    
Arguments:

    Character - Supplies a character to display to the screen.

Return Value:

    The integer 5.

--*/
	
{
	//
	// 0x400 is the screen buffer address, so write to it.
	//
	
	*(ULONG*)0x400 = Character;

	return 5;
}

VOID
HaltProcessor (
	VOID
	)

/*++

Routine Description:


	This routine halts the system processor.
    
Arguments:

    None.

Return Value:

    None.

--*/
	
{
	__asm {
		HALT
	};
}

VOID
Startup (
	VOID
	)

/*++

Routine Description:

    This routine is the main program initialization entry.
    
Arguments:

    None.

Return Value:

    None.

--*/

{
	//
	// Display the character 'A' to the screen, and halt.
	//

	DisplayCharacter('A');
	*(ULONG*)0x400 =66; // raw write, 'B'
	HaltProcessor();
}
