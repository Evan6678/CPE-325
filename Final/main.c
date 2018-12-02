/*--------------------------------------------------------------------------------
 *Author: Evan Stewart
 *Assignemnt: CPE 325 Lab 8
 *
 *Description: Use interrupts and flowcharts
--------------------------------------------------------------------------------*/
#include <msp430xG46x.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//output strings
char greeting[] = "Welcome to MSP430 3x3 Matrix Calculator!";

char instructionsHeader [] = "Instructions:";
char instructionsBody [] = "Enter 2 3x3 Matrices. Enter your desired value then click the space bar to move to the next value.";
char instructionsBody2 [] = "Negative values are not allowed and the max value for a single position is 9,999";

char firstMatrix [] = "Please enter the first matrix:";
char secondMatrix [] = "Please enter the second matrix:";

char transposeHeader1[] = "Transpose for matrix 1:";
char transposeHeader2[] = "Transpose for matrix 2:";

char additionHeader[] = "Addition of both matrices:";
char subtractionHeader[] = "Subtraction of both matrices:";
char multHeader[] = "Multiplication of both matrices:";

char delimA = '[';
char delimB = ']';

//input buffer
char userInput[5];
int userInputCounter = 0;

//matrix input variables
int matrix1 [3][3];
int matrix2 [3][3];

//transpose
long int transpose1 [3][3];
long int transpose2 [3][3];

//addition
long int addition [3][3];

//subtract
long int subtraction[3][3];

//multiplication
unsigned long int multiplication[3][3];

//flags and counters
char writeMatrix2 = 0;
int matrixColCounter = 0;
int matrixRowCounter = 0;

//key user selected
char pressedKey;

//consts INV is invalid
const int INV = -1;

//output Buffer
char outputBuffer[10];
int numOfDigits = 0;

////////functions////////

//assembly calls
extern int HW_MULT(int *num1, int *num2, int *storage);

//prints
void printInstructions();
void newLine();
void putChar(char x);
void enterMatrixOneHeader();
void enterMatrixTwoHeader();
void printResult(long int arr[3][3]);

//calculations and processing
void reset();
void calculations();
void getTranspose();
void toArray(long int number);
void getAddition();
void getSubtraction();
void getMult();

//init functions
void initInputArray();
void initOutputArray();
void initMatrixStorage();

////////////////////////////

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

    //init matrix storage and buffer to NULL
    initMatrixStorage();
    initInputArray();
    initOutputArray();

    //loop variables
    int i = 0;

    //print greeting
    for(i = 0; i < sizeof(greeting); i++)
    {
        putChar(greeting[i]);
    }

    newLine();
    newLine();

    //Print out instructions
    printInstructions();

    //print first input header
    enterMatrixOneHeader();

    IE2 |= UCA0RXIE;                // Enable USCI_A0 RX interrupt
    _BIS_SR(LPM0_bits + GIE);      // enter low power mode
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIA0RX_ISR (void) {

    //get the user pressed key
    pressedKey = UCA0RXBUF;

    //use ascii values to remove any unwanted inputs
    if ( !((pressedKey < 0x20) || (pressedKey > 0x20 && pressedKey < 0x30) || (pressedKey >0x39)) )
    {
        //if the user hits the space bar save the current value buffer and prepare for the next
        if(pressedKey == 0x20)
        {
            //if == 0 then we need to save in matrix 1 else save in matrix 2
            if(writeMatrix2 == 0)
            {
                //check and validate that the user is not trying to input a empty string
                if(!(matrix1[matrixRowCounter][matrixColCounter] == INV))
                {
                    processSpace();
                }
            }
            else
            {
                if(!(matrix2[matrixRowCounter][matrixColCounter] == INV))
                {
                    processSpace();
                }
            }
        }
        //else continue to process and save valid inputs
        else
        {
            //this runs for the first input of this position to reset the variable from INV to 0 to show that
            //the user has input some value
            if(matrix1[matrixRowCounter][matrixColCounter] == INV && writeMatrix2 == 0)
            {
                matrix1[matrixRowCounter][matrixColCounter] = 0;
            }

            if(matrix2[matrixRowCounter][matrixColCounter] == INV && writeMatrix2 == 1)
            {
                matrix2[matrixRowCounter][matrixColCounter] = 0;
            }

            //saves value into the buffer
            if(userInputCounter < 4)
            {
                userInput[userInputCounter] = pressedKey;
                userInputCounter++;
                putChar(pressedKey);
            }
        }
    }
    else
    {
        //Do nothing. it is garbage input
    }
}

//Processing Functions/////////////////////////////////

//get multiplication matrix
void getMult()
{
    int i, j;
    for(i = 0; i < 3; ++i)
    {
        for(j = 0; j < 3; ++j)
        {
            HW_MULT(&matrix1[i][0], &matrix2[0][j], &multiplication[i][j]);
        }
    }
}

//find the subraction matrix
void getSubtraction()
{
    int i, j;
    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            subtraction[i][j] = matrix1[i][j] - matrix2[i][j];
        }
    }
}

//find the addition matrix
void getAddition()
{
    int i, j;
    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            addition[i][j] = matrix1[i][j] + matrix2[i][j];
        }
    }
}

//resets the values for next run
void reset()
{
    matrixRowCounter = 0;
    matrixColCounter = 0;
    writeMatrix2 = 0;
    userInputCounter = 0;

    initMatrixStorage();

    enterMatrixOneHeader();
}

