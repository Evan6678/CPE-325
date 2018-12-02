#include <msp430.h> 


/**
 * main.c
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    //WDT interrupts
    _EINT();

    P2DIR |= BIT2;                  // Set LED1 as output
    P2OUT = 0x00;                   // Clear LED1 status


    //set the speaker direction and special function
    P3DIR = 0x20;
    P3SEL = 0x20;

    long long int i = 150000;



    while(1)
    {
        WDTCTL = WDT_ADLY_1000;
        IE1|= WDTIE;

        TB0CCTL4 = OUTMOD_4;
        TB0CTL = TBSSEL_2 + MC_1;
        TB0CCR0 = 0;

        for(i; i > 0; i--){};
        i = 150000;

        TB0CCTL4 = OUTMOD_4;
        TB0CTL = TBSSEL_2 + MC_1;
        TB0CCR0 = 400;


        for(i; i > 0; i--){};
        i = 150000;

        TB0CCTL4 = OUTMOD_4;
        TB0CTL = TBSSEL_2 + MC_1;
        TB0CCR0 = 600;

        WDTCTL = WDT_ADLY_250;
        IE1|= WDTIE;

        for(i; i > 0; i--){};

        i = 150000;

        TB0CCTL4 = OUTMOD_4;
        TB0CTL = TBSSEL_2 + MC_1;
        TB0CCR0 = 800;

        WDTCTL = WDT_ADLY_16;
        IE1|= WDTIE;


        for(i; i > 0; i--){};
        i = 150000;

        TB0CCTL4 = OUTMOD_4;
        TB0CTL = TBSSEL_2 + MC_1;
        TB0CCR0 = 1000;

        WDTCTL = WDT_ADLY_1_9;
        IE1|= WDTIE;

        for(i; i > 0; i--){};
        i = 150000;
    }

    return 0;
}


#pragma vector = WDT_VECTOR
__interrupt void watchdog_timer(void)
{
    P2OUT ^= BIT2;
}
