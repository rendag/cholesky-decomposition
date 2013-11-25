#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void print(double **matrix, int matrixSize);
double** matrixAddition(double **matrix1, double **matrix2, int matrixSize);
double** matrixMultiply(double **matrix1, double **matrix2, int matrixSize);
double* vectorMultiply(double **matrix, double *vector, int matrixSize, int vectorSize);
double** transpose(double **matrix, int matrixSize);
double** initialize(int minValue, int maxValue, int matrixSize);

//Print a square matrix.
void print(double **matrix, int matrixSize){
        int i, j;
        for (i = 0; i < matrixSize; i++) {
                for (j = 0; j < matrixSize; j++) {
                        printf("%.2f\t", matrix[i][j]);
                }
                printf("\n");
        }
        printf("\n");
}

//Multiply two square matrices of the same size.
double** matrixMultiply(double **matrix1, double **matrix2, int matrixSize){
        
        //Allocates memory for a matrix of doubles.
        int i, j, k;
        double **matrixOut = (double **)malloc(matrixSize * sizeof(double*));
        for (i = 0; i < matrixSize; i++){
                matrixOut[i] = (double *) malloc(matrixSize * sizeof(double));
        }
        
        double result = 0;
        //Fill each cell of the matrix output.
        for(i = 0 ; i < matrixSize ; i++){
                for (j = 0; j < matrixSize ; j++){
                        //Multiply each row of matrix 1 with each column of matrix 2.
                        for(k = 0; k < matrixSize; k++){
                                result += matrix1[i][k] * matrix2[k][j];
                        }
                        
                        matrixOut[i][j] = result;
                        result = 0; //Reset;
                }
        }        
        
        return matrixOut;                
}

//Add two square matrices of the same size.
double** matrixAddition(double **matrix1, double **matrix2, int matrixSize){
        //Allocates memory for a matrix of doubles.
        int i, j;
        double **matrixOut = (double **)malloc(matrixSize * sizeof(double*));
        for (i = 0; i < matrixSize; i++){
                matrixOut[i] = (double *) malloc(matrixSize * sizeof(double));
        }        
        
        //Fill each cell of the matrix output.
        for(i = 0 ; i < matrixSize ; i++){
                for (j = 0; j < matrixSize ; j++){
                        matrixOut[i][j] = matrix1[i][j] + matrix2[i][j];
                }
        }        
        
        return matrixOut;                
}

//Multiply a square matrix by a vector. Return null if failure.
double* vectorMultiply(double **matrix, double *vector, int matrixSize, int vectorSize){
        
        double *result = (double *) malloc(matrixSize * sizeof(double));
        
        if(vectorSize != matrixSize){
                return NULL;
        }
        
        int i, j;
        double sum = 0.0;
        //Multiplication.
        for(i = 0 ; i < matrixSize ; i++){
                for (j = 0; j < matrixSize ; j++){
                        sum += matrix[i][j] * vector[j];
                }
                result[i] = sum;
                sum = 0; //Reset.
        }        
        
        return result;
}

//Return the transpose of a square matrix.
double** transpose(double **matrix, int matrixSize){
        
        //Allocates memory for a matrix of doubles.
        int i, j;
        double **matrixOut = (double **)malloc(matrixSize * sizeof(double*));
        for (i = 0; i < matrixSize; i++){
                matrixOut[i] = (double *) malloc(matrixSize * sizeof(double));
        }
        
        //Transpose the matrix.
        for(i = 0 ; i < matrixSize ; i++){
                for (j = 0; j < matrixSize ; j++){
                        matrixOut[i][j] = matrix[j][i];
                }
        }        
        
        return matrixOut;                
}


//Create a real positive-definite matrix.
double** initialize(int minValue, int maxValue, int matrixSize){
        
    //Allocates memory for a matrices of doubles.
	int i, j;
	double **matrix = (double **)malloc(matrixSize * sizeof(double*));
	double **identity = (double **)malloc(matrixSize * sizeof(double*));
	for (i = 0; i < matrixSize; i++){
		matrix[i] = (double *) malloc(matrixSize * sizeof(double));
		identity[i] = (double *) malloc(matrixSize * sizeof(double));
	}	
	
	//Creates an upper-triangular matrix of random numbers between minValue and maxValue.
	//Creates an identity matrix multiplied by maxValue.		
	double random;
	for(i = 0 ; i < matrixSize ; i++){
		
		identity[i][i] = maxValue * matrixSize;	
		
		for(j = 0 ; j < matrixSize ; j++){
			
			random = (maxValue - minValue) * 
				((double)rand() / (double)RAND_MAX) + minValue;
			if(random == 0.0){
				random = 1.0; //Avoid division by 0.
			}
			matrix[i][j] = random;
		}
	}	
		
	//Transform to positive-definite.
	double **transposed = transpose(matrix, matrixSize);	
	matrix = matrixAddition(matrix, transposed, matrixSize);
	matrix = matrixAddition(matrix, identity, matrixSize);	
	
	return matrix;
}


int test (int argc, char **argv){
        srand((unsigned)time(0));
        if (argc < 4){ //Check there is at least 3 inputted argument.
                printf("Error. Not enough arguments.");
                return -1;
        }

        int matrixSize = atoi(argv[1]);        
        if(matrixSize < 1 || matrixSize > 5000){
                printf("Error. Matrix size invalid. Input value from 1 to 5000.");
                return -1;
        }        
        
        int minValue = atoi(argv[2]);
        int maxValue = atoi(argv[3]);
        if(minValue > maxValue){
                printf("Error. Min value smaller than max value.");
                return -1;
        }                
        
        /*TESTING*/
        double *vectorTest = (double *) malloc(matrixSize * sizeof(double));
        vectorTest[0] = 1.2; vectorTest[1] = 5.67; vectorTest[2] = -3.4;
        
        double **matrix = initialize(minValue, maxValue, matrixSize);                
        printf("Matrix 1:\n");
        print(matrix, matrixSize);        
        
        double **matrix2 = initialize(minValue, maxValue, matrixSize);
        printf("Matrix 2:\n");
        print(matrix2, matrixSize);                
        
        printf("Vector multiplication with matrix 1:\n");
        vectorTest = vectorMultiply(matrix, vectorTest, matrixSize, 3);
        int i;
        for(i = 0 ; i < 3; i++){
                printf("%.2f ", vectorTest[i]);
        }
        printf("\n\n");
        
        printf("Transpose of 1:\n");
        print(transpose(matrix, matrixSize), matrixSize);
        printf("Addition:\n");
        print(matrixAddition(matrix, matrix2, matrixSize), matrixSize);
        printf("Multiplication:\n");
        print(matrixMultiply(matrix, matrix2, matrixSize), matrixSize);
        /*END TESTING*/
        
        return 0;
}