//does all the handeling for getting and printing calculations
//this is the controller
void calculations()
{
    int i;
    getTranspose();

    newLine();
    newLine();

    for(i = 0; i < sizeof(transposeHeader1); i++)
    {
        putChar(transposeHeader1[i]);
    }

    printResult(transpose1);

    for(i = 0; i < sizeof(transposeHeader2); i++)
    {
        putChar(transposeHeader2[i]);
    }

    printResult(transpose2);

    for(i = 0; i < sizeof(additionHeader); i++)
    {
        putChar(additionHeader[i]);
    }

    getAddition();
    printResult(addition);

    for(i = 0; i < sizeof(subtractionHeader); i++)
    {
        putChar(subtractionHeader[i]);
    }

    getSubtraction();
    printResult(subtraction);

    for(i = 0; i < sizeof(subtractionHeader); i++)
    {
        putChar(multHeader[i]);
    }

    getMult();
    printResult(multiplication);
}

//gets the transpose
void getTranspose()
{
    int i, f, a = 3, b = 3;

    for( i = 0 ; i < a ; i++ )
    {
      for( f = 0 ; f < b ; f++ )
      {
         transpose1[f][i] = matrix1[i][f];
      }
    }

    for( i = 0 ; i < a ; i++ )
    {
      for( f = 0 ; f < b ; f++ )
      {
         transpose2[f][i] = matrix2[i][f];
      }
    }
}

//does all the processing for when the space bar is pressed
void processSpace()
{
    //determins which matrix and saves the value
    if(writeMatrix2 == 0)
    {
        matrix1[matrixRowCounter][matrixColCounter] = atoi(userInput);
        initInputArray();
        userInputCounter = 0;
    }
    else
    {
        matrix2[matrixRowCounter][matrixColCounter] = atoi(userInput);
        initInputArray();
        userInputCounter = 0;
    }

    matrixColCounter++;

    //if 3 then we need to move to next row in matrix
    if(matrixColCounter == 3)
    {
        matrixColCounter = 0;
        matrixRowCounter++;

        putChar(delimB);
        newLine();

        //if 3 then we need to move to next matrix
        if (matrixRowCounter == 3)
        {
            matrixRowCounter = 0;

            //if 0 then we need to get matrix 2
            if(writeMatrix2 == 0)
            {
                writeMatrix2 = 1;
                enterMatrixTwoHeader();
                putChar(delimA);
            }
            //else we have matrix 2 and need to process values
            else
            {
                //call calculations function
                calculations();
                reset();
            }
        }
        //if not end then add bracket
        else
        {
            putChar(delimA);
        }
    }
    //if not end we need to add space
    else
    {
        putChar(' ');
    }
}

//converts int to char array
void toArray(long int number)
{
    int i, y;

    //handle negative values
    //print a negative then calculate as a positive value
    if(number < 0)
    {
        putChar('-');
        number = abs(number);
    }

    //get the length of the int (123) = 3, (12) = 2
    numOfDigits = log10(number) + 1;

    //catch bad calc from log10 of 0 and set to 0
    if(numOfDigits == -32768)
    {
        numOfDigits = 0;
        outputBuffer[0] = 0x30;
    }

    //load the int into a char array
    for(i=0;i < numOfDigits; i++, number/=10)
    {
        y = number %  10;
        outputBuffer[i] = y + '0';
    }
}
/////////////////////////////////////////////////

//Init functions////////////////////////////////
void initInputArray()
{
    int i = 0;
    for (i = 0; i < sizeof(userInput); i++)
    {
        userInput[i] = NULL;
    }
}
void initOutputArray()
{
    int i = 0;
    for (i = 0; i < sizeof(outputBuffer); i++)
    {
        outputBuffer[i] = NULL;
    }
}

void initMatrixStorage()
{
    //reset the matrix storage
    int i = 0;
    int f = 0;

    for(i = 0; i < 3; i++)
    {
        for(f = 0; f < 3; f++)
        {
            matrix1 [i][f] = INV;
            matrix2 [i][f] = INV;

            transpose1 [i][f] = 0;
            transpose2 [i][f] = 0;
            addition [i][f] = 0;
            subtraction[i][f] = 0;
            multiplication[i][f] = 0;
        }
    }
}
///////////////////////////////////////////////////

//Print Functions//////////////////////////////////
void printResult(long int arr[3][3])
{
    int i, j, x;

    newLine();
    newLine();


    //prints results
    for (i = 0; i < 3; i++)
    {
        putChar(delimA);
        for (j = 0; j < 3; j++)
        {
            toArray(arr[i][j]);

            for(x = numOfDigits; x >= 0; x--)
            {
                putChar(outputBuffer[x]);
            }

            initOutputArray();

            if(j != 3)
            {
                putChar(' ');
            }
        }
        putChar(delimB);
        newLine();
    }

    newLine();
    newLine();
}

void putChar(char x)
{
    while(!(IFG2&UCA0TXIFG));
        UCA0TXBUF = x;
}

void newLine()
{
    putChar('\n');
    putChar('\r');
}

void printInstructions()
{
    int i = 0;

    for(i = 0; i < sizeof(instructionsHeader); i++)
    {
        putChar(instructionsHeader[i]);
    }

    newLine();
    newLine();

    for(i = 0; i < sizeof(instructionsBody); i++)
    {
        putChar(instructionsBody[i]);
    }

    newLine();

    for(i = 0; i < sizeof(instructionsBody2); i++)
    {
        putChar(instructionsBody2[i]);
    }
}

void enterMatrixOneHeader()
{
    newLine();
    newLine();

    int i = 0;

    for (i = 0; i < sizeof(firstMatrix); i++)
    {
        putChar(firstMatrix[i]);
    }

    newLine();

    putChar(delimA);
}

void enterMatrixTwoHeader()
{
    newLine();
    newLine();

    int i = 0;

    for (i = 0; i < sizeof(secondMatrix); i++)
    {
        putChar(secondMatrix[i]);
    }
    newLine();
}
/////////////////////////////////////////////////////////////
