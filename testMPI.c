#include "matrix.h"

int main(int argc, char **argv)
{
    // generate seed
    srand(time(NULL));
    if (argc != 2)

    {   
       printf("You did not feed me arguments, I will die now :( ... \n");
       printf("Usage: %s [matrix size] \n", argv[0]);
       return 1;
    }
    int matrixSize = atoi(argv[1]);
    
    // Generate random SPD matrix
    double** A = initialize(0, 10, matrixSize);
    /*printf("Chol matrix\n");
	print(A, matrixSize);*/
    double **L = initialize(0, 10, matrixSize);
    
    //Testing OpenMpi Program 
    
    copyMatrix(A,L,matrixSize);
    cholMPI(A,L, matrixSize, argc, argv); // Warning: cholMPI() acts directly on the given matrix L

}


