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
            if(DEBUG)printf("[SPEC_3_WFS3REQ]\t%d\tWysyla MREQ3!\n",this->process_id);
            for(int i = 0; i<process_count; i++){
                if(process_id == i) continue;
                MPI_Send( &message, 1, MPI_INT, i, MREQ3, MPI_COMM_WORLD);
            }
            int ack_count = 0;
            int message_tak[2];
            while(!is_S3REQ){
                if(ack_count >= this->data.expert_count - 1){
                    bool is_free = false;
                    for(int i = 0; i<this->process_count; i++){
                        if(this->process_list[i]>0){
                            is_free = true;
                            this->process_list[i] -= 1;
                            this->data.team_ids[1] = i;
                            this->data.team_ids[2] = this->process_id;
                            if(DEBUG)printf("[SPEC_3_WFS3REQ]\t%d\tWysyla MTAK3 zabiera proces %d!\n",this->process_id,i);
                            this->data.lamport_clock_value+=1;
                            message_tak[0] = this->data.lamport_clock_value;
                            message_tak[1] = i;
                            for(int i = 0; i<process_count; i++){
                                if(process_id == i) continue;
                                MPI_Send(&message_tak, 2, MPI_INT, i, MTAK3 ,MPI_COMM_WORLD);
                            }
                            break;
                        }
                    }
                        if(is_free)break;
                        if(DEBUG)printf("[SPEC_3_WFS3REQ]\t%d\tBrak specjalisty 2!\n",this->process_id);
                }
                MPI_Status status;
                MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

                switch(status.MPI_TAG){
                    case MREQ3 :
                        if(DEBUG)printf("[SPEC_3_WFS3REQ]\t%d\tOdebral MREQ3!\n",this->process_id);
                        if (ack_count < this->data.expert_count -1)
                        {
                            if(this->data.lamport_clock_value < message_buffor[0] || 
                            (this->data.lamport_clock_value == message_buffor[0] && this->process_id < status.MPI_SOURCE)){
                                this->data.lamport_clock_value+=2;
                                message = this->data.lamport_clock_value;
                                if(DEBUG)printf("[SPEC_3_WFS3REQ]\t%d\tWysyla MACK3 do %d!\n",this->process_id, status.MPI_SOURCE);
                                MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK3, MPI_COMM_WORLD);
                            }else{
                                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, message_buffor[0])+1;
                            }
                        }else{
                            this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, message_buffor[0])+1;
                        }
                        break;
                    case MACK3:
                        if(DEBUG)printf("[SPEC_3_WFS3REQ]\t%d %d\tOdebral MACK3!\n",this->process_id,ack_count+1);
                        ack_count+=1;
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, message_buffor[0])+1;
                        break;
                    case S3REQ :
                        if(DEBUG)printf("[SPEC_3_WFS3REQ]\t%d\tOdebral S3REQ!\n",this->process_id);
                        this->process_list[status.MPI_SOURCE]+=1;
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, message_buffor[0])+1;
                        break;
                    case MTAK3 :
                        if(DEBUG)printf("[SPEC_3_WFS3REQ]\t%d %d\tOdebral MTAK3!\n",this->process_id,ack_count+1);
                        ack_count+=1;
                        this->process_list[message_buffor[1]]-=1;
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, message_buffor[0])+1;
                        break;
                }

            }
        }
        void report_team_ready(){
            int team_mess[2];
            team_mess[0] = ++this->data.lamport_clock_value;
            team_mess[1] = this->process_id;
            if(DEBUG)printf("[SPEC_3_RTR]\t%d\tWysyla TREADY do %d!\n",this->process_id, this->data.team_ids[1]);
            MPI_Send(&team_mess, 2, MPI_INT, this->data.team_ids[1], TREADY, MPI_COMM_WORLD);
        }
        
        void wait_for_FTREADY(){
            bool is_FTREADY = false;
            int message_buffor[4];
            int message = this->data.lamport_clock_value;
            while(!is_FTREADY){
                MPI_Status status;
                MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

                switch(status.MPI_TAG){
                    case FTREADY:
                        if(DEBUG)printf("[SPEC_3_WFSTREDY]\t%d\tOdebral FTREADY (%d %d %d)!\n",this->process_id, message_buffor[1], message_buffor[2], message_buffor[3]);
                        memcpy(this->data.team_ids, &(message_buffor[1]), sizeof(int)*3);
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, message_buffor[0])+1;
                        is_FTREADY = true;
                        break;
                    case MREQ3 :
                        if(DEBUG)printf("[SPEC_3_WFSTREDY]\t%d\tWysyla MACK3 do %d!\n",this->process_id, status.MPI_SOURCE);
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, message_buffor[0])+2;
                        MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK3, MPI_COMM_WORLD);
                        break;
                    case S3REQ :
                        if(DEBUG)printf("[SPEC_3_WFSTREDY]\t%d\tOdebral S3REQ!\n",this->process_id);
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, message_buffor[0])+1; 
                        this->process_list[status.MPI_SOURCE]+=1;
                        break;
                    case MTAK3 :
                        if(DEBUG)printf("[SPEC_3_WFSTREDY]\t%d\tOdebral MTAK3!\n",this->process_id);
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, message_buffor[0])+1;
                        this->process_list[message_buffor[1]]-=1;
                        break;
                }

            }
        }

        void prepare_for_ressurection() {
            this->data.lamport_clock_value++;
            int message = this->data.lamport_clock_value;

            for(int i = 0; i < 2; i++){
                if(DEBUG)printf("[SPEC_3_PFR]\t%d\tWysyla RREADY do %d!\n",this->process_id, this->data.team_ids[i]);
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
                        if(DEBUG)printf("[SPEC_3_RESSURECT]\t%d\tOtrzymal RREADY od %d!\n",this->process_id,status.MPI_SOURCE);
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, mess_buf[0])+1;
                        rready_count++;
                        if(rready_count == 2){
                            if(DEBUG)printf("[SPEC_3_RESSURECT]\t%d\tZaczyna wskrzeszanie!\n",this->process_id);
                            sleep(rand()%1+1);
                            if(DEBUG)printf("[SPEC_3_RESSURECT]\t%d\tKonczy wskrzeszanie!\n",this->process_id);
                            message = ++this->data.lamport_clock_value;
                            for(int i = 0; i<process_count; i++){
                                if(process_id == i) continue;
                                MPI_Send( &message, 1, MPI_INT, i, S3IFREQ, MPI_COMM_WORLD);
                            }
                            team_ready = true;
                        }
                        break;
                    case MREQ3 :
                        if(DEBUG)printf("[SPEC_3_RESSURECT]\t%d\tOdebral MREQ3!\n",this->process_id);                   
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, mess_buf[0])+2;
                        message = this->data.lamport_clock_value;
                        MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK3, MPI_COMM_WORLD);
                        break;
                    case S3REQ :
                        if(DEBUG)printf("[SPEC_3_RESSURECT]\t%d\tOdebral S3REQ!\n",this->process_id);
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, mess_buf[0])+1;
                        this->process_list[status.MPI_SOURCE]+=1;
                        break;
                    case MTAK3 :
                        if(DEBUG)printf("[SPEC_3_RESSURECT]\t%d\tOdebral MTAK3!\n",this->process_id);
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value, mess_buf[0])+1;
                        this->process_list[mess_buf[1]]-=1;
                        break;
                }
            }
        }

        void lifetime(){
            wait_for_S3REQ();
            report_team_ready();
            wait_for_FTREADY();
            prepare_for_ressurection();
        }


};
