/*------------------------------------------------------------------------------
 * File:        Lab10_D1.c (CPE 325 Lab10 Demo code)
 * Function:    SPI Interface (MPS430Fg4618)
 * Description: Using the MSP-EXP430FG4618 Development Tool establish a data
 *              exchange between the MSP430FG4618 and MSP430F2013 devices using
 *              the SPI mode. The MSP430FG4618 uses the USCI module while the
 *              MSP430F2013 uses the USI module. MSP430FG4618 communicates with
 *              PC via RS232 module using USCI Serial Communication peripheral
 *              interface. This program takes user prompts the user to input a
 *              choice to turn ON or OFF the LED3 located on MSP430F2013. The
 *              user choice is communicated to MSP430FG4618 (master) via  USCI
 *              serial interface and the corresponding action is communicated
 *              to MSP430F2013(slave) via SPI. Based on the user choice,
 *              MSP430F2013 will turn ON or OFF the LED3. This is the master code
 *              that runs on MSP430FG4618.
 *                               Slave                    Master
 *                            MSP430F2013              MSP430FG4618
 *                        -----------------          -----------------
 *                       |              XIN|-    /|\|              XIN|-
 *                       |                 |      | |                 | 32kHz xtal
 *                       |             XOUT|-     --|RST          XOUT|-
 *                       |                 |        |                 |
 *                 LED <-|P1.0             |        |                 |
 *                       |         BF /P1.4|------->|P3.0/BF          |
 *                       |         SDI/P1.7|<-------|P3.1/UCB0SIMO    |
 *                       |         SDO/P1.6|------->|P3.2/UCB0SOMI    |
 *                       |        SCLK/P1.5|<-------|P3.3/UCB0CLK     |
 *
 * Clocks:      ACLK = LFXT1 = 32768Hz, MCLK = SMCLK = DCO = default (~1MHz)
 *              An external watch crystal between XIN & XOUT is required for ACLK
 * Instructions: 1. Set the following parameters in HyperTerminal
 *                  Port :        COM1
 *                  Baud rate :   19200
 *                  Data bits:    8
 *                  Parity:       None
 *                  Stop bits:    1
 *                  Flow Control: None
 *              2. This lab requires to configure the USI module of MSP430F2013
 *                 as slave and MSP430FG4618 as master in SPI mode.
 *              3. Connect the following jumpers on header 1 (H1) on the
 *                 experimenter's board.  [1-2], [3-4], [5-6], [7-8]
 *                          H1
 *                        _______
 *                      1|-------|2
 *                      3|-------|4
 *                      5|-------|6
 *                      7|-------|8
 *                       |_______|
 *                  Jumper must be present on PWR1, PWR2 and JP2.
 *
 *              4. Download and run this code by the connecting the FET debugger
 *                 to JTAG2 on the experimenter's board.
 *              5. Make sure the device selected is MSP430F2013 in the General
 *                 Options of CCS.
 *
 * Input:       Character y or n from the user
 * Output:      Turn on or off the LED3 and display the status on HyperTerminal
 *
 * MASTER
 *------------------------------------------------------------------------------*/
#include  "msp430xG46x.h"
#include  <stdio.h>

#define LED_ON_STATE    0x31            // Character '1'
#define LED_OFF_STATE   0x30            // Character '0'
#define LED_BLINK_STATE_1 0x32          // Character '2'
#define LED_BLINK_STATE_2 0x33          // Character '3'


#define LED_NUL_STATE   0x00            // Character NULL - used for dummy write operation

#define LED_ON          0x01
#define LED_OFF         0x00

unsigned char ch;                       // Hold char from UART RX
unsigned char rx_flag;                  // Receiver rx status flag

char gm1[] = "LED3 Control Program";
char gm2[] = "Alphabets are ignored";
char ledON [] = "Command Sent to LED3: ON";
char ledOFF [] = "Command Sent to LED3: OFF";
char led1Hz [] = "Command Sent to LED3: Blinking at 1Hz";
char led5Hz [] = "Command Sent to LED3: Blinking at 5Hz";
char ledState[] = "State of LED3: ";

void SPISetup(void)
{
    UCB0CTL0 = UCMSB + UCMST + UCSYNC;  // Sync. mode, 3-pin SPI, Master mode, 8-bit data
    UCB0CTL1 = UCSSEL_2 + UCSWRST;      // SMCLK and Software reset
    UCB0BR0 = 0x02;                     // Data rate = SMCLK/2 ~= 500kHz
    UCB0BR1 = 0x00;
    P3SEL |= BIT1 + BIT2 + BIT3;        // P3.1,P3.2,P3.3 option select
    UCB0CTL1 &= ~UCSWRST;               // **Initialize USCI state machine**
}

unsigned char SPIGetState(void)
{
    while((P3IN & 0x01));               // Verifies busy flag
    IFG2 &= ~UCB0RXIFG;
    UCB0TXBUF = LED_NUL_STATE;          // Dummy write to start SPI
    while (!(IFG2 & UCB0RXIFG));        // USCI_B0 TX buffer ready?
    return UCB0RXBUF;
}

