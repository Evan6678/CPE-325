;-------------------------------------------------------------------------------
;   File:        Lab6_D3.asm
;   Description: The program demonstrates Press/Release using SW1 and LED1.
;                LED1 is initialized off. The main program enables interrupts
;                from P1.BIT0 (SW1) and remains in an infinite loop doing nothing.
;                P1_ISR implements debouncing and waits for a SW1 to be released.
;
;   Clocks:      ACLK = 32.768kHz, MCLK = SMCLK = default DCO = 2^20=1,048,576 Hz
;   Platform:    TI Experimenter's Board
;
;                 MSP430xG461x
;             -----------------
;         /|\|                 |
;          | |                 |
;          --|RST              |
;            |             P2.2|-->LED1(GREEN)
;            |             P1.0|<--SW1
;
;   Author:  Aleksandar Milenkovic, milenkovic@computer.org
;   Date:    September 14, 2018
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file

;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
            .def    SW1_ISR
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET:      mov.w   #__STACK_END, SP        ; Initialize stack pointer
StopWDT:    mov.w   #WDTPW|WDTHOLD, &WDTCTL ; Stop watchdog timer
;-------------------------------------------------------------------------------
Setup:
            bis.b   #006h, &P2DIR           ; Set P2.2 to output
                                            ; direction (0000_0100)
            bic.b   #006h, &P2OUT           ; Set P2OUT to 0x0000_0100
                                            ; (ensure LED1 is off)
            bis.w   #GIE, SR                ; Enable Global Interrupts
            bis.b   #003h, &P1IE            ; Enable Port 1 interrupt from bit 0
            bis.b   #003h, &P1IES           ; Set interrupt to call from hi to low
            bic.b   #003h, &P1IFG           ; Clear interrupt flag

InfLoop:
            jmp     $                       ; Loop here until interrupt

;----------------------------------------------------------------------------
; P1_0 (SW1) interrupt service routine (ISR)
;----------------------------------------------------------------------------
SW1_ISR:
            bic.b   #003h, &P1IFG           ; Clear interrupt flag

		    bit.b   #01h, &P1IN             ; Check if SW1 is pressed
           	jeq		Sw1						;if sw1 is pressed jump

           	bit.b	#02h, &P1IN				;Check SW2 if pressed
           	jeq		Sw2						;

            jnz     LExit                   ; If not zero, SW is not pressed

;----------------------------------------------------------------------------
; SW1
;----------------------------------------------------------------------------
Sw1:
                                            ; loop and check again
Debounce1:   mov.w   #2000, R15              ; Set to (2000 * 10 cc)
SW1_20ms:    dec.w   R15                     ; Decrement R15
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            jnz     SW1_20ms                 ; Delay over?
            bit.b   #00000001b,&P1IN        ; Verify SW1 is still pressed
            jnz     LExit                   ; If not, wait for SW1 press
LEDon:      bis.b   #004h,&P2OUT            ; Turn on LED1
SW1wait:    bit.b   #001h,&P1IN             ; Test SW1
            jz      SW1wait                 ; Wait until SW1 is released
            bic.b   #004,&P2OUT             ; Turn off LED1
			jmp LExit						; When done return
;----------------------------------------------------------------------------
; SW2
;----------------------------------------------------------------------------
Sw2:

Debounce2:  mov.w   #2000, R15              ; Set to (2000 * 10 cc)
SW2_20ms:   dec.w   R15                     ; Decrement R15
            nop
            nop
            nop
            nop
            nop
            nop
            nop
            jnz     SW2_20ms                ; Delay over?
            xor		#002h, &P2OUT			;turn on LED on or off
			jmp		LExit					; When done return

LExit:      reti                            ; Return from interrupt
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
            .sect   ".int20"                ;P1.x Vector
            .short  SW1_ISR
            .end
