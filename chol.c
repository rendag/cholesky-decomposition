#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>
#include "matrix.h"

typedef int bool;
#define true 1
#define false 0

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

double ** choleskyMPI(double ** A, int n, int argc, char ** argv){
	// Copy matrix A and take only lower triangular part
	double ** L = init_matrix(n);
	trans_copy(A, L, n);
	
	int npes, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	// For each column
	int i, j, k;
	for (j = 0; j < n; j++) {
	
		/*
		 * Step 1:
		 * Update the diagonal element
		 */

		if (rank == 0) {
			for (k = 0; k < j; k++) {
				L[j][j] = L[j][j] - L[j][k] * L[j][k];
			}

			L[j][j] = sqrt(L[j][j]);
		}

		// Broadcast new value to other processes
		MPI_Bcast(&L[j][j], 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
		// Wait until everyone has updated the diagonal element
		MPI_Barrier(MPI_COMM_WORLD);

		/*
		 * Step 2:
		 * Update the elements below the diagonal element
		 */

		// Divide the rest of the work
		int numElements = n - (j+1);
		int elementsPerProcess = numElements/npes;
		int mymin, mymax;
		int extraWork;
		bool compute = true;
		
		// Check if there are more processes than the number of
		// elements to update
		if (numElements < npes) {
		
			if ((rank+1) > numElements) {
				// Tell the unnecessary processes to take a break
				compute = false;
			}
			else {
				// Assign one element to each process
				mymin = (j+1) + rank;
				mymax = mymin + 1;
			}
		}
		// There are equal or more elements to compute than the 
		// number of processes
		else {
			mymin = (j+1) + rank * elementsPerProcess;
			mymax = mymin + elementsPerProcess;

			// Compute the excess of work
			extraWork = numElements % npes;
		}
		
		if (compute == true) {
			// Update the value of the elements assigned to the
			// current process
			for (i = mymin; i < mymax; i++) {
				for (k = 0; k < j; k++) {
					L[i][j] = L[i][j] - L[i][k] * L[j][k];
				}
			
				L[i][j] = L[i][j] / L[j][j];
			}	

			// Check if there's extra work to be done
			if (extraWork > 0) {
				// Assign one extra element to each process 
				// starting with P0
				if (rank < extraWork) {
					i = (j+1) + elementsPerProcess*npes + rank;
					
					// Update the value of the extra element
					for (k = 0; k < j; k++) {
						L[i][j] = L[i][j] - L[i][k] * L[j][k];
					}
			
					L[i][j] = L[i][j] / L[j][j];
				}
			}
		}
		
		/*
		 * Step 3:
		 * Broadcast each process' results to all other processes
		 */
		
		int proc;
		
		// Check if there are more processes than updated elements
		if (numElements < npes) {
			proc = 0;
			
			// Broadcast each process' work
			for (i = j+1; i < n; i++) {
				MPI_Bcast(&L[i][j], 1, MPI_DOUBLE, proc, MPI_COMM_WORLD);
				proc++;
			}
		}
		// There's more updated elements than processes
		else {
			proc = 0;
			k = 0;
			
			// Broadcast each process' work
			for (i = j+1; i < (j+1) + (elementsPerProcess*npes); i++) {
				if (k == elementsPerProcess) {
					k = 0;
					proc++;
				}
					
				MPI_Bcast(&L[i][j], 1, MPI_DOUBLE, proc, MPI_COMM_WORLD);
				k++;
			}
			
			proc = 0;
			
			// Broadcast the extrawork 
			for (i = j+1 + elementsPerProcess * npes; i < n; i++) {
				MPI_Bcast(&L[i][j], 1, MPI_DOUBLE, proc, MPI_COMM_WORLD);
				proc++;
			}
		}
		
		// Wait until every process is done updating its matrix
		MPI_Barrier(MPI_COMM_WORLD);
	}
	
	MPI_Finalize();
	
	return L;
}

int main(int argc, char **argv)
{
	// generate seed
	srand(time(NULL));

	// set matrix size
	int n = 10;

	// Generate random SPD matrix
	double** A = initialize(0, 10, n);
	printf("A = \n");
	print_matrix(A, n);
	
	// Apply Cholesky
	double** L = choleskyMPI(A, n, argc, argv);
	printf("L = \n");
	print_matrix(L, n);
	
	// Test
	double ** LLT = matrixMultiply(L, transpose(L, n), n);
	printf("L*L^T = \n");
	print_matrix(LLT, n);

	return 0;
}

