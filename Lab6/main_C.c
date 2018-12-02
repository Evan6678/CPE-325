#include <msp430.h> 


/**
 * main.c
 */

//(N+1)*32768Hz
unsigned char SW1pressed = 0;       // SW1 status (0 not pressed, 1 pressed)
unsigned char SW2pressed = 0;       // SW2 status (0 not pressed, 1 pressed)

void main(void) {
    WDTCTL = WDTPW+WDTHOLD;         // Stop WDT

    P2DIR |= BIT2;                  // Set LED1 as output
    P2OUT = 0x00;                   // Clear LED1 status

    _EINT();                        // Enable interrupts
    P1IE |= 0x03;                   // P1IE.BIT0 interrupt enabled
    P1IES |= 0x03;                  // P1IES.BIT0 hi/low edge
    P1IFG &= ~0x03;                 // P1IFG.BIT0 is cleared

    SCFI0 = FN_2;                  //sets the range
    SCFQCTL = 30;                  //set to one MHZ

    unsigned int i = 0;
    while(1){

        for (i = 0; i < 50000; i ++)
        {}
            P2OUT ^= BIT2;


    };


}

// Port 1 interrupt service routine
#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR (void) {
    if(P1IFG &= BIT0)
    {
        if (SW1pressed == 0) {
            SW1pressed = 1;

            SCFI0 |= FN_4;
            SCFQCTL = 60;

            P1IFG &= ~BIT0;               // P1IFG.BIT0 is cleared
            P1IES &= ~BIT0;               // P1IES.BIT0 low/high edge
        } else if (SW1pressed == 1) {
            SW1pressed = 0;

            SCFI0 = FN_2;                  //sets the range
            SCFQCTL = 30;                  //set to one MHZ

            P1IFG &= ~BIT0;                // P1IFG.BIT0 is cleared
            P1IES |= BIT0;                 // P1IES.BIT0 hi/low edge
        }
    }

    else if(P1IFG &= BIT1)
    {
        if (SW2pressed == 0) {
            SW2pressed = 1;

            SCFI0 |= FN_2;
            SCFQCTL = 60;

            P1IFG &= ~BIT1;               // P1IFG.BIT0 is cleared
            P1IES &= ~BIT1;               // P1IES.BIT0 low/high edge
        } else if (SW2pressed == 1) {
            SW2pressed = 0;

            SCFI0 = FN_2;                  //sets the range
            SCFQCTL = 30;                  //set to one MHZ

            P1IFG &= ~BIT1;                // P1IFG.BIT0 is cleared
            P1IES |= BIT1;                 // P1IES.BIT0 hi/low edge
        }
    }
    else if((P1IFG &= BIT0) && (P1IFG &= BIT1))
    {
        if (SW1pressed == 0 && SW2pressed == 0) {
            SW1pressed = 1;
            SW2pressed = 1;

            SCFI0 |= FN_4;
            SCFQCTL = 121;

            P1IFG &= ~BIT1;
            P1IES &= ~BIT1;

            P1IFG &= ~BIT0;               // P1IFG.BIT0 is cleared
            P1IES &= ~BIT0;               // P1IES.BIT0 low/high edge
        } else if (SW1pressed == 1 && SW2pressed == 1) {
            SW1pressed = 0;
            SW2pressed = 1;

            SCFI0 = FN_2;                  //sets the range
            SCFQCTL = 30;                  //set to one MHZ

            P1IFG &= ~BIT1;
            P1IES |= BIT1;

            P1IFG &= ~BIT0;                // P1IFG.BIT0 is cleared
            P1IES |= BIT0;                 // P1IES.BIT0 hi/low edge
        }
    }
}
