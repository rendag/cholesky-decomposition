#include <mpi.h>
#include "matrix.h"

typedef int bool;
#define true 1
#define false 0

void cholMPI(double ** L, int n, int argc, char ** argv);
