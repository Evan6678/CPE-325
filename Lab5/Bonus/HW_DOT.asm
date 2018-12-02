;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file

;-------------------------------------------------------------------------------

			.def	HW_DOT

HW_DOT:
			push	R4						;Save value of R4	|Array 1|
			push	R5						;Save value of R5	|Array 2|
			push	R6						;Save value of R6	|Result location|
			push	R7						;Save value of R7	|Counter|
			push	R9						;Save value of R9	|R9 is used locally to store the individal multiplicaton|

			clr.w	R4						;Clear R4
			clr.w	R5						;Clear R5
			clr.w	R6						;Clear R6
			clr.w	R7						;Clear R7
			clr.w	R8						;Clear R8
			clr.w	R9						;Clear R9

			mov.w	18(SP), R4				;move the start of arr1 into R4
			mov.w	16(SP), R5				;move the start of arr2 int0 R5
			mov.w	14(SP),	R6				;move the result of the dot product into R6
			mov.w	12(SP),	R7				;mov the counter value into R7
			mov.w	#0,		R8				;init R8 to 0
			mov.w	#0,		R9				;init R9 to 0

mult:
			cmp.w	#0,	R7					;check counter value
			jeq		lend					;if 0 jump to end
			mov.b	@R4+, &0132h			;load first operand
			sxt		&MPYS					;sign first operand
			mov.b	@R5+, &0138h			;load second operand
			sxt		&OP2					;sign second operand

			mov.w	&RESLO, R9				;Push first 8 bits into result

			add.w	R9,	0(R6)				;add the result of the mult to the dot product holder
			jmp 	dec						;jump to shifter

dec:
			dec.w	R7						;decrement counter
			jmp		mult					;start next loop

lend:
			pop		R9						;Restore register
			pop		R7						;Restore register
			pop		R6						;Restore register
			pop		R5						;Restore register
			pop		R4						;Restore register
			ret
