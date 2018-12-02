;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.

            .ref	DOT
            .ref	HW_DOT
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

			.data
arr1:		.byte	1, 2, 3, 4, 5, 6, 7, 8	;first array
arr2:		.byte	8, 7, 6, 5, 4, 3, 2, 1	;second array

res1:		.word	0h						;result location 1
res2:		.word	0h						;result location 2

counter:	.int	8h						;counter variable
;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------

main:
			push	#arr1					;push array1 onto the stack
			push	#arr2					;push array2 onto the stack
			push 	#res1					;push result location to stack
			push 	counter					;push counter to the stack
			call 	#DOT

			push 	#arr1					;push array1 onto the stack
			push	#arr2					;push array2 onto the stack
			push	#res2					;push result onto the stack
			push 	counter					;push counter onto stack
			call	#HW_DOT
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
            
