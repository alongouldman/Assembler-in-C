		.entry		COUNT

		.extern		STRADD

		.extern		LEN
COUNT:	mov	 STRADD,	r1

		cmp		#0, r1

		inc		LEN

		inc		r1

		jmp		COUNT

ENDCOUNT:		stop
