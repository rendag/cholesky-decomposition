chol: tests.c matrix.c
	gcc -Wall -o "chol" "chol.c" "tests.c" "matrix.c" -lm -ggdb
