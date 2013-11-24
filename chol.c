#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "matrix.h"

double** init_matrix(int size){
	double **matrix = (double **)malloc(size * sizeof(double *));
		
	int i;
	for(i = 0; i < size; i++)
		matrix[i] = (double *)malloc(size * sizeof(double));
		
	return matrix;
}

void print_matrix(double **matrix, int size){
	int i,j;	
	
	for (i = 0; i < size; i++){
		for (j = 0; j < size; j++){
			printf("\t%10.6f", matrix[i][j]);
		}
		printf("\n");
	}
	
	printf("\n");
}

void trans_copy (double ** source, double ** dest, int size){
	int i,j;
	
	for (i = 0; i < size; i++){
		for (j = 0; j <= i; j++){
			dest[i][j] = source[i][j];
		}
	}
}

void rand_matrix(double ** matrix, int n){
	srand(time(NULL));
	
	// Generate an n by n matrix A
	// Fill matrix w/ random values
	int i = 0 ;
	int j = 0 ;
	for( i=0; i<n; i++){
		for( j=0; j<n; j++){
			matrix[i][j] = rand()/(double) RAND_MAX *10;
		}
	}
}

double ** cholesky(double ** A, int n){
	// Copy matrix A and take only lower triangular part
	double ** L = init_matrix(n);
	trans_copy(A, L, n);
	
	int i,j,k;
	for (i = 0; i < n; i++){
		for (j = 0; j < n; j++){
			if (j < i){
				// Inner sum
				for (k = 0; k < j; k++){
					L[i][j] = L[i][j] - L[i][k] * L[j][k];
				}

				L[i][j] = L[i][j]/ L[j][j];
				
			} else if (j == i){
				// On diagonals, apply same inner sum as before
				
				for (k = 0; k< i; k++){
					L[i][i] = L[i][i] - L[i][k]*L[i][k];
				}
				
				L[i][i] = sqrt(L[i][i]);
			} else {
				// Upper triangular part of W - Clear it
				L[i][j] = 0;
			}
		}
	}
	
	return L;
}

int main(int argc, char **argv)
{
	// generate seed
	srand(time(NULL));

	// set matrix size
	int n = 4;
	
	// Generate random SPD matrix
	double** A = initialize(0, 10, n);
	printf("A = \n");
	print_matrix(A, n);

	// Apply Cholesky
	double** L = cholesky(A, n);
	printf("L = \n");
	print_matrix(L, n);
	
	// Test
	double ** LLT = matrixMultiply(L, transpose(L, n), n);
	printf("L*L^T = \n");
	print_matrix(LLT, n);

	return 0;
}

