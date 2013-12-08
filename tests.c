#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include "matrix.h"

typedef int bool;
enum { false, true };
struct timespec begin={0,0}, end={0,0};

time_t start, stop;

int main(int argc, char **argv)
{
    // generate seed
    srand(time(NULL));
    
    if (argc != 3)

    {   
       printf("You did not feed me arguments, I will die now :( ... \n");
       printf("Usage: %s [matrix size] [number of threads] \n", argv[0]);
       return 1;
    }
    int matrixSize = atoi(argv[1]);
    int threadsNumber = atoi(argv[2]);
    
    
    
    printf("Test basic output for a matrix of size %d:\n", matrixSize);
    // Generate random SPD matrix
    double** A = initialize(0, 10, matrixSize);
    /*printf("Chol matrix\n");
	print(A, matrixSize);*/
    double **L = initialize(0, 10, matrixSize);
    
    // Test Serial Program
    // Apply Serial Cholesky
    printf("Testing Serial implementation Output: \n");
    
    clock_gettime(CLOCK_MONOTONIC, &begin);
    L = cholSerial(A, matrixSize);
    clock_gettime(CLOCK_MONOTONIC, &end); //Get the current time.
    
    testBasicOutputOfChol(A,L,matrixSize);
    //Test execution time
    printf("The serial computation took %.5f seconds\n",
           ((double)end.tv_sec + 1.0e-9 * end.tv_nsec) - 
           ((double)begin.tv_sec + 1.0e-9 * begin.tv_nsec));
    
    
    //Testing OpenMP Program
    printf("Testing OpenMP implementation Output: \n");
    omp_set_num_threads(threadsNumber);
    copyMatrix(A,L,matrixSize);
    clock_gettime(CLOCK_MONOTONIC, &begin);
    cholOMP(L, matrixSize);
    clock_gettime(CLOCK_MONOTONIC, &end); //Get the current time.
    //testBasicOutputOfChol(A,L,matrixSize);
    
    //Test execution time
    printf("The OpenMP computation took %.5f seconds \n",
           ((double)end.tv_sec + 1.0e-9 * end.tv_nsec) - 
           ((double)begin.tv_sec + 1.0e-9 * begin.tv_nsec));

    printf("\n");
    return 0;
}
int testBasicOutputOfChol(double **A, double ** L, int n)
{
    double ** LLT = matrixMultiply(L, transpose(L, n), n);
  
    int i, j;
    float precision = 0.00000000001;
    for (i = 0; i < n; i++){
                for (j = 0; j < n; j++){
                        if( !(abs(LLT[i][j] - A[i][j]) < precision))
                        {
                         printf("FAILED\n"); //if it fails show the error
                         ComputeSumOfAbsError(A,LLT,n);
                         return 0;
                        }
                }
        }
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
    double** L = cholSerial(A, n);
    clock_t end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;
    printf("It took %f seconds\n", seconds);


}


void testErrorOfLinearSystemApplication(int matrixSize)
{
    printf("Test linear system application of Cholesky for matrix size %d:\n", matrixSize);
    double** A = initialize(0, 10, matrixSize);
    double *xTheo = (double *) malloc(matrixSize * sizeof(double));
    int index;
   
    for(index = 0; index < matrixSize ; index ++)
    {
        xTheo[index] = rand()/(double) RAND_MAX *10;
    }
    double *b = vectorMultiply(A, xTheo, matrixSize,matrixSize);
    //Apply Cholesky
    double** L = cholSerial(A, matrixSize);
    
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
     printVector(xExpr, matrixSize);
     printf("The sum of abs error is %10.6f \n", vectorComputeSumofAbsError(xTheo,xExpr,matrixSize));
}
