#include "constants.h"
class Employer: public thread{
    public:
    Employer(){
        srand(time(NULL));
    }

    void broadcast_mission(){
        if(DEBUG)printf("Wyslano informacje o misji z %d procesu do wszystkich procesow!\n",process_id);
        int message = MISSION;
        for(int i = 0; i<process_count; i++){
            if(process_id == i) continue;
            MPI_Send(&message, 1, MPI_INT, i, MISSION ,MPI_COMM_WORLD);
        }
    }

    void process_lifetime(){
        bool is_alive = true;
        while(is_alive){
            sleep(rand()%5+1);
            broadcast_mission();
        }
    }
};