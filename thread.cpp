#include <mpi.h>
#include <iostream>
class Thread{
    public:
        int process_id; 
        int process_count;
    Thread(){
        MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
        MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    }
};