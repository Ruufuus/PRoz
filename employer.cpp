#include "constants.h"
class Employer {
    private:
        int process_id; 

    Employer(){
        process_id = MPI_COMM_RANK;
    }

    void broadcast_mission(){
        if(DEBUG)printf("Wyslano informacje o misji z %d procesu do wszystkich procesow!\n",MPI_COMM_RANK);
        MPI_Bcast(null, 0, MPI_INT, MPI_COMM_RANK, MISSION);
    }
};