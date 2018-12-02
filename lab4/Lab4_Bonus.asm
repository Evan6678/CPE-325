;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.

			.data							;
myStr:		.string "Wel MSP430:)!", ''
myStr2:		.string ""
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------

main:		bis.b	#0FFh, &P1DIR			;Set P1 to output
			mov.w	#myStr, R4				;move input to R4
			mov.w	#myStr2, R5				;move input to R5
			clr.w	R8						;clear R8 for counter
			mov.b	@R5, R7					;point to first pos in new string

gnext:		mov.b	@R4+, R6				;move char into register
			cmp		#0, R6					;compare if null
			jeq		lend					;if null end
			cmp		#20h, R6				;
			jeq		include					;if a space then include
			cmp		#30h, R6				;compare if less than 0x30
			jl		delete					;if less then it is no good
			cmp		#3Ah, R6				;compare
			jl		include					;if less than it is a digit and ok
			cmp		#41h, R6				;compare
			jl		delete					;if less not a value we want
			cmp		#5Bh, R6				;
			jl		include					;if less then its an uppercase letter
			cmp		#61h, R6				;
			jl		delete					;if less we do not want it
			cmp		#7Bh, R6				;
			jl		include					;if less then is a lowercase letter
			jmp		gnext					;if here its a value we do not need

include:	inc.b	R7						;increment R7
			mov.b	R6, -1(R7)				;push value into R7
			jmp		gnext					;go back to loop

delete:		inc.w	R8						;increment R8
			jmp gnext						;jump to gnext

lend:   mov.b   R8, &P1OUT					; output digit counter
        bis.w   #LPM4,SR					; LPM4
        nop									;

;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
