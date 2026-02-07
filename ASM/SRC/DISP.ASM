;++
;
; FACILITY:
;
;	Display test program for AURORA
;
; ABSTRACT:
;
;	Displays the string "HELLO WORLD!" to the available screen and halts.
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
	ADDI R1, R0, 0x48
	CALL displchar
	
	ADDI R1, R0, 0x45
	CALL displchar

	ADDI R1, R0, 0x4C
	CALL displchar
	CALL displchar

	ADDI R1, R0, 0x4F
	CALL displchar

	ADDI R1, R0, 0x20
	CALL displchar

	ADDI R1, R0, 0x57
	CALL displchar

	ADDI R1, R0, 0x4F
	CALL displchar

	ADDI R1, R0, 0x52
	CALL displchar

	ADDI R1, R0, 0x4C
	CALL displchar

	ADDI R1, R0, 0x44
	CALL displchar

	ADDI R1, R0, 0x21
	CALL displchar

end:	
	HALT

displchar:
	STORE R1, R0, 0x400
	RET
