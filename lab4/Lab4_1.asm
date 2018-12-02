;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.

			.data
myStr:		.string "Welcome MSP430 Assembly", ''
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

main:	bis.b	#0FFh, &P1DIR				;Set P1 to output
		bis.b	#0FFh, &P2DIR				;Set P2 to output
		mov.w	#myStr, R4					;Move the string into R4
		clr.b	R5							;Clear the register that will be digit counter
		clr.b 	R6							;Clear the register that will be letter counter
gnext:	mov.b	@R4+, R7					;load next char into R4
		cmp		#0, R7						;compare to find NULL
		jeq		lend 						;if null jump to end
		cmp		#30h, R7					;check if less than any accepted ascii value
		jl		gnext						;if less than accepted get next
		cmp		#3Ah, R7					;now if its less than 3A it is a digit
		jl		incDigit					;Jump to digit incrementor if less than delimiter ascii
		cmp		#41h, R7					;if less then A and greater than 9 it is a bad value
		jl		gnext						;get next letter
		cmp		#5Bh, R7					;if less then this it is an upper case letter
		jl		incLetter					;if less increment letter
		jmp		gnext						;if reached here its greater then usable value

incLetter:
		inc.w	R6							;incrememnt counter
		jmp		gnext						;get next letter

incDigit:
		inc.w	R5							;increment counter
		jmp		gnext						;get next letter

lend:   mov.b   R5, &P1OUT					; output digit counter
		mov.b	R6, &P2OUT					; output letter counter
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
            .end
            
