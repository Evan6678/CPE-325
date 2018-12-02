/*--------------------------------------------------------------------------------
 *Author: Evan Stewart
 *Assignemnt: CPE 325 Lab 8
 *
 *Description: Use interrupts and flowcharts
--------------------------------------------------------------------------------*/
#include <msp430xG46x.h>
#include <stdio.h>

//output strings
char output1[26] = "Please enter the username:";
char output2[19] = "Enter the password:";
char output3[33] = "Incorrect username or password!!!";
char output4[21] = "Welcome to CPE 325!!!";

//validation keys
char usernameKey[] = "username";
char passwordKey[] = "password";

//length of validation keys
int lenOfUserKey = (sizeof(usernameKey) / sizeof(char))-1;
int lenOfPassKey = (sizeof(passwordKey) / sizeof(char))-1;

//length of user input strings
int lenOfUserInput = 0;
int lenOfPassInput = 0;

//user input strings
char usernameUserInput[100];
char passwordUserInput[100];

//key user selected
char pressedKey;

//flags to determine status of different work flows
int isPass = 0;
int WDTFlag = 0;

int checkUser(int i);
int checkPass(int i);
void validate();
void newLine();
void reset();
void watchdogTimer();
void printUsernameMessage();
void printIncorrectStatement();
void printCorrectStatement();

void main(void) {
    WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer

    P5DIR |= BIT1;                  // P5.1 is output;

    UCA0CTL1 |= UCSWRST;            // Set software reset during initialization
    P2SEL |= BIT4 + BIT5;           // Set UC0TXD and UC0RXD to transmit and receive
    UCA0CTL0 = 0;                   // USCI_A0 control register
    UCA0CTL1 |= UCSSEL_2;           // Clock source SMCLK
    UCA0BR0 = 54;                   // 1048576 Hz / 19200 = 54 | 5
    UCA0BR1 = 0;
    UCA0MCTL = 0x0A;                // Modulation
    UCA0CTL1 &= ~UCSWRST;           // Clear software reset

    IE2 |= UCA0RXIE;                // Enable USCI_A0 RX interrupt

    printUsernameMessage();

    _BIS_SR(LPM0_bits + GIE);      // enter low power mode
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIA0RX_ISR (void) {

    static int i = 0;
    pressedKey = UCA0RXBUF;

    //check if a timeout happened and reset the variable
    if(WDTFlag == 1)
    {
        WDTFlag = 0;
        i = 0;
    }

    if(isPass == 0)                     //if reading in user name
    {
        i = checkUser(i);
    }
    else                                //else reading in password
    {
        i = checkPass(i);
    }
}

int checkUser(int i)
{
    //if user hits enter key
    if (pressedKey == 0x0D)
    {
        //start timeout clock
        watchdogTimer();

        newLine();
        //ask the user statement to enter password
        for(i = 0; i <19; i++)
        {
            while(!(IFG2&UCA0TXIFG));
                UCA0TXBUF = output2[i];
        }
        newLine();
        //set flag to know time to read in password
        isPass = 1;
        //reset array incrementor to start at beginning of password array
        i = 0;
    }
    //if reached buffer length then stop accepting chars
    else if(lenOfUserInput == 100)
    {
        return i;
    }
    //else add keystroke to array
    else
    {
        //print out the char
        while( ! (IFG2 & UCA0TXIFG) );      // Wait until can transmit
        UCA0TXBUF = UCA0RXBUF;

        //add the char into the array and then increment
        usernameUserInput[i] = pressedKey;
        lenOfUserInput++;
        i++;
    }
    return i;
}

int checkPass(int i)
{
    //check if enter was pressed
    if (pressedKey == 0x0D)
    {
        //if pressed validate and reset vars
        newLine();
        validate();
        i = 0;
    }
    //if at end of buffer stop accepting
    else if(lenOfPassInput == 100)
    {
        return i;
    }
    //add char into array and print to screen
    else
    {
        while( ! (IFG2 & UCA0TXIFG) );      // Wait until can transmit
        UCA0TXBUF = '*';

        passwordUserInput[i] = pressedKey;
        lenOfPassInput++;
        i++;
    }
    return i;
}

void validate()
{
    int i;
    WDTCTL = WDTPW + WDTHOLD;  // Stop WDT

    //validate they are the same size of the accepted values
    if (lenOfUserInput !=  lenOfUserKey || lenOfPassInput != lenOfPassKey)
    {
        printIncorrectStatement();
        reset();
        return;
    }
    else
    {
        //validate that user name is correct
        for (i = 0; i < lenOfUserKey; i++)
        {
            if(usernameKey[i] != usernameUserInput[i])
            {
                printIncorrectStatement();
                reset();
                return;
            }
        }
        //validate that password is correct
        for (i = 0; i < lenOfPassKey; i++)
        {
            if(passwordKey[i] != passwordUserInput[i])
            {
                printIncorrectStatement();
                reset();
                return;
            }
        }
    }

    printCorrectStatement();
    reset();
}

void newLine()
{
    //add new line
    while(!(IFG2&UCA0TXIFG));
            UCA0TXBUF = '\n';
    //add a carrage return to start at beginning of line
    while(!(IFG2&UCA0TXIFG));
            UCA0TXBUF = '\r';
}

void printIncorrectStatement()
{
    int i;
    for(i = 0; i <33; i++)
    {
        while(!(IFG2&UCA0TXIFG));
            UCA0TXBUF = output3[i];
    }
}
void printCorrectStatement()
{
    int i;
    for(i = 0; i <21; i++)
    {
        while(!(IFG2&UCA0TXIFG));
            UCA0TXBUF = output4[i];
    }
}

void printUsernameMessage(){

    int i;

    //print output message to user
    for(i = 0; i <26; i++)
    {
        while(!(IFG2&UCA0TXIFG));
            UCA0TXBUF = output1[i];
    }
    newLine();
}

void watchdogTimer()
{
    WDTCTL = WDT_ADLY_1000;
    IE1 |= WDTIE;
}

// Watchdog Timer interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
    static int i = 0;

    if (i >= 30)
    {
        i = 0;
        WDTFlag = 1;
        reset();
        WDTCTL = WDTPW + WDTHOLD;  // Stop WDT
    }
    i++;
}

void reset()
{
    //reset all vars and flags to prepare for next iteration
    isPass = 0;
    lenOfUserInput = 0;
    lenOfPassInput = 0;
    newLine();
    printUsernameMessage();
}
