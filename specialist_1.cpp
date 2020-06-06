#include "constants.h"
class Specialist_1{
    private:
        specialist data;
        int process_id; 
    public:
        Specialist_1(specialist data){
            MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
            this->data = data;
        }
        void wait_for_mission(){
            bool is_mission = false;
            int message;
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