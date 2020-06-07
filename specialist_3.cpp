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
                if(ack_count >= this->data.expert_count - this->data.mission_unassigned){
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
                        if (this->data.lamport_clock_value < message_buffor[0] || 
                        (this->data.lamport_clock_value == message_buffor[0] && this->process_id < status.MPI_SOURCE)){
                            this->data.lamport_clock_value++;
                            message = this->data.lamport_clock_value;
                            MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK3, MPI_COMM_WORLD);
                        }
                        break;
                    case S3REQ :
                        this->data.mission_unassigned++;
                        this->data.team_ids[0] = message_buffor[1];
                        this->data.team_ids[1] = message_buffor[2];
                        is_S3REQ = true;
                        break;
                    case MTAK3 :
                        this->data.mission_unassigned--;
                        break;
                }

                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, message_buffor[0])+1;
            }
        }
        void report_team_ready(){
            int team_mess[4];
            team_mess[0] = this->data.lamport_clock_value + 1;
            memcpy(&(team_mess[1]),this->data.team_ids, sizeof(int)*3);

            for(int i = 0; i < 2; i++){
                MPI_Send(&team_mess, 4, MPI_INT, this->data.team_ids[i], TREADY, MPI_COMM_WORLD);
            }
        }

        void prepare_for_ressurection() {
            this->data.lamport_clock_value++;
            int message = this->data.lamport_clock_value;

            for(int i = 0; i < 2; i++){
                MPI_Send(&message, 1, MPI_INT, this->data.team_ids[i], RREADY, MPI_COMM_WORLD);
            }

            int mess_buf[4];
            int rready_count = 0;
            bool team_ready = false;
            MPI_Status status;
            while(!team_ready){
                MPI_Recv(&mess_buf, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

                switch(status.MPI_TAG){
                    case RREADY :
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, mess_buf[0])+1;
                        rready_count++;
                        if(rready_count == 2){
                            if(DEBUG)printf("[SPEC3_RESSURECT]\t%d\tZaczyna wskrzeszanie!\n",this->process_id);
                            sleep(5);
                            if(DEBUG)printf("[SPEC3_RESSURECT]\t%d\tKonczy wskrzeszanie!\n",this->process_id);
                            message = ++this->data.lamport_clock_value;
                            for(int i = 0; i<process_count; i++){
                                if(process_id == i) continue;
                                MPI_Send( &message, 1, MPI_INT, i, S3IFREQ, MPI_COMM_WORLD);
                            }
                            team_ready = true;
                        }
                        break;
                    case MREQ3 :
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, mess_buf[0])+1;
                        message = this->data.lamport_clock_value;
                        MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK3, MPI_COMM_WORLD);
                        break;
                    case S3REQ :
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, mess_buf[0])+1;
                        this->data.mission_unassigned++;
                        break;
                    case MTAK3 :
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, mess_buf[0])+1;
                        this->data.mission_unassigned--;
                        break;
                }
            }
        }

        void lifetime(){
            wait_for_S3REQ();
            report_team_ready();
            prepare_for_ressurection();
        }


};