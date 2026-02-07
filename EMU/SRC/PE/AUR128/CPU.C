/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    cpu.c

Abstract:

    This module implements code necessary for AUR128 processor runs.

Author:

    Mayank Pathak (mpathak) 7-Feb-2026

Revision History:

--*/

#include "AUR32.H"

UINT128
PiAdd128 (
	UINT128 a,
	UINT128 b
	)
{
    UINT128 r;
    unsigned long carry = 0;
    
    r.Low     = a.Low + b.Low;
    carry     = (r.Low < a.Low) ? 1 : 0;
    r.MidLow  = a.MidLow + b.MidLow + carry;
    carry     = (r.MidLow < a.MidLow) ? 1 : 0;
    r.MidHigh = a.MidHigh + b.MidHigh + carry;
    carry     = (r.MidHigh < a.MidHigh) ? 1 : 0;
    r.High    = a.High + b.High + carry;
    
    return r;
}

UINT128
PiSub128 (
	UINT128 a,
	UINT128 b
	)
{
    UINT128 r;
    unsigned long carry = 0;
    
    r.Low     = a.Low - b.Low;
    carry     = (r.Low < a.Low) ? 1 : 0;
    r.MidLow  = a.MidLow - b.MidLow - carry;
    carry     = (r.MidLow < a.MidLow) ? 1 : 0;
    r.MidHigh = a.MidHigh - b.MidHigh + carry;
    carry     = (r.MidHigh < a.MidHigh) ? 1 : 0;
    r.High    = a.High - b.High - carry;
    
    return r;
}

VOID
PiTriggerInterrupt (
	PCPU128 Processor,
	UINT irq
	)
{
    if(irq >= 16) return;
    Processor->Pending |= (1 << irq);
}

VOID
PiStepProcessorA128 (
	PUCPU UProcessor
	)

/*++

Routine Description:

    This routine steps the Aurora128 CPU and executes code.
    
Arguments:

    Processor - Supplies a pointer to the CPU to step.

Return Value:

    None.

--*/
	
{
    PCPU128 Processor = UProcessor->Aur128;

    UINT Instruction = PmRead32(UProcessor, Processor->PC.Low);
    Processor->PC.Low += 4;

    UINT Opcode = PiGetOpcode(Instruction);
    UINT Rd     = PiGetRd(Instruction);
    UINT Rs1    = PiGetRs1(Instruction);
    UINT Rs2    = PiGetRs2(Instruction);
    int16_t Imm = PiGetImm16(Instruction);
    UINT Addr   = PiGetAddr26(Instruction);

    if(Processor->IE && Processor->Pending)
    {
        //
        // Find first pending interrupt and clear the pending bit.
        //

        int irq = 0;
        while(!(Processor->Pending & (1 << irq))) irq++;

        Processor->Pending &= ~(1 << irq);

        //
        // Save current PC in R[30] as return address.
        //

        Processor->R[30] = Processor->PC;

        //
        // Jump to vector.
        //

        Processor->PC = Processor->VectorPC[irq];
    }

    switch (Opcode)
    {
        case OP_NOP: break;
        case OP_ADD:
            Processor->R[Rd] = PiAdd128(Processor->R[Rs1], Processor->R[Rs2]);
            break;
        case OP_SUB:
            Processor->R[Rd] = PiSub128(Processor->R[Rs1], Processor->R[Rs2]);
            break;
        case OP_ADDI:
            Processor->R[Rd].Low += Imm;
            break;
        case OP_LOAD:
            Processor->R[Rd] = PmRead128(Processor, Processor->R[Rs1].Low + Imm);
            break;
        case OP_STORE:
            PmWrite128(Processor, Processor->R[Rs1].Low + Imm, Processor->R[Rd]);
            break;
        case OP_JMP:
            Processor->PC.Low = Addr;
            break;
        case OP_BEQ:
            if (Processor->R[Rd].Low == Processor->R[Rs1].Low)
                Processor->PC.Low += Imm * 4;
            break;
        case OP_HALT:
            Processor->Running = 0;
            break;
        case OP_CALL:
            Processor->R[31] = Processor->PC;
            Processor->PC.Low = Addr;
            break;
        case OP_RET:
            Processor->PC = Processor->R[31];
            break;
        case OP_RETI:
            Processor->PC = Processor->R[30];
            break;
        default:
            printf("INVALID OPCODE %u\n", Opcode);
            PiTriggerInterrupt(Processor, INT_INVALID);
    }

    Processor->R[0] = (UINT128){0,0,0,0};
}
