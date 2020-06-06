#include "constants.h"
class Employer {
    private:
        int process_id; 

    public:
    Employer(){
        MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
        srand(time(NULL));
    }

    void broadcast_mission(){
        if(DEBUG)printf("Wyslano informacje o misji z %d procesu do wszystkich procesow!\n",process_id);
        int message = MISSION;
        MPI_Bcast(&message, 1, MPI_INT, process_id, MPI_COMM_WORLD);
    }

    void process_lifetime(){
        bool is_alive = true;
        while(is_alive){
            sleep((rand()%5+1)*1000000);
            broadcast_mission();
        }
    }
};