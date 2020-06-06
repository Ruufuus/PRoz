#include "constants.h"
class Specialist_1: public thread{
    private:
        specialist data;
    public:
        Specialist_1(specialist data){
            this->data = data;
        }
        void wait_for_mission(){
            bool is_mission = false;
            int message = MACK1;
            for(int i = 0; i<process_count; i++){
                if(process_id == i) continue;
                MPI_Send(&message, 1, MPI_INT, i, MISSION ,MPI_COMM_WORLD);
            }
            while(!is_mission){
                MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
                if(DEBUG)printf("Proces %d otrzymal wiadomosc o tresci %d!\n",process_id,message);
                if(message == MISSION){
                    this->data.mission_unassigned+=1;
                }
                else if(message == MTAK1){
                    this->data.mission_unassigned-=1;
                }
            }
        }

    
};