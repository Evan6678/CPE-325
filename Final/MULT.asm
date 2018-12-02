;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file

;-------------------------------------------------------------------------------

			.def	HW_MULT
			.global HW_MULT

HW_MULT:
			push	R7						;Save value of R7	|Counter|

			clr.w	R7						;Clear R7
			mov		#0, &MAC				;Clear MAC
			mov		#0, &OP2				;Clear OP2
			mov		#0, &RESLO				;Clear RESLO

			mov.w	#3,	R7					;mov the counter value into R7

mult:
			cmp.w	#0,	R7					;check counter value
			jeq		lend					;if 0 jump to end
			mov.w	@R12+, &MAC				;load first operand
			mov.w	@R13, &OP2				;load second operand

			mov.w	&RESLO, 0(R14)			;Push first 8 bits into result
			mov.w	&RESHI, 2(R14)			;

			inc.w	R13						;
			inc.w	R13						;
			inc.w	R13						;
			inc.w	R13						;
			inc.w	R13						;
			inc.w	R13						;

			jmp 	dec						;jump to shifter

dec:
			dec.w	R7						;decrement counter
			jmp		mult					;start next loop

lend:
			pop R7							;return value to R7
			ret
