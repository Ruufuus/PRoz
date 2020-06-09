#include "constants.h"
class Employer: public Thread{
    public:
    Employer(){
        srand(time(NULL));
    }

    void broadcast_mission(){
        if(DEBUG)printf("[EMPLOYER_BM]\t%d\tWyslano MISSION!\n",process_id);
        for(int i = 0; i<process_count; i++){
            if(process_id == i) continue;
            MPI_Send(NULL, 0, MPI_INT, i, MISSION ,MPI_COMM_WORLD);
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