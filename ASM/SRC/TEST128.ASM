;++
;
; FACILITY:
;
;	Assembler test program for AURORA
;
; ABSTRACT:
;
;	This program exercises the 128-bit extended instruction set, 
;	specifically testing the Barrel Shifter (SLL/SRL), Atomic 
;	Memory Operations (AMOADD), and 128-bit Register Moves.
;
; AUTHOR:
;
;	Mayank Pathak	7 February 2026
;
; REVISION HISTORY:
;
;	Mayank Pathak    8 February 2026
;		Added more 128-bit tests.
;
;--

START:
    ;---------------------------------------------------------
    ; Test 128-bit Register Move (MOV128)
    ;---------------------------------------------------------
    ADDI    R1, R0, 0xAABB
    MOV128  R2, R1
    
    ;---------------------------------------------------------
    ; Test Barrel Shifter (SLL / SRL)
    ;---------------------------------------------------------
    ADDI    R10, R0, 0x0001
    ADDI    R11, R0, 33
    SLL     R10, R10, R11
    SRL     R10, R10, R11

    ;---------------------------------------------------------
    ; Test Atomic Memory Operation (AMOADD)
    ;---------------------------------------------------------
    ; Using 0x0200 - Safe zone far below SCREEN_BASE (0x0400)
    ADDI    R20, R0, 0x0200
    ADDI    R21, R0, 100
    STORE   R21, R20, 0
    
    ADDI    R22, R0, 50
    AMOADD R23, R20, R22

    ;---------------------------------------------------------
    ; System Logic
    ;---------------------------------------------------------
    ADDI    R10, R0, 0xFFFF
    CLZ     R4, R10
    
    ADDI    R5, R0, 13
    SYSCALL                     
    
    HALT
