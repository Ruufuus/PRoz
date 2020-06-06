#include "constants.h"
class Employer {
    private:
        int process_id; 

    Employer(){
        process_id = MPI_COMM_RANK;
        srand(time(NULL))
    }

    void broadcast_mission(){
        if(DEBUG)printf("Wyslano informacje o misji z %d procesu do wszystkich procesow!\n",process_id);
        MPI_Bcast(NULL, 0, MPI_INT, process_id, MISSION);
    }

    void process_lifetime(){
        bool is_alive = true;
        while(is_alive){
            sleep(rand()%5+1);
            broadcast_mission();
        }
    }
};