chol: chol.c matrix.c
	mpicc -Wall -o "chol" "chol.c" "matrix.c" -lm -ggdb
