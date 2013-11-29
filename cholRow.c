#include "matrix.h"
#include <omp.h>

//double ** chol_omp(double ** L, int n);
	
	
double ** chol_omp(double ** L, int n) {
	// Warning: acts directly on given matrix!
	
	int i, j, k;
	omp_lock_t writelock;
	omp_init_lock(&writelock);
	
	for (j = 0; j < n; j++) {  
		
		for (i = 0; i < j; i++){			
			L[i][j] = 0;
		}
				
		#pragma omp parallel for shared(L) private(k)
		for (k = 0; k < i; k++) {
			omp_set_lock(&writelock);
			L[j][j] = L[j][j] - L[j][k] * L[j][k]; //Critical section.
			omp_unset_lock(&writelock);
		}			
		
		#pragma omp single	
		L[i][i] = sqrt(L[j][j]);	
		
		#pragma omp parallel for shared(L) private(i, k)
		for (i = j+1; i < n; i++) {
			for (k = 0; k < j; k++) {
				L[i][j] = L[i][j] - L[i][k] * L[j][k];
			}
			L[i][j] = L[i][j] / L[j][j];
		}		
	
		omp_destroy_lock(&writelock);
	}
	
	return L;
}

int main(int argc, char **argv)
{
	// generate seed
	srand(time(NULL));
	
	if(argc < 3){
		printf("Enter 2 args. Matrix size, number threads.\n");
		return -1;
	}
			
	int n = atoi(argv[1]); // set matrix size
	omp_set_num_threads(atoi(argv[2]));
	
	// Generate random SPD matrix
	double** A = initialize(0, 10, n);
	printf("Initial matrix\n");
	print(A, n);
	
	struct timespec begin={0,0}, end={0,0};
    clock_gettime(CLOCK_MONOTONIC, &begin);
	
	// Apply Cholesky
	chol_omp(A, n);
	
	clock_gettime(CLOCK_MONOTONIC, &end); //Get the current time.
    printf("The computation took %.5f seconds to complete. \n",
           ((double)end.tv_sec + 1.0e-9 * end.tv_nsec) - 
           ((double)begin.tv_sec + 1.0e-9 * begin.tv_nsec));
	
	printf("Chol matrix\n");
	print(A, n);

    // Test
    double ** LLT = matrixMultiply(A, transpose(A, n), n);
    printf("L*L^T = \n");
    print(LLT, n);

	return 0;
}
