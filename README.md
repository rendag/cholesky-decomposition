# Serial and OpenMP tests
tests.c contains tests for the serial and OpenMP implementation                                                                      
include tests.c in the make file                                                                                             
make                                                                                                                  
./chol [matrix size] [number of threads]

# MPI tests
testMPI.c contains tests for MPI implementation                                                                             
include testMPI.c in the make file
make                                                                                                                                   
mpirun -np [number of processes] ./chol [matrix size]
