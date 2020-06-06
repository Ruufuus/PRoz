#include <mpi.h>
#include <iostream>
class Thread{
    private:
        int process_id; 
        int process_count;
    public:
    Thread(){
        MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
        MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    }
};