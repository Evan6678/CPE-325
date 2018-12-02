/*------------------------------------------------------------------------------
SLAVE
 *------------------------------------------------------------------------------*/
#include "msp430x20x3.h"

#define LED_ON_STATE    0x31        // Character '1'
#define LED_OFF_STATE   0x30        // Character '0'
#define LED_NUL_STATE   0x00        // Character NULL - used for dummy write operation
#define LED_BLINK_STATE_1 0x32          // Character '2'
#define LED_BLINK_STATE_2 0x33          // Character '3'

#define LED_ON          0x01
#define LED_OFF         0x00

#define SET_BUSY_FLAG()   P1OUT |= 0x10
#define RESET_BUSY_FLAG() P1OUT &= ~0x10

#define SET_LED()       P1OUT |= 0x01
#define RESET_LED()     P1OUT &= ~0x01

unsigned char LEDState ;
unsigned char NextState;

void SPISetup(void)
{
    USICTL0 |= USISWRST;            // Set UCSWRST -- needed for re-configuration process
    USICTL0 |= USIPE5 + USIPE6 + USIPE7 + USIOE; // SCLK-SDO-SDI port enable,MSB first
    USICTL1 = USIIE;                // USI  Counter Interrupt enable
    USICTL0 &= ~USISWRST;           // **Initialize USCI state machine**
}

void InitComm(void)
{
    USICNT = 8;                     // Load bit counter, clears IFG
    USISRL = LEDState;              // Set LED state
    RESET_BUSY_FLAG();              // Reset busy flag
}

void LEdInit(unsigned char state)
{
    if (state == LED_OFF_STATE) {
        RESET_LED();
        LEDState = LED_OFF_STATE;
    }
    else {
        SET_LED();
        LEDState = LED_ON_STATE;
    }
    P1DIR |= 0x11;                  // P1.0,4 output
}

void SystemInit()
{
    WDTCTL = WDTPW + WDTHOLD;      // Stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ;         // Set DCO
    DCOCTL = CALDCO_1MHZ;
}

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;      // Stop watchdog timer
    LEdInit(LED_OFF_STATE);        // LED state initialization
    SPISetup();                    // USI module in SPI mode initialization
    InitComm();                    // Communication initialization

    while(1)
    {
        _BIS_SR(LPM0_bits + GIE );   // Enter LPM0 with interrupt

        switch (NextState) {
            case 0x00:              // Dummy operation; no change in the state
                break;
            default  :
                LEDState = NextState;   // New state
                break;
         }
        // Change the status of LED depending on the command
        if (LEDState == LED_OFF_STATE){
            TA0CTL = TASSEL_1 + MC_0;
            RESET_LED();
        }
        else if(LEDState == LED_BLINK_STATE_1)
        {
            //1Hz Blink
            TA0CCTL0 = CCIE;
            TA0CCR0 = 1980;
            TA0CTL = TASSEL_1 + MC_3;
        }
        else if(LEDState == LED_BLINK_STATE_2)
        {
            //5Hz Blink
            TA0CCTL0 = CCIE;
            TA0CCR0 = 430;
            TA0CTL = TASSEL_1 + MC_3;

        }
        else {
            TA0CTL = TASSEL_1 + MC_0;
            SET_LED();

        }

        USISRL = LEDState;          // Prepares reply to master with new state
        RESET_BUSY_FLAG();          // Clears busy flag - ready for new communication
       }
}

#pragma vector=USI_VECTOR
__interrupt void USI_ISR(void)
{
    SET_BUSY_FLAG();                // Set busy flag - slave is ready with a new communication
    NextState = USISRL;             // Read new command
    USICNT = 8;                     // Load bit counter for next TX
    _BIC_SR_IRQ(LPM0_bits);         // Exit from LPM0 on RETI
}

#pragma vector = TIMERA0_VECTOR
__interrupt void timerISR(void)
{
    P1OUT ^= BIT0;

}

