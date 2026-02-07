/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    cpu.c

Abstract:

    This module implements code necessary for AUR32 processor runs.

Author:

    Mayank Pathak (mpathak) 7-Feb-2026

Revision History:

--*/

#include "AUR32.H"

UINT
PiGetOpcode (
	UINT Instruction
	)
{
	return (Instruction >> 26) & 0x3F;
}

UINT
PiGetRd (
	UINT Instruction
	)
{
	return (Instruction >> 21) & 0x1F;
}

UINT
PiGetRs1 (
	UINT Instruction
	)
{
	return (Instruction >> 16) & 0x1F;
}

UINT
PiGetRs2 (
	UINT Instruction
	)
{
	return (Instruction >> 11) & 0x1F;
}

int16_t
PiGetImm16 (
	UINT Instruction
	)
{
	return Instruction & 0xFFFF;
}

UINT
PiGetAddr26 (
	UINT Instruction
	)
{
	return Instruction & 0x03FFFFFF;
}

VOID
PiStepProcessorA32 (
	PUCPU UProcessor
	)

/*++

Routine Description:

    This routine steps the Aurora32 CPU and executes code.
    
Arguments:

    Processor - Supplies a pointer to the CPU to step.

Return Value:

    None.

--*/
	
{
	PCPU Processor = UProcessor->Aur32;

	//
	// Fetch the instruction from memory.
	//
	
	UINT Instruction = PmRead32(UProcessor, Processor->PC);

	//
	// We increment the program counter by 4 every step,
	// as that is the instruction width.
	//
	
	Processor->PC += 4;

	//
	// Decode the instruction that was fetched.
	//
	
	UINT Opcode = PiGetOpcode(Instruction);
	UINT Rd = PiGetRd(Instruction);
	UINT Rs1 = PiGetRs1(Instruction);
	UINT Rs2 = PiGetRs2(Instruction);
	int16_t Imm = PiGetImm16(Instruction);
	UINT Addr = PiGetAddr26(Instruction);

	//
	// Check for valid opcodes and perform the instruction.
	//

	switch (Opcode)
	{
		case OP_NOP:
			break;

		case OP_ADD:
			Processor->R[Rd] = Processor->R[Rs1] + Processor->R[Rs2];
			break;

		case OP_SUB:
			Processor->R[Rd] = Processor->R[Rs1] - Processor->R[Rs2];
			break;

		case OP_ADDI:
			Processor->R[Rd] = Processor->R[Rs1] + Imm;
			break;

		case OP_LOAD:
			Processor->R[Rd] = PmRead32(UProcessor, Processor->R[Rs1] + Imm);
			break;

		case OP_STORE:
			PmWrite32(UProcessor, Processor->R[Rs1] + Imm, Processor->R[Rd]);
			break;

		case OP_JMP:
			Processor->PC = Addr;
			break;

		case OP_BEQ:
			if (Processor->R[Rd] == Processor->R[Rs1])
				Processor->PC += Imm * 4;
			break;

		case OP_HALT:
			Processor->Running = 0;
			break;

		case OP_CALL:
			Processor->R[31] = Processor->PC;
			Processor->PC = Addr;
			break;

		case OP_RET:
			Processor->PC = Processor->R[31];
			break;
	
		default:
			printf("INVALID OPCODE %u\n", Opcode);
			exit(1);
	}

	//
	// R0 is always set to zero.
	//

	Processor->R[0] = 0;
}
