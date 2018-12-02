/******************************************************************************

Title: Lab 2-Matrix calculator
Author: Evan Stewart
Date: 8/22/2018
GitHub: https://github.com/Evan6678
Description: Extra credit for lab2. 4x4 matrix calculator

*******************************************************************************/

#include <stdio.h>


int main()
{
    
    //define both matrices
    const int matrix1[4][4] = {
        {1,1,1,1}, 
        {2,2,2,2}, 
        {3,3,3,3},
        {4,4,4,4}
    };
    
    const int matrix2[4][4] = {
        {4,4,4,4}, 
        {3,3,3,3}, 
        {2,2,2,2},
        {1,1,1,1}
    };
    
    int product[4][4];
    
    int row;
    int col;
    int inner;
    
    //conduct matrix multiplication
    for (row = 0; row < 4; row++)
    {
        for (col = 0; col < 4; col++)
        {
            product[row][col] = 0;
            for (inner = 0; inner < 4; inner++)
            {
                product[row][col] += matrix1[row][inner] * matrix2[inner][row];
            }
        }
    }
    
    //print matrix
    for (row = 0; row < 4; row++)
    {    
        for (col = 0; col < 4; col++){
            printf("%d  ", product[row][col]);
        }
        printf("\n");
    }

    return 0;
}







