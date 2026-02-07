;++
;
; FACILITY:
;
;	Assembler test program for AURORA
;
; ABSTRACT:
;
;	Moves a value into R2 and transfers control to a loop that runs
;	and adds 1 to R3 until it reaches the same value as R2.
;
; AUTHOR:
;
;	Mayank Pathak	6 February 2026
;
; REVISION HISTORY:
;
;
;--

start:
    ADDI R2, R0, 30

loop:
    ADDI R3, R3, 1
    BEQ R3, R2, end
    JMP loop

end:
    HALT
