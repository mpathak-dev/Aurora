; Axiom 128-bit Test Program
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
