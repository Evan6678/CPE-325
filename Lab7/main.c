#include <msp430.h> 


/**
 * main.c
 */

int SW1Pressed = 0;             //holder to determine if pressed or not
int SW2Pressed = 0;
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	//set direction of LED and clear
	P5DIR = 0x02;
	P5OUT = 0x00;

	//set SW1 and SW2 to input
	P1DIR = 0x00;

	//set the speaker direction and special function
	P3DIR = 0x20;
	P3SEL = 0x20;

	//set the timer B
	TB0CCTL4 = OUTMOD_4;
	TB0CTL = TBSSEL_2 + MC_1;
	TB0CCR0 = 852;

	//Set interrupts for Switches
	_EINT();
	P1IE = 0x03;        //enable interrupts for Port 1
	P1IES = 0x03;       //Set the edge case for SW1 and SW2 to Hi/low
	P1IFG = 0x00;       //clear all flags

}


#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR (void)
{
    //check switch 2
    if (P1IFG &= BIT1)
    {
        if(SW2Pressed == 0)
        {
            SW2Pressed = 1;        //set flag to 1
            P1IFG &= ~BIT1;        //clear the flag
            P1IES &= ~BIT1;        //go to low/hi Edge Select


            TB0CCTL4 = OUTMOD_4;
            TB0CTL = TBSSEL_2 + MC_1;
            TB0CCR0 = 758;
        }
        else
        {
            SW2Pressed = 0;        //set flag to 1
            P1IES |= BIT1;         //go to hi/low Edge Select
            P1IFG &= ~BIT1;         //clear the flag

            TB0CCTL4 = OUTMOD_4;
            TB0CTL = TBSSEL_2 + MC_1;
            TB0CCR0 = 852;
        }
    }

    //check switch 1
    else
    {
        if (SW1Pressed == 0)
        {
            SW1Pressed = 1;

            P1IFG &= ~BIT0;        //clear the flag
            P1IES &= ~BIT0;        //go to low/hi Edge Select

            WDTCTL = WDT_MDLY_32;   //make WDT turn on
            IE1|= WDTIE;            //Set interrupt
        }
        else
        {
            SW1Pressed = 0;
            P1IES |= BIT0;         //go to hi/low Edge Select
            P1IFG &= ~BIT0;         //clear the flag

            P5OUT = 0x00;           //turn off LED


            WDTCTL = WDTPW | WDTHOLD;   //Turn off the WDT
        }
    }
}

#pragma vector = WDT_VECTOR
__interrupt void watchdog_timer(void)
{
    static int i = 0;
    i++;
    if (i == 32)
    {
        P5OUT ^= 0x02;
        i = 0;
    }
}
