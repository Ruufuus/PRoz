#include "constants.h"
class Specialist_2: public Thread{
    private:
        specialist data;
    public:
        Specialist_2(specialist data){
            this->data = data;
        }

        void wait_for_S2REQ(){
            bool is_mission = false;
            this->data.lamport_clock_value+=1;
            int message = this->data.lamport_clock_value;
            int message_buffor[4];
            int request_priority = this->data.lamport_clock_value;
            if(DEBUG)printf("%d [SPEC_2_WFS2REQ]\t%d\tWysyla MREQ2!\n", this->data.lamport_clock_value,this->process_id);
            for(int i = 0; i<process_count; i++){
                if(process_id == i) continue;
                MPI_Send(&message, 1, MPI_INT, i, MREQ2 ,MPI_COMM_WORLD);
            }
            int ack_count = 0;
            int message_tak[3];
            while(!is_mission){
                if(ack_count >= this->data.expert_count-1){
                    bool is_free = false;
                    for(int i = 0; i<this->process_count; i++){
                        if(this->process_list[i]>0){
                            is_free = true;
                            this->process_list[i]-=1;
                            this->data.team_ids[0] = i;
                            this->data.lamport_clock_value+=1;
                            message_tak[0] = this->data.lamport_clock_value;
                            message_tak[1] = i;
                            if(DEBUG)printf("%d [SPEC_2_WFS2REQ]\t%d\tWysyla MTAK2 zabiera proces %d!\n", this->data.lamport_clock_value,this->process_id,i);
                            for(int i = 0; i<process_count; i++){
                                if(process_id == i) continue;
                                MPI_Send(&message_tak, 2, MPI_INT, i, MTAK2 ,MPI_COMM_WORLD);
                            }
                            break;
                        }
                    }
                    if(is_free)break;
                    //if(DEBUG)printf("[SPEC_2_WFS2REQ]\t%d\tBrak specjalistow nr1 szukajacych!\n",this->process_id);
                    
                }
                MPI_Status status;
                MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                if(status.MPI_TAG == S2REQ){
                    this->process_list[status.MPI_SOURCE]+=1;
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    if(DEBUG)printf("%d [SPEC_2_WFS2REQ]\t%d\tOtrzymal S2REQ od %d!\n", this->data.lamport_clock_value,process_id,status.MPI_SOURCE);
                }
                else if(status.MPI_TAG == MTAK2){
                    this->process_list[message_buffor[1]]-=1;
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    ack_count += 1;
                    if(DEBUG)printf("%d [SPEC_2_WFS2REQ]\t%d %d\totrzymal MTAK2 od %d!\n", this->data.lamport_clock_value,process_id,ack_count,status.MPI_SOURCE);
                }else if(status.MPI_TAG == SKREQ){
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                    message = this->data.lamport_clock_value;
                    MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, SKACK ,MPI_COMM_WORLD);
                    if(DEBUG)printf("%d [SPEC_2_WFS2REQ]\t%d\totrzymal SKREQ od %d!\n", this->data.lamport_clock_value,process_id,status.MPI_SOURCE);
                }
                else if(status.MPI_TAG == MREQ2){
                    if(DEBUG)printf("%d [SPEC_2_WFS2REQ]\t%d\tLAMP: %d otrzymal MREQ2 od %d LAMP: %d!\n", this->data.lamport_clock_value,process_id,this->data.lamport_clock_value,status.MPI_SOURCE, message_buffor[0]);
                        if(ack_count < this->data.expert_count -1){
                        if((request_priority==message_buffor[0] && this->process_id<status.MPI_SOURCE) 
                        || (request_priority>message_buffor[0])){
                            this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                            message = this->data.lamport_clock_value;
                            if(DEBUG)printf("%d [SPEC_2_WFS2REQ]\t%d\twysyla MACK2 do %d!\n", this->data.lamport_clock_value,process_id,status.MPI_SOURCE);
                            MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK2 ,MPI_COMM_WORLD);
                        }else{
                            this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                        }
                    }else{
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    }
                }else if(status.MPI_TAG == MACK2){
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    ack_count+=1;
                    if(DEBUG)printf("%d [SPEC_2_WFS2REQ]\t%d %d\totrzymal MACK2 od %d LAMP: %d!\n", this->data.lamport_clock_value,process_id,ack_count,status.MPI_SOURCE, message_buffor[0]);
                }
            }
        }

        void wait_for_specialist_3(){
            this->data.lamport_clock_value+=1;
            int message[3] = {this->data.lamport_clock_value, this->data.team_ids[0], this->process_id};
            if(DEBUG)printf("%d [SPEC_2_WFS3]\t%d\tWysyla S3REQ!\n", this->data.lamport_clock_value,this->process_id);
            for(int i = 0; i<process_count; i++){
                if(process_id == i) continue;
                MPI_Send(&message, 3, MPI_INT, i, S3REQ ,MPI_COMM_WORLD);
            }
        }

        void wait_for_team(){
        bool is_team_ready = true;
        MPI_Status status;
        int message;
        int message_buffor[4];
        while(is_team_ready){
            MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(status.MPI_TAG == S3IFREQ){
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                int mes_tab[3] = {this->data.lamport_clock_value, this->data.team_ids[0], this->process_id};

                if(DEBUG)printf("%d [SPEC_2_WFT]\t%d\tWysyla S3REQ do %d!\n", this->data.lamport_clock_value,this->process_id,status.MPI_SOURCE);
                MPI_Send(&mes_tab, 3, MPI_INT, status.MPI_SOURCE, S3REQ ,MPI_COMM_WORLD);
            }
            else if(status.MPI_TAG == TREADY){
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                this->data.team_ids[2] = message_buffor[1];
                this->data.team_ids[1] = this->process_id;
                int message_to_send[4] = {this->data.lamport_clock_value,this->data.team_ids[0],this->data.team_ids[1],this->data.team_ids[2]};

                if(DEBUG)printf("%d [SPEC_2_WFT]\t%d\tOtrzymuje TREADY od %d (%d %d %d)!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE, this->data.team_ids[0], this->process_id, status.MPI_SOURCE);
                MPI_Send(&message_to_send, 4, MPI_INT, data.team_ids[0], TREADY, MPI_COMM_WORLD);
                if(DEBUG)printf("%d [SPEC_2_WFT]\t%d\tOtrzymuje FTREADY od %d!\n", this->data.lamport_clock_value,this->process_id, data.team_ids[2]);
                MPI_Send(&message_to_send, 4, MPI_INT, data.team_ids[2], FTREADY, MPI_COMM_WORLD);
                break;
            }else if(status.MPI_TAG == MREQ2){
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                message = this->data.lamport_clock_value;
                if(DEBUG)printf("%d [SPEC_2_WFT]\t%d\tWysyla MACK2 do %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
                MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK2 ,MPI_COMM_WORLD);
            }else if(status.MPI_TAG == SKREQ){
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                message = this->data.lamport_clock_value;
                if(DEBUG)printf("%d [SPEC_2_WFT]\t%d\tWysyla  SKACK do %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
                MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, TACK ,MPI_COMM_WORLD);
            }else if(status.MPI_TAG == S2REQ){
                this->process_list[status.MPI_SOURCE]+=1;
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                if(DEBUG)printf("%d [SPEC_2_WFT]\t%d\tOtrzymuje S2REQ od %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
            }else if(status.MPI_TAG == MTAK2){
                this->process_list[message_buffor[1]]-=1;
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                if(DEBUG)printf("%d [SPEC_2_WFT]\t%d\tOtrzymuje MTAK2 od %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
            }
        }
    
};

        int wait_for_skeleton(){
        bool is_skeleton = true;
        int skack_count = 0;
        int rready_count = 0;
        MPI_Status status;
        int message = ++this->data.lamport_clock_value;
        int message_buffor[4];
        int request_priority = this->data.lamport_clock_value;
        if(DEBUG)printf("%d [SPEC_2_WFS]\t%d\tWysyla SKREQ!\n", this->data.lamport_clock_value,this->process_id);
        for(int i = 0; i<process_count; i++){
            if(process_id == i) continue;
            MPI_Send(&message, 1, MPI_INT, i, SKREQ ,MPI_COMM_WORLD);
        }
        while(is_skeleton){
            if(skack_count >= this->data.expert_count - this->data.initial_skeleton_count){
                this->data.lamport_clock_value+=1;
                message = this->data.lamport_clock_value;
                if(DEBUG)printf("%d [SPEC_2_WFS]\t%d\tBierze szkielet!\n", this->data.lamport_clock_value,this->process_id);
                for(int i = 0; i<process_count; i++){
                    if(process_id == i) continue;
                    MPI_Send(&message, 1, MPI_INT, i, SKACK ,MPI_COMM_WORLD);
                }
                if(DEBUG)printf("%d [SPEC_2_WFS]]\t%d\tZaczyna brac szkielet!\n", this->data.lamport_clock_value,this->process_id);
                //sleep(rand()%5+1);
                if(DEBUG)printf("%d [SPEC_2_WFS]\t%d\tKonczy brac szkielet!\n", this->data.lamport_clock_value,this->process_id);
                break;
            }
            MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(status.MPI_TAG == SKACK){
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                skack_count+=1;
                if(DEBUG)printf("%d [SPEC_2_WFS]\t%d\tOtrzymuje SKACK od %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
            }if(status.MPI_TAG == RREADY){
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                rready_count+=1;
                if(DEBUG)printf("%d [SPEC_2_WFS]\t%d\tOtrzymuje RREADY od %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
            }
            else if(status.MPI_TAG == SKREQ){
                if(DEBUG)printf("%d [SPEC_2_WFS]\t%d\tLAMP: %d Otrzymuje SKERQ od %d LAMP: %d!\n", this->data.lamport_clock_value,this->process_id, this->data.lamport_clock_value, status.MPI_SOURCE, message_buffor[0]);
                if(skack_count < this->data.expert_count - this->data.initial_skeleton_count){
                    if((request_priority==message_buffor[0] && this->process_id<status.MPI_SOURCE) 
                    || (request_priority>message_buffor[0])){
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                        message = this->data.lamport_clock_value;
                        if(DEBUG)printf("%d [SPEC_2_WFS]\t%d\tWysyla SKACK do %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
                        MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, SKACK ,MPI_COMM_WORLD);
                    }else{
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    }
                }
            }else if(status.MPI_TAG == S2REQ){
                this->data.mission_unassigned+=1;
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                if(DEBUG)printf("%d [SPEC_2_WFS]\t%d\tOtrzymuje S2REQ od %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
            }else if(status.MPI_TAG == MTAK2){
                this->data.mission_unassigned-=1;
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                if(DEBUG)printf("%d [SPEC_2_WFS]\t%d\tOtrzymuje MTAK2 od %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
            }else if(status.MPI_TAG == MREQ2){
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                message = this->data.lamport_clock_value;
                if(DEBUG)printf("%d [SPEC_2_WFS]\t%d\tWysyla MACK2 do %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
                MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK2 ,MPI_COMM_WORLD);
            }
        }
        return rready_count;
    }

        void ressurection(int rready_count){
            int id;
            int message;
            int message_buffor[4];
            MPI_Status status;
            for(int i = 0; i < 3; i++){
                id = this->data.team_ids[i];
                if(id == this->process_id)continue;
                this->data.lamport_clock_value +=1;
                message = this->data.lamport_clock_value;
                if(DEBUG)printf("%d [SPEC_2_RESSURECT]\t%d\tWysyla RREADY do %d!\n", this->data.lamport_clock_value,this->process_id, id);
                MPI_Send(&message, 1, MPI_INT, id, RREADY ,MPI_COMM_WORLD);
            }
            bool is_team_ready = false;
            int team_ready_counter = rready_count;
            while(!is_team_ready){
                if(team_ready_counter == 2)
                {
                    if(DEBUG)printf("%d [SPEC_2_RESSURECT]\t%d\tZaczyna wskrzeszanie!\n", this->data.lamport_clock_value,this->process_id);
                    //sleep(rand()%1+1);
                    if(DEBUG)printf("%d [SPEC_2_RESSURECT]\t%d\tKonczy wskrzeszanie!\n", this->data.lamport_clock_value,this->process_id);
                    break;
                }
                MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                if(status.MPI_TAG == RREADY){
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    team_ready_counter+=1;
                    if(DEBUG)printf("%d [SPEC_2_RESSURECT]\t%d\tOtrzymuje RREADY od %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
                }
                else if(status.MPI_TAG == MREQ2){
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                    message = this->data.lamport_clock_value;
                    if(DEBUG)printf("%d [SPEC_2_RESSURECT]\t%d\tWysyla MACK2 do %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
                    MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK2 ,MPI_COMM_WORLD);
                }else if(status.MPI_TAG == SKREQ){
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                    message = this->data.lamport_clock_value;
                    if(DEBUG)printf("%d [SPEC_2_RESSURECT]\t%d\tWysyla SKACK do %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
                    
                    MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, SKACK ,MPI_COMM_WORLD);
                }else if(status.MPI_TAG == S2REQ){
                    this->data.mission_unassigned+=1;
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    if(DEBUG)printf("%d [SPEC_2_RESSURECT]\t%d\tOtzymuje S2REQ od %d!\n", this->data.lamport_clock_value,this->process_id, status.MPI_SOURCE);
                    
                }else if(status.MPI_TAG == MTAK2){
                    this->data.mission_unassigned-=1;
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;

                    if(DEBUG)printf("%d [SPEC_2_RESSURECT]\t%d\tOtrzymuje MTAK2 do %d!\n", this->data.lamport_clock_value, this->process_id, status.MPI_SOURCE);
                    
                }
            }
        }

        void lifetime(){
            this->wait_for_S2REQ();
            this->wait_for_specialist_3();
            this->wait_for_team();
            int rready__counter = this->wait_for_skeleton();
            this->ressurection(rready__counter);
        }
};