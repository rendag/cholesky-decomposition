
#include "cholMPI.h"

int main(int argc, char **argv)
{
	// generate seed
	srand(time(NULL));

	// set matrix size
	int n = 10;

	// Generate random SPD matrix
	double** A = initialize(0, 10, n);
	printf("A = \n");
	print(A, n);
	
	// Apply Cholesky
	double** L = initialize(0, 10, n);
	copyMatrix(A, L, n);
	cholMPI(L, n, argc, argv); // Warning: cholMPI() acts directly on the given matrix
	printf("L = \n");
	print(L, n);
	
	// Test
	double ** LLT = matrixMultiply(L, transpose(L, n), n);
	printf("L*L^T = \n");
	print(LLT, n);

	return 0;
}

