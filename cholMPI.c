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
		 * Fill the entries above the diagonal with zeroes
		 */
		 
		 for (i = 0; i < j; i++) {
		 	L[i][j] = 0.0;
		 }
		 
	
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
			
			// There's no extra work to be done
			extraWork = 0;
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
        end = MPI_Wtime();
	}
	
	MPI_Finalize();
	
		if (rank == 0) {
			printf("Runtime = %d\n", end-start);
		}
}
