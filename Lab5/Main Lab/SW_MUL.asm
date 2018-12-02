;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------

			.cdecls C,LIST,"msp430.h"       ; Include device header file

			.def SW_MUL

			.text

SW_MUL:
			push.w	R5				;Push R5 to save the value
			push.w	R6				;Push R6 to save the value
			push.w	R7				;Push R7 to save the value
			push.w	R8				;Push R8 to save the value
			push.w	R9				;push R9 to save the value

			clr.w	R5				;Clear register
			clr.w	R6				;Clear register
			clr.w	R7				;Clear register
			clr.w	R8				;Clear register
			clr.w	R9				;Clear register

			mov.b	14(SP), R5		;R5 = M[numA]
			swpb	14(SP)			;Flip Bit
			mov.b	14(SP), R6		;R6 = M[numB]
			mov.w	12(SP), R7		;R7 = ret
			mov.w	#8,	R8			;R8 is counter variable

mult:
			cmp.w	#0, R8			;check if at end of binary number
			jeq		lend			;if = to 0 wrapup and end
			cmp		#1,	R8			;check if at last bit to check sign
			jeq		checkNeg		;if last bit check if negative
			bit.b	#1, R6			;compare LSB to 1
			jne		sum				;if bit is 1 then jump to sum
			jmp		rotate			;else it is 0 gump to rotate

sum:
			add.w	R5, R9			;add R5 into holder R9
			rla.b	R5				;rotate left
			rra.b	R6				;rotate right
			dec		R8				;decrement counter
			jmp		mult			;jump back to mult

rotate:
			rla.b	R5				;Rotate the R5 Value to push GMB out
			rra.b	R6				;Rotate the R6 value right to push LMB out
			dec		R8				;decrement counter
			jmp		mult			;jump back to mult

checkNeg:
			bit		#1, R6			;check if bit is 1
			jne		negB			;if 1 it is negative
			bit		#1, R5			;check if bit is 1
			jne		negA			;
			jmp		lend			;if 0 then end

negA:
			sub.w	R5, R9			;subtract numA to B
			jmp		lend			;end program

negB:
			bit		#1, R5			;check if bit is 1
			jne 	lend			;jump to end if neg also
			sub.w	R5, R9			;subtract numA to B
			jmp 	lend			;jump to end
lend:
			mov.b	R9, 0(R7)		;move value to res2
			pop.w	R9				;restore R9
			pop.w	R8				;restore R8
			pop.w	R7				;restore R7
			pop.w	R6				;restore R6
			pop.w	R5				;restore R5
			ret
			.end
