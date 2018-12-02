;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file

;-------------------------------------------------------------------------------

			.def	DOT

DOT:
			push	R4						;Save value of R4	|Array 1|
			push	R5						;Save value of R5	|Array 2|
			push	R6						;Save value of R6	|Result location|
			push	R7						;Save value of R7	|Counter|
			push	R8						;Save value of R9	|R8 is used locally to store the individal multiplicaton|
			push	R9						;NumA = M[R4]
			push	R10						;NumB = M[R5]
			push	R11						;mult calculator counter

			clr.w	R4						;Clear R4
			clr.w	R5						;Clear R5
			clr.w	R6						;Clear R6
			clr.w	R7						;Clear R7
			clr.w	R8						;Clear R8
			clr.w	R9						;Clear R9
			clr.w	R10						;Clear R10
			clr.w	R11						;Clear R11

			mov.w	24(SP), R4				;move the start of arr1 into R4
			mov.w	22(SP), R5				;move the start of arr2 int0 R5
			mov.w	20(SP),	R6				;move the result of the dot product into R6
			mov.w	18(SP),	R7				;mov the counter value into R7

getNext:
			mov.w	#8, R11					;Set counter for next Mult
			clr.w	R8						;Clear R8 between mults
			cmp		#0, R7					;check if at end of array
			jeq		lend					;if at end jump
			mov.b	@R4+, R9				;get numA
			mov.b	@R5+, R10				;get numB

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
mult:
			cmp		#1,	R11					;check if at last bit to check sign
			jeq		checkNeg				;if last bit check if negative
			bit.b	#1, R10					;compare LSB to 1
			jne		sum						;if bit is 1 then jump to sum
			jmp		rotate					;else it is 0 gump to rotate

sum:
			add.w	R9, R8					;add R5 into holder R9
			rla.b	R9						;rotate left
			rra.b	R10						;rotate right
			dec		R11						;decrement counter
			jmp		mult					;jump back to mult

rotate:
			rla.b	R9						;Rotate the R5 Value to push GMB out
			rra.b	R10						;Rotate the R6 value right to push LMB out
			dec		R11						;decrement counter
			jmp		mult					;jump back to mult

checkNeg:
			bit		#1, R10					;check if bit is 1
			jne		negB					;if 1 it is negative
			bit		#1, R9					;check if bit is 1
			jne		negA					;if A is negative jump
			jmp		endMult					;if 0 then end

negA:
			sub.w	R9, R8					;subtract numA to B
			jmp		endMult					;end program

negB:
			bit		#1, R9					;check if bit is 1
			jne 	lend					;jump to end if neg also
			sub.w	R9, R8					;subtract numA to B
			jmp 	endMult					;jump to end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

endMult:
			add.w	R8, 0(R6)				;Add result of Mult to total
			dec		R7						;decrement counter
			jmp		getNext					;get next value

lend:
			pop		R11						;Restore regiser
			pop		R10						;Restore register
			pop		R9						;Restore register
			pop		R8						;Restore register
			pop		R7						;Restore register
			pop		R6						;Restore register
			pop		R5						;Restore register
			pop		R4						;Restore register
			ret								;return from sub routine
