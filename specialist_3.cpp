#include "constants.h"

class Specialist_3: public Thread {
    private:
     specialist data;
    
    public:
        Specialist_3(specialist data){
            this->data = data;
        }

        void wait_for_S3REQ(){
            bool is_S3REQ = false;
            this->data.lamport_clock_value += 1;
            int message = this->data.lamport_clock_value;
            int message_buffor[4];
            
            for(int i = 0; i<process_count; i++){
                if(process_id == i) continue;
                MPI_Send( &message, 1, MPI_INT, i, MREQ3, MPI_COMM_WORLD);
            }

            int ack_count = 0;

            while(!is_S3REQ){
                if(ack_count == this->data.expert_count - this->data.mission_unassigned){
                    message = this->data.lamport_clock_value;
                    if(DEBUG)printf("[SPEC_3_WFS3REQ]\t%d\tWysyla MTAK3!\n",this->process_id);
                    for(int i = 0; i<process_count; i++){
                        if(process_id == i) continue;
                        MPI_Send(&message, 1, MPI_INT, i, MTAK3, MPI_COMM_WORLD);
                    }
                    break;
                }
                MPI_Status status;
                MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

                switch(status.MPI_TAG){
                    case MREQ3 :
                        if (this->data.lamport_clock_value < message_buffor[0]){
                            this->data.lamport_clock_value += 1;
                            message = this->data.lamport_clock_value;
                            MPI_Send(&message, 1, MPI_INT, i, MACK3, MPI_COMM_WORLD);
                        } else {
                            this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, message_buffor[0])+1;
                        }
                        break;
                    case S3REQ :
                        this->data.mission_unassigned += 1;
                        break;
                    case MTAK3 :
                        this->data.mission_unassigned -= 1;
                    default:
                        break;

                }
            }
        }

        void report_team_ready(){

        }
}