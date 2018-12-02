;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file

            .def HW_MUL

            .text

HW_MUL:
			push.w	R5				;Push R5 to save the value
			push.w	R6				;Push R6 to save the value
			push.w	R7				;Push R7 to save the value

			clr.w	R5				;Clear register
			clr.w	R6				;Clear register
			clr.w	R7				;Clear register

			mov.b	10(SP), R5		;R5 = M[numA]
			swpb	10(SP)			;Flip Bit
			mov.b	10(SP), R6		;R6 = M[numB]
			mov.w	8(SP), R7		;R7 = ret

			mov.b	R5, &0132h		;load first operand
			sxt		&MPYS			;sign first operand
			mov.b	R6, &0138h		;load second operand
			sxt		&OP2			;sign second operand

			mov.w	&RESLO, 0(R7)	;Push first 8 bits into result
			mov.w	&RESHI,	2(R7)	;Push Second 8 bits into result

lend:		pop.w	R7				;restore R7
			pop.w	R6				;restore R6
			pop.w	R5				;restore R5
			ret
			.end
