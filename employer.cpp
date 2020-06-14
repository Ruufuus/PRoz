#include "constants.h"
class Employer: public Thread{

    public:
    Employer(){
        srand(time(NULL));
    }

    void broadcast_mission(){
        if(DEBUG)printf("[EMPLOYER_BM]\t%d\tWyslano MISSION!\n",process_id);
        int msg = 0;
        for(int i = 0; i<process_count; i++){
            if(process_id == i) continue;
            MPI_Send(&msg, 1, MPI_INT, i, MISSION ,MPI_COMM_WORLD);
        }
    }

    void process_lifetime(){
        bool is_alive = true;
        while(is_alive){
            sleep(rand()%1+1);
            broadcast_mission();
        }
    }
};
