#include "cholSerial.h"
#include "cholMPI.h"

int main(int argc, char **argv)
{
	// generate seed
	srand(time(NULL));

	// set matrix size
	int n = 10;

	// Generate random SPD matrix
	double** A = initialize(0, 10, n);
	
	// Apply Cholesky
	double** L = initialize(0, 10, n);
	copyMatrix(A, L, n);
	
	cholMPI(L, n, argc, argv); // Warning: cholMPI() acts directly on the given matrix

	return 0;
}

