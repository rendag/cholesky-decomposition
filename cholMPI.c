#include "cholMPI.h"

void cholMPI(double ** L, int n, int argc, char ** argv){
	// Warning: cholMPI() acts directly on the given matrix! 
	double start, end ;
	int npes, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	

	MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
    start = MPI_Wtime();

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
		
		/*
<<<<<<< HEAD
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
        end = MPI_Wtime();
	}

	MPI_Finalize();
	
		if (rank == 0) {
			printf("Runtime = %d\n", end-start);
		}
}
