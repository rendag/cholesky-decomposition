#include "cholMPI.h"

void cholMPI(double ** L, int n, int argc, char ** argv){
	// Warning: cholMPI() acts directly on the given matrix! 
	
	int npes, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	double start, end;	
	MPI_Barrier(MPI_COMM_WORLD); /* Timing */
	if (rank == 0) {
		start = MPI_Wtime();
		
		/*
		// Test
		printf("A = \n");
		print(L, n);
		*/
	}
	
	// For each column
	int i, j, k;
	for (j = 0; j < n; j++) {
	
		/*
		 * Step 0:
		 * Replace the entries above the diagonal with zeroes
		 */
		if (rank == 0) {
			for (i = 0; i < j; i++) {
				L[i][j] = 0.0;
			}
		}
		
		/*
		 * Step 1:
		 * Update the diagonal element
		 */
		 
		if (j%npes == rank) {

			for (k = 0; k < j; k++) {
				L[j][j] = L[j][j] - L[j][k] * L[j][k];
			}

			L[j][j] = sqrt(L[j][j]);
		}

		// Broadcast row with new values to other processes
		MPI_Bcast(L[j], n, MPI_DOUBLE, j%npes, MPI_COMM_WORLD);

		/*
		 * Step 2:
		 * Update the elements below the diagonal element
		 */

		// Divide the rest of the work
		for (i = j+1; i < n; i++) {
			if (i%npes == rank) {
				for (k = 0; k < j; k++) {
					L[i][j] = L[i][j] - L[i][k] * L[j][k];
				}
			
				L[i][j] = L[i][j] / L[j][j];
			}
		}
	}
				
	MPI_Barrier(MPI_COMM_WORLD); /* Timing */
	if (rank == 0){	
		end = MPI_Wtime();
		printf("Runtime = %lf\n", end-start);
		
		/*
        // Test
        double ** LLT = matrixMultiply(L, transpose(L, n), n);
        printf("L*L^T = \n");
        print(LLT, n);
        */
	}

	MPI_Finalize();
}
