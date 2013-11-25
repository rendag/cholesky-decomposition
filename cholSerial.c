#include "cholSerial.h"

double ** cholSerial(double ** A, int n){
	// Copy matrix A and take only lower triangular part
	double ** L = initMatrix(n);
	transCopy(A, L, n);
	
	int i,j,k;
	for (i = 0; i < n; i++){
		for (j = 0; j < n; j++){
			if (j < i){
				// Inner sum
				for (k = 0; k < j; k++){
					L[i][j] = L[i][j] - L[i][k] * L[j][k];
				}

				L[i][j] = L[i][j]/ L[j][j];
				
			} else if (j == i){
				// On diagonals, apply same inner sum as before
				
				for (k = 0; k< i; k++){
					L[i][i] = L[i][i] - L[i][k]*L[i][k];
				}
				
				L[i][i] = sqrt(L[i][i]);
			} else {
				// Upper triangular part of W - Clear it
				L[i][j] = 0;
			}
		}
	}
	
	return L;
}
