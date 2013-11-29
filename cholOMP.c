#include "matrix.h"
#include <omp.h>
        
        
double ** cholOMP(double ** L, int n) {
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
