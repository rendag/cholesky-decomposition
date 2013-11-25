chol: chol.c cholMPI.c matrix.c
	mpicc -Wall -o "chol" "chol.c" "cholMPI.c" "matrix.c" -lm -ggdb
