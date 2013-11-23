chol: chol.c matrix.c
	gcc -Wall -o "chol" "chol.c" "matrix.c" -lm -ggdb
