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
myStr:		.string "Welcome to MSP430 Assembly!", ''
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

main:
			bis.b	#0FFh, &P1DIR			;Set p1 to output
			bis.b	#0FFh, &P2DIR			;Set p2 to output
			mov.w	#myStr, R4				;move string into R4
			clr.b	R5						;clear R5 to hold temp uppercase value
			clr.b	R6						;clear R6 to hold the counter

gnext:		mov.b	@R4+, R7				;Load next char into R6
			cmp		#0, R7					;Compare to see if NULL
			jeq		lend					;if null end
			cmp		#61h, R7				;Compare 0x61 to R6
			jl		gnext					;if less than this not6 a lower case letter
			cmp		#7Bh, R7				;compare to R6 max accepted ascii value
			jl		swap					;if less than it is a lower case value
			jmp		gnext					;if here it is above accepted value, grab next

swap:		sub.w	#20h, R7				;Subtract 0x20 to make uppercase
			mov.b	R7, -1(R4)				;replace lower case value with upper case
			inc.w	R6						;increment the counter
			jmp		gnext					;jump back to gnext

                                            
lend:   mov.b   R4, &P1OUT					; output digit counter
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
            
