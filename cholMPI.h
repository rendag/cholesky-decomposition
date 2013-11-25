#include <mpi.h>
#include "matrix.h"

typedef int bool;
#define true 1
#define false 0

double ** cholMPI(double ** A, int n, int argc, char ** argv);
