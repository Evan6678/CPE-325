/*------------------------------------------------------------------------------
 * File:        Lab11_D3.c (CPE 325 Lab11 Demo code)
 * Function:    Sinusoidal wave with DAC (MPS430FG4618)
 * Description: This C program reconstructs the sinusoidal wave (y=1.25(1+sin(x)))
 *              from the samples using DAC and outputs at P6.6. WDT is used to
 *              give an interrupt for every ~0.064ms to wake up the CPU and
 *              feed the DAC with new value. Connect the oscilloscope to P6.6
 *              to observe the signal. The interval used to read the samples
 *              controls the frequency of output signal.
 * Clocks:      ACLK = LFXT1 = 32768Hz, MCLK = SMCLK = DCO = default (~1MHz)
 *              An external watch crystal between XIN & XOUT is required for ACLK
 *
 *                         MSP430xG461x
 *                      -------------------
 *                   /|\|              XIN|-
 *                    | |                 | 32kHz
 *                    --|RST          XOUT|-
 *                      |                 |
 *                      |        DAC0/P6.6|--> sine (10Hz)
 *                      |                 |
 * Input:       None
 * Output:      Sinusoidal wave with 10Hz frequency at P6.6
 * Author:      Aleksandar Milenkovic, milenkovic@computer.org
 *              Max Avula, ma0004@uah.edu
 *------------------------------------------------------------------------------*/
#include "msp430xG46x.h"
#include "cos_lut_512.h"
#include "square_lut_512.h"
#include "sawtooth_lut_512.h"
#include "triangle_lut_512.h"

void main(void) {
    unsigned int i = 0;
    WDTCTL = WDTPW + WDTHOLD;                       // Stop WDT
    ADC12CTL0 = REF2_5V + REFON;                    // Internal 2.5V ref on
    P1DIR &= ~0x03;                                 // Set SW1 and SW2 direction
    for (i=50000; i>0; i--);                        // Delay to allow Ref to settle
    __disable_interrupt();                          // Disable Interrupts
    DAC12_0CTL = DAC12IR + DAC12AMP_5 + DAC12ENC;   //Sets DAC12
    CCTL0 = CCIE;                                   // CCR0 interrupt enabled
    CCR0 = 82;                                      // 1/25 = 0.04s Sets Timer Freq (1048576*0.04sec/512)
    TACTL = TASSEL_2 + MC_1;                        // set clock to SMCLK, up mode

    while (1) {
        __bis_SR_register(LPM0_bits + GIE);         // Enter LPM0, interrupts enabled

        if((P1IN) == 0x03){
            DAC12_0DAT = SAWTOOTH_512[i];           // sawtooth
        }else if((P1IN) == 0x02){
            DAC12_0DAT = SQUARE_512[i];             // square
        }else if((P1IN) == 0x01){
            DAC12_0DAT = TRIANGLE_512[i];           // triangle
        }else{
            DAC12_0DAT = COS_512[i];                // cos
        }
        i=(i+1)%512;
        DAC12_0DAT &= 0xFFF;
    }
}

#pragma vector = TIMERA0_VECTOR
__interrupt void TA0_ISR(void)
{
    __bic_SR_register_on_exit(LPM0_bits);           // Exit LPMx, interrupts enabled
}

