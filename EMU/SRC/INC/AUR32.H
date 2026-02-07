/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    aur32.h

Abstract:

    Top level include file for the AURORA Emulator component.

Author:

    Mayank Pathak (mpathak) 6-Feb-2026

Revision History:

--*/

#ifndef _AUR32_
#define _AUR32_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef UCHAR BYTE;
typedef void VOID;
typedef void* PVOID;
typedef unsigned long ULONG;
typedef char CHAR;
typedef char* PCHAR;
typedef UCHAR* PUCHAR;
typedef ULONG* PULONG;
typedef UCHAR BOOLEAN;
typedef unsigned int UINT;
typedef USHORT* PUSHORT;
typedef ULONG ULONG_PTR;
typedef const char* PCSTR;
typedef ULONG EMUSTATUS;

typedef struct UINT128
{
    UINT Low;
    UINT MidLow;
    UINT MidHigh;
    UINT High;
} UINT128, *PUINT128;

#define TRUE 1
#define FALSE 0

#define IN
#define OUT
#define OPTIONAL

#define MEMORY_SIZE (1024 * 1024) // 1MB for now
#define SCREEN_BASE 0x0400
#define SCREEN_SIZE 1024  // 32x32 chars

#define MM_FAULT_WRITE 0
#define MM_FAULT_ACCESS 1
#define MM_FAULT_READ 2

#define TYPE_AUR32 0
#define TYPE_AUR128 1

#define VECTOR_BASE 0xF000  // Top of 1MB memory, 0xF000..0xFFFF reserved for vectors
#define VECTOR_COUNT 16      // 16 interrupts
#define VECTOR_SIZE 4        // 32-bit instruction per vector
#define VECTOR_ADDR(i) (VECTOR_BASE + (i) * VECTOR_SIZE)

typedef struct CPU
{
    UINT R[32];     // General registers
    UINT PC;        // Program counter
    UINT FLAGS;
    PUCHAR Memory;
    int Running;
} CPU, *PCPU;

#define IDT_SIZE 256 // 256 interrupts, x86 style

typedef struct IDTEntry {
    UINT128 HandlerPC; // 128-bit Aurora128 pointer to code
} IDTEntry;

typedef struct CPU128
{
    UINT128 R[32];
    UINT128 PC;
    UINT FLAGS;
    PUCHAR Memory;
    int Running;

	UINT IE;
	UINT Pending;
	UINT128 VectorPC[16];
} CPU128, *PCPU128;

enum
{
    INT_TIMER      = 0,
    INT_KEYBOARD   = 1,
    INT_SOFTWARE   = 2,
    INT_MEMORY     = 3,
	INT_DISK	   = 4,
	INT_INVALID    = 15
};

typedef struct UCPU
{
	PCPU Aur32;
	PCPU128 Aur128;
} UCPU, *PUCPU;

enum
{
    OP_NOP  = 0,
    OP_ADD  = 1,
    OP_SUB  = 2,
    OP_ADDI = 3,
    OP_LOAD = 4,
    OP_STORE= 5,
    OP_JMP  = 6,
    OP_BEQ  = 7,
    OP_HALT = 8,
    OP_CALL = 9,
    OP_RET = 10,
	OP_RETI = 11
};

typedef struct LOADER_BLOCK
{
	PCSTR ProgramString;
	UINT LoadAddress;
	UCHAR LoadTestProgram;
	UCHAR MachineType;
} LOADER_BLOCK, *PLOADER_BLOCK;

extern UCHAR Memory[MEMORY_SIZE];

UINT
PmRead32 (
	PUCPU Processor,
	UINT Address
	);

VOID
PmWrite32 (
	PUCPU Processor,
	UINT Address,
	UINT Value
	);

UINT128
PmRead128 (
	PCPU128 Processor,
	UINT Address
	);

VOID
PmWrite128 (
    PCPU128 Processor,
    UINT Address,
    UINT128 Value
    );

VOID
MmFaultHandler (
	UINT Address,
	UCHAR Type
	);

VOID
PiInitializeMachineA32 (
	PCPU Processor
	);

VOID
PiInitializeMachineA128 (
	PCPU128 Processor
	);

BOOLEAN
PiInitializeProcessor (
	PUCPU Processor,
	PLOADER_BLOCK LoaderBlock
	);

UCHAR
PiGetMachineType (
	VOID
	);

VOID
PeStepProcessor (
	PUCPU UProcessor
	);

VOID
PiStepProcessorA32 (
	PUCPU UProcessor
	);

VOID
PiStepProcessorA128 (
	PUCPU UProcessor
	);

VOID
PeDumpMachineState (
	PUCPU Processor
	);

VOID
PiDumpMachineStateA128 (
	PUCPU Processor
	);

VOID
PiDumpMachineStateA32 (
	PUCPU Processor
	);

UINT
PiGetOpcode (
	UINT Instruction
	);

UINT
PiGetRd (
	UINT Instruction
	);

UINT
PiGetRs1 (
	UINT Instruction
	);

UINT
PiGetRs2 (
	UINT Instruction
	);

int16_t
PiGetImm16 (
	UINT Instruction
	);

UINT
PiGetAddr26 (
	UINT Instruction
	);

VOID
EiLoadProgram (
	PUCPU Processor,
	UINT *Program,
	size_t Size
	);
	
VOID
EiLoadBinary (
    PUCPU Processor,
    const char *Filename,
    UINT Address
);

VOID
EiSystemStartup (
	PLOADER_BLOCK LoaderBlock
	);

#endif
