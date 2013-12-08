chol: tests.c cholMPI.c matrix.c cholSerial.c cholOMP.c 
	mpicc -Wall -o "chol" "tests.c" "cholMPI.c" "matrix.c" "cholSerial.c" "cholOMP.c" -lm -ggdb -fopenmp
