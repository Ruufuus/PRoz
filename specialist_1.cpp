#include "constants.h"
class Specialist_1: public Thread{
    private:
        specialist data;
    public:
        Specialist_1(specialist data){
            this->data = data;
        }
        void wait_for_mission(){
            bool is_mission = false;
            this->data.lamport_clock_value+=1;
            int message = this->data.lamport_clock_value;
            for(int i = 0; i<process_count; i++){
                if(process_id == i) continue;
                MPI_Send(&message, 1, MPI_INT, i, MREQ1 ,MPI_COMM_WORLD);
            }
            int ack_count = 0;
            while(!is_mission){
                if(ack_count == this->data.expert_count-1 && this->data.mission_unassigned>0){
                    this->data.mission_unassigned-=1;
                    this->data.lamport_clock_value+=2;
                    if(DEBUG)printf("Proces %d bierze misje!\n",this->process_id);
                    for(int i = 0; i<process_count; i++){
                        if(process_id == i) continue;
                        MPI_Send(&message, 1, MPI_INT, i, MACK1 ,MPI_COMM_WORLD);
                        MPI_Send(&message, 1, MPI_INT, i, MTAK1 ,MPI_COMM_WORLD);
                    }
                    break;
                }
                MPI_Status status;
                MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                if(status.MPI_TAG == MISSION){
                    this->data.mission_unassigned+=1;
                    if(DEBUG)printf("Proces %d otrzymal wiadomosc o nowej misji od procesu %d!\n",process_id,status.MPI_SOURCE);
                }
                else if(status.MPI_TAG == MTAK1){
                    this->data.mission_unassigned-=1;
                    if(DEBUG)printf("Proces %d otrzymal wiadomosc o zabraniu misji od procesu %d!\n",process_id,status.MPI_SOURCE);
                }
                else if(status.MPI_TAG == MREQ1){
                    if(DEBUG)printf("Proces %d otrzymal wiadomosc o checi zabrania misji od procesu %d z wartoscia zegara lamporta rowna %d!\n",process_id,status.MPI_SOURCE, message);
                    if((this->data.lamport_clock_value==message && this->process_id<status.MPI_SOURCE) || (this->data.lamport_clock_value<message)){
                        if(DEBUG)printf("Proces %d wysyla wiadomosc ack do id:%d!\n",process_id,status.MPI_SOURCE);
                        MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, MACK1 ,MPI_COMM_WORLD);
                        this->data.lamport_clock_value+=1;
                    } 
                }else if(status.MPI_TAG == MACK1){
                    ack_count+=1;
                }
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message)+1;
                if(DEBUG)printf("Wartosc zegara lamporta %d procesu %d!\n",this->data.lamport_clock_value,this->process_id);
            }
        }

    
};