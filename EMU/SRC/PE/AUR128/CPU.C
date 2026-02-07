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
        case OP_ADDI: {
			UINT128 imm128 = { (UINT)Imm, (Imm < 0) ? 0xFFFFFFFF : 0, 
                               (Imm < 0) ? 0xFFFFFFFF : 0, (Imm < 0) ? 0xFFFFFFFF : 0 };
            Processor->R[Rd] = PiAdd128(Processor->R[Rs1], imm128);
            break;
        }
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

		case OP_CLZ: {
            // Count leading zeros in the 128-bit register (High to Low)
            // Essential for high-speed interrupt and priority scheduling
            UINT count = 0;
            UINT128 val = Processor->R[Rs1];
            UINT parts[4] = { val.High, val.MidHigh, val.MidLow, val.Low };
            
            for (int i = 0; i < 4; i++) {
                if (parts[i] == 0) count += 32;
                else {
                    UINT temp = parts[i];
                    while (!(temp & 0x80000000)) { temp <<= 1; count++; }
                    break;
                }
            }
            Processor->R[Rd] = (UINT128){ count, 0, 0, 0 };
            break;
        }

        case OP_CAS: {
            // Atomic Compare and Swap: If [Rs1] == Rs2, then [Rs1] = Rd.
            // Returns original value in Rd for lock-check logic.
            UINT128 currentVal = PmRead128(Processor, Processor->R[Rs1].Low);
            if (currentVal.Low == Processor->R[Rs2].Low && 
                currentVal.High == Processor->R[Rs2].High) { // Simplified 64-bit check
                PmWrite128(Processor, Processor->R[Rs1].Low, Processor->R[Rd]);
            }
            Processor->R[Rd] = currentVal; 
            break;
        }

        case OP_SYSCALL:
            // Trigger Software Interrupt 2 (System Service)
            PiTriggerInterrupt(Processor, INT_SOFTWARE);
            break;
        
        default:
            printf("INVALID OPCODE %u\n", Opcode);
            PiTriggerInterrupt(Processor, INT_INVALID);
    }

    Processor->R[0] = (UINT128){0,0,0,0};
}
