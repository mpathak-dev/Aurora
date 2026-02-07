;++
;
; FACILITY:
;
;	Assembler test program for AURORA
;
; ABSTRACT:
;
;	This is a test 128-bit program for the AUR128 architecture. It
;	utilizes the new instructions like CLZ, SYSCALL and CAS.
;
; AUTHOR:
;
;	Mayank Pathak	7 February 2026
;
; REVISION HISTORY:
;
;
;--

START:
    ADDI R1, R0, 0x500
    ADDI R2, R0, 0
    ADDI R3, R0, 1
    CAS  R3, R1, R2
    
    ADDI R10, R0, 0x000F
    CLZ  R4, R10
    
    ADDI R5, R0, 5
    SYSCALL
    HALT
