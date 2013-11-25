#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <stdlib.h>
#include "matrix.h"
#include <time.h>

typedef int bool;
enum { false, true };

int main(int argc, char **argv)
{
	// generate seed
	srand(time(NULL));
	testErrorOfLinearSystemApplication(100);
    //testBasicOutputOfSerialChol(4);
    //testTimeforSerialChol(50);
    //testTimeforSerialChol(1000);
    //testTimeforSerialChol(5000);
    printf("\n");
	return 0;
}
int testBasicOutputOfSerialChol(int n)
{
    // set matrix size
	
    printf("Test basic output for a matrix of size %d:\n", n);
    
	// Generate random SPD matrix
	double** A = initialize(0, 10, n);

	// Apply Cholesky
	double** L = cholesky(A, n);
	//printf("L = \n");
	//print_matrix(L, n);
	

	double ** LLT = matrixMultiply(L, transpose(L, n), n);
    int i, j;
    float precision = 0.0000001;
    for (i = 0; i < n; i++){
		for (j = 0; j < n; j++){
			if( !(abs(LLT[i][j] - A[i][j]) < precision))
			{
			    printf("FAILED\n");
			    return 0; 
			}
		}
	}
	
	/*printf("L*L^T = \n");
	print_matrix(LLT, n);*/
	printf("PASSED\n");
	return 1;
}

void testTimeforSerialChol(int n)
{
    printf("Test duration for serial version with matrix of size %d \n", n);
    // Generate random SPD matrix
   	double** A = initialize(0, 10, n);
    clock_t start = clock();
    // Apply Cholesky
	double** L = cholesky(A, n);
    clock_t end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    printf("It took %f seconds\n", seconds);


}

void testErrorOfLinearSystemApplication(int matrixSize)
{
    printf("Test linear system application of Cholesky for matrix size  %d:\n", matrixSize);
    double** A = initialize(0, 10, matrixSize);
    double *xTheo = (double *) malloc(matrixSize * sizeof(double));
    int index;
   
    for(index = 0; index < matrixSize ; index ++)
    {
        xTheo[index] = rand()/(double) RAND_MAX *10;
    }
    double *b = matrixVectorMultiply(A, xTheo, matrixSize);
    //Apply Cholesky
    double** L = cholesky(A, matrixSize);
    
    double *y = (double *) malloc(matrixSize * sizeof(double));
    // Forward-substitution part

    int i, j; 
    for(i = 0; i < matrixSize; i++){ 
        y[i] = b[i] ;
        for (j = 0; j < i ; j++){ 
            y[i] = y[i] - L[i][j] * y[j];
           } 
        y[i] = y[i] / L[i][i]; 
     } 
     
      // Back-substitution part
     double **LT = transpose(L, matrixSize);
     double *xExpr = (double *) malloc(matrixSize * sizeof(double));
     for(i = matrixSize - 1; i>=0; i--){ 
        xExpr[i] = y[i];
        for (j = i + 1 ; j < matrixSize; j++){
            xExpr[i] = xExpr[i] - LT[i][j]*xExpr[j]; 
        } 
        xExpr[i] = xExpr[i] / LT[i][i]; 
    }
     printf("x experimental is: \n");
     print_vector(xExpr, matrixSize);
     printf("The sum of abs error is %10.6f \n", computeSumofAbsError(xTheo,xExpr,matrixSize));
}
    
   


