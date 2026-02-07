/*++

Copyright (c) 2026 The Aurora32 Project

Module Name:

    aur32.h

Abstract:

    Top level include file for the AURORA C Compiler component.

Author:

    Mayank Pathak (mpathak) 6-Feb-2026

Revision History:

	Mayank Pathak    7 February 2026	
        Suited as needed for AURORA C Compiler.
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
typedef ULONG STATUS;

#define TRUE 1
#define FALSE 0

#define IN
#define OUT
#define OPTIONAL



#endif
