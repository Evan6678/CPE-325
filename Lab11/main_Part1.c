/*------------------------------------------------------------------------------
 * File:        Lab11_D2.c (CPE 325 Lab11 Demo code)
 * Function:    Interfacing thumbstick (MPS430FG4618)
 * Description: This C program interfaces with an imaginary thumbstick that has
 *              x and y axis and output from 0 to 3V. The value of x and y axis
 *              is sent as percentage of power to the UAH serial app.
 * Clocks:      ACLK = LFXT1 = 32768Hz, MCLK = SMCLK = DCO = default (~1MHz)
 *              An external watch crystal between XIN & XOUT is required for ACLK
 *
 *                         MSP430xG461x
 *                      -------------------
 *                   /|\|              XIN|-
 *                    | |                 | 32kHz
 *                    --|RST          XOUT|-
 *                      |                 |
 *                      |     P2.4/UCA0TXD|------------>
 *                      |                 | 38400 - 8N1
 *                      |     P2.5/UCA0RXD|<------------
 *                      |                 |
 * Input:       Connect thumbstick to the board
 * Output:      Displays % of power in UAH serial app
 * Author:      Micah Harvey
 *------------------------------------------------------------------------------*/

#include <msp430xG46x.h>
#include <math.h>

void turn_off();
void sendData();
void UART_putchar(char);
void initializeUART();

volatile long int ADCXval, ADCYval, ADCZval;
volatile float Xper, Yper, Zper, NETG;

void main(void)
{
    int i;

    WDTCTL = WDTPW +WDTHOLD;            // Stop WDT
    initializeUART();                   // Setup UART for RS-232

    P6DIR &= ~0xA8;                     // Configure P6.3 and P6.7 as input pins
    P6SEL |= 0xA8;                      // Configure P6.3 and P6.7 as analog pins
    P3DIR |=0x20;                 // Set P3.5
    P3SEL |= 0x20;                // Special select

    //Set up timer to send ADC info to PC every 100 ms
    TACCR0 = 6554;                      // 6554 / 32768 Hz = 0.2s
    TACTL = TASSEL_1 + MC_1;            // ACLK, up mode
    TACCTL0 = CCIE;                     // Enabled interrupt

    //Set up ADC 12
    ADC12CTL0 = ADC12ON + SHT0_6 + MSC; // configure ADC converter
    ADC12CTL1 = SHP + CONSEQ_1;         // Use sample timer, single sequence
    ADC12MCTL0 = INCH_3;                // ADC A3 pin - Stick X-axis
    ADC12MCTL1 = INCH_7;                // ADC A7 pin - Stick Y-axis
    ADC12MCTL2 = INCH_5 + EOS;          // ADC A5 pin - Stick Z-axis
                                        // EOS - End of Sequence for Conversions
    ADC12IE |= 0x02;                    // Enable ADC12IFG.8
    for (i = 0; i < 0x3600; i++);       // Delay for reference start-up
    ADC12CTL0 |= ENC;                   // Enable conversions

    _EINT();

    while (1){
        ADC12CTL0 |= ADC12SC;               // Start conversions
        __bis_SR_register(LPM0_bits + GIE); // Enter LPM0

        Xper = ((ADCXval*3.0/4095.0)-1.5)/0.3;      // Calculate percentage outputs
        Yper = ((ADCYval*3.0/4095.0)-1.5)/0.3;
        Zper = ((ADCZval*3.0/4095.0)-1.5)/0.3;
        NETG = sqrt((Xper*Xper)+(Yper*Yper)+(Zper*Zper));

        if(NETG >= 2){
            TB0CCTL4 = OUTMOD_4;       // TB0 output is in toggle mode
            TB0CTL = TBSSEL_2 + MC_1;  // SMCLK is clock source, UP mode
            TB0CCR0 = 893;              // (1/587 * 2^20)/2 = 893
        }else{
            TB0CTL = TBSSEL_2 + MC_0;
        }

    }
}

///////////////////////////////////////////////////////////////////////////////
// User-defined function definitions
///////////////////////////////////////////////////////////////////////////////

void UART_putchar(char c)
{
    while(!(IFG2 & UCA0TXIFG));         // Wait for previous character to be sent
    UCA0TXBUF = c;                      // Send byte to the buffer for transmitting
}

//Initializes UART control registers for transmitting data, as well as baud rate
void initializeUART(void)
{
    P2SEL |= BIT4 + BIT5;               // Set up Rx and Tx bits
    UCA0CTL0 = 0;                       // Set up default RS-232 protocol
    UCA0CTL1 |= BIT0 + UCSSEL_2;        // Disable device, set clock
    UCA0BR0 = 27;                       // 1048576 Hz / 38400 = 54.5
    UCA0BR1 = 0;
    UCA0MCTL = 0x94;
    UCA0CTL1 &= ~BIT0;                  // Start UART device
}

void sendData(void)
{
    int i;

    //Send packet via RS-232
    UART_putchar(0x55);                 // Send header

    //Use character pointers to send one byte of float X and Y value at a time
    char *xpointer=(char *)&Xper;
    char *ypointer=(char *)&Yper;
    char *zpointer=(char *)&Zper;

    //Send x percentage float one byte at a time
    for(i=0; i<4; i++){
        UART_putchar(xpointer[i]);
    }

    //Send y percentage float one byte at a time
    for(i=0; i<4; i++){
        UART_putchar(ypointer[i]);
    }
    //Send  percentage float one byte at a time
    for(i=0; i<4; i++){
       UART_putchar(zpointer[i]);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Interrupt service routines
///////////////////////////////////////////////////////////////////////////////

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR(void)
{
    ADCXval = ADC12MEM0;                  // Move results, IFG is cleared
    ADCYval = ADC12MEM1;
    ADCZval = ADC12MEM2;
    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
}

#pragma vector = TIMERA0_VECTOR
__interrupt void timerA_isr()
{
    sendData();                           // Send data to serial app
    __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
}