void SPISetState(unsigned char State)
{
    while(P3IN & 0x01);                 // Verifies busy flag
    IFG2 &= ~UCB0RXIFG;
    UCB0TXBUF = State;                  // Write new state
    while (!(IFG2 & UCB0RXIFG));        // USCI_B0 TX buffer ready?
}

void UART0_putchar(char c)
{
    // Wait for previous character to transmit
    while (!(IFG2 & UCA0TXIFG));
    UCA0TXBUF = c;
}

void Serial_Initialize(void)
{
    P2SEL |= BIT4+BIT5;                 // Set UC0TXD and UC0RXD to transmit and receive data
    UCA0CTL1 |= BIT0;                   // Software reset
    UCA0CTL0 = 0;                       // USCI_A0 control register
    UCA0CTL1 |= UCSSEL_2;               // Clock source SMCLK - 1048576 Hz
    UCA0BR0=54;                         // Baud rate - 1048576 Hz / 19200
    UCA0BR1=0;
    UCA0MCTL=0x0A;                      // Modulation
    UCA0CTL1 &= ~BIT0;                  // Software reset
    IE2 |=UCA0RXIE;                     // Enable USCI_A0 RX interrupt
}

void main(void)
{
    WDTCTL = WDTPW+WDTHOLD;             // Stop watchdog timer
    Serial_Initialize();
    SPISetup();
    _EINT();                            // Enable global interrupts

    int z, i;
    for(z = 100; z > 0; z--);           // Delay to allow baud rate stabilize

    // Greeting Message
    for(i = 0; i < 20; i++) {
        ch = gm1[i];
        UART0_putchar(ch);              // Print the greeting message on hyperterminal
    }

    UART0_putchar('\n');                // Newline
    UART0_putchar('\r');                // Carriage return

    while(1) {

        while(!(rx_flag & 0x01) );      // Wait until receive the character from HyperTerminal
        rx_flag = 0;                    // Clear rx_flag

        if( !((ch < 0x30) || (ch > 0x34 && ch < 0x41) || (ch > 0x5A && ch < 0x61) || (ch > 0x7A)) )
            switch (ch) {

                case  '1':

                    SPISetState(LED_ON_STATE);

                    for(i = 1000; i > 0;i--);    // Delay

                    UART0_putchar('\n');                // Newline
                    UART0_putchar('\r');                // Carriage return

                    for(i = 0; i < 24; i++) {
                        UART0_putchar(ledON[i]);
                    }

                    UART0_putchar('\n');                // Newline
                    UART0_putchar('\r');                // Carriage return

                    break;

                case  '0':

                    SPISetState(LED_OFF_STATE);

                    for(i = 1000; i > 0;i--);    // Delay

                    UART0_putchar('\n');                // Newline
                    UART0_putchar('\r');                // Carriage return

                    for(i = 0; i < 25; i++) {
                        UART0_putchar(ledOFF[i]);
                    }

                    UART0_putchar('\n');                // Newline
                    UART0_putchar('\r');                // Carriage return

                    break;

                case  '2':

                    SPISetState(LED_BLINK_STATE_1);

                    for(i = 1000; i > 0;i--);    // Delay

                    UART0_putchar('\n');                // Newline
                    UART0_putchar('\r');                // Carriage return

                    for(i = 0; i < 37; i++) {
                        UART0_putchar(led1Hz[i]);
                    }

                    UART0_putchar('\n');                // Newline
                    UART0_putchar('\r');                // Carriage return

                    break;

                case  '3':

                    SPISetState(LED_BLINK_STATE_2);

                    for(i = 1000; i > 0;i--);    // Delay

                    UART0_putchar('\n');                // Newline
                    UART0_putchar('\r');                // Carriage return

                    for(i = 0; i < 37; i++) {
                        UART0_putchar(led5Hz[i]);
                    }

                    UART0_putchar('\n');                // Newline
                    UART0_putchar('\r');                // Carriage return

                    break;

                case  '4':

                    UART0_putchar('\n');                // Newline
                    UART0_putchar('\r');                // Carriage return

                    for(i = 0; i < 15; i++) {
                        UART0_putchar(ledState[i]);
                    }

                    for(i = 1000; i > 0;i--);    // Delay
                    UART0_putchar(SPIGetState());// Get the current state of LED and print

                    UART0_putchar('\n');                // Newline
                    UART0_putchar('\r');                // Carriage return
                    break;

                default :
                    for(i = 0; i < 21; i++) {
                        ch = gm2[i];
                        UART0_putchar(ch);       // Print the greeting message on hyperterminal
                    }
                    UART0_putchar('\n');         // Newline
                    UART0_putchar('\r');         // Carriage return
                    break;
        }


    }
}

// Interrupt for USCI Rx
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIB0RX_ISR (void)
{
    ch = UCA0RXBUF;                     // Character received is moved to a variable
    rx_flag=0x01;                       // Signal main function receiving a char
}
