.entry LOOP
.entry LENGTH
.extern L3
.extern W
MAIN: 	mov #4, W
	add r2,STR
LOOP: 	jmp W
	prn #-5
	sub r1, r4
	inc K
	mov r5,r3
	bne L3
END: 	stop
STR: 	.string "string with spaces"
LENGTH:	.data 6, 15 ,89   ,-16
K: 	.data 2
