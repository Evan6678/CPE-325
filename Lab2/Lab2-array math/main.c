/******************************************************************************

Title: Lab 2-Array Arithmatic calculator
Author: Evan Stewart
Date: 8/22/2018
GitHub: https://github.com/Evan6678
Description: Getting the sums and dot product of 2 arrays

*******************************************************************************/

#include <stdio.h>

void printArray(int [4]);

int main()
{
    //decalare all needed veriables
    const int array1[4] = {-1, 2, 5, 3};
    const int array2[4] = {20, -8, 4, 1};
    
    int sumArray[4];
    int productArray[4];
    int dotProduct = 0;
    
    int counter = 3;
    
    //get the sum array for output and the product array to assist in dot product
    for(counter; counter >= 0; counter--){
        sumArray[counter] = array1[counter] + array2[counter];
        productArray[counter] = array1[counter] * array2[counter];
    }
    
    counter = 3;
    
    //calculate the dot product
    for(counter; counter >= 0; counter--){
        dotProduct += productArray[counter];
    }
    
    //call function to print arrays
    printf("Input Array1: ");
    printArray(&array1);
    printf("Input Array2: ");
    printArray(&array2);
    printf("Sum of arrays: ");
    printArray(&sumArray);
    
    //print dot product 
    printf("Dot Product of the arrays: [%d]", dotProduct);

    return 0;
}


void printArray(int array[4]){
    
    int counter = 0;
    
    printf("[");
    
    for(counter; counter <= 3; counter++){
        printf("%d ", array[counter]);
    }
    
    printf("]\n");
}





