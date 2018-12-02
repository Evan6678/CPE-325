#include <msp430.h> 
#include <stdio.h>
#include <limits.h>
#include <float.h>

void printHeader();
void printDashLine();

int main(void)
{

    //print out the header
    printHeader();

    //NEW code
    printf("| %-25s| %-20d| %-25d| %-40d|\n", "char", sizeof(char), CHAR_MIN, CHAR_MAX);
    printf("| %-25s| %-20d| %-25d| %-40d|\n","short int", sizeof(short int), SHRT_MIN, SHRT_MAX);
    printf("| %-25s| %-20d| %-25d| %-40d|\n","int", sizeof(int), INT_MIN, INT_MAX);
    printf("| %-25s| %-20d| %-25ld| %-40ld|\n","long int", sizeof(long int), LONG_MIN, LONG_MAX);
    printf("| %-25s| %-20d| %-25d| %-40d|\n","unsigned long int", sizeof(unsigned long int), 0, ULONG_MAX);
    printf("| %-25s| %-20d| %-25lld| %-40lld|\n","long long int", sizeof(long long int), LLONG_MIN, LLONG_MAX);
    printf("| %-25s| %-20d| %-25d| %-40llu|\n","unsigned long long int", sizeof(unsigned long long int), 0, ULLONG_MAX);
    printf("| %-25s| %-20d| %-25f| %-40f|\n","float", sizeof(float), FLT_MIN, FLT_MAX);
    //printf("| %-25s| %-20d| %-25lf| %-40lf|\n","double", sizeof(double), DBL_MIN, DLB_MAX);
    //END

    return 0;
}

//=================================================
//Void print functions
//=================================================
void printHeader(){

    printDashLine();
    printf("| Data Type                | Size (in bytes)     | Minimum                  | Maximum                                 |\n");
    printDashLine();

}

void printDashLine(){
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
}



