#include <mpi.h>
#include <iostream>
class Thread{
    public:
        int process_id; 
        int process_count;
        int * process_list;
    Thread(){
        MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
        MPI_Comm_size(MPI_COMM_WORLD, &process_count);
        process_list = new int [process_count];
        memset(process_list,0,sizeof(int)*process_count);
    }

    ~Thread(){
        free(process_list);
    }
};