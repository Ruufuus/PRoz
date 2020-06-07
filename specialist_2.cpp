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
            for(int i = 0; i<process_count; i++){
                if(process_id == i) continue;
                MPI_Send(&message, 1, MPI_INT, i, MREQ2 ,MPI_COMM_WORLD);
            }
            int ack_count = 0;
            while(!is_mission){
                if(ack_count == this->data.expert_count-this->data.mission_unassigned){
                    this->data.mission_unassigned-=1;
                    this->data.lamport_clock_value+=1;
                    message = this->data.lamport_clock_value;
                    if(DEBUG)printf("[SPEC_2_WFS2REQ]\t%d\tWysyla MTAK2!\n",this->process_id);
                    for(int i = 0; i<process_count; i++){
                        if(process_id == i) continue;
                        MPI_Send(&message, 1, MPI_INT, i, MTAK2 ,MPI_COMM_WORLD);
                    }
                    break;
                }
                MPI_Status status;
                MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                if(status.MPI_TAG == S2REQ){
                    this->data.mission_unassigned+=1;
                    this->data.team_ids[0] = message_buffor[1];
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    if(DEBUG)printf("[SPEC_2_WFS2REQ]\t%d\tOtrzymal S2REQ od %d!\n",process_id,status.MPI_SOURCE);
                }
                else if(status.MPI_TAG == MTAK2){
                    this->data.mission_unassigned-=1;
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    ack_count += 1;
                    if(DEBUG)printf("[SPEC_2_WFS2REQ]\t%d\totrzymal MTAK2 od %d!\n",process_id,status.MPI_SOURCE);
                }else if(status.MPI_TAG == SKREQ){
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                    message = this->data.lamport_clock_value;
                    MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, SKACK ,MPI_COMM_WORLD);
                    if(DEBUG)printf("[SPEC_2_WFS2REQ]\t%d\totrzymal SKREQ od %d!\n",process_id,status.MPI_SOURCE);
                }
                else if(status.MPI_TAG == MREQ2){
                    if(DEBUG)printf("[SPEC_2_WFS2REQ]\t%d\tLAMP: %d otrzymal MREQ2 od %d LAMP: %d!\n",process_id,this->data.lamport_clock_value,status.MPI_SOURCE, message_buffor[0]);
                    if((this->data.lamport_clock_value==message_buffor[0] && this->process_id<status.MPI_SOURCE) || (this->data.lamport_clock_value<message_buffor[0])){
                        if(DEBUG)printf("[SPEC_2_WFS2REQ]\t%d\twysyla MACK2 do %d!\n",process_id,status.MPI_SOURCE);
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                        message = this->data.lamport_clock_value;
                        MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK2 ,MPI_COMM_WORLD);
                    }else{
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    }
                }else if(status.MPI_TAG == MACK2){
                    if(DEBUG)printf("[SPEC_2_WFS2REQ]\t%d\totrzymal MACK2 od %d LAMP: %d!\n",process_id,status.MPI_SOURCE, message_buffor[0]);
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    ack_count+=1;
                }
            }
        }

        void wait_for_specialist_3(){
            this->data.lamport_clock_value+=1;
            int message[3] = {this->data.lamport_clock_value, this->data.team_ids[0], this->process_id};
            if(DEBUG)printf("[SPEC_2_WFS3]\t%d\tWysyla S3REQ!\n",this->process_id);
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
                if(DEBUG)printf("[SPEC_2_WFT]\t%d\tWysyla S3REQ do %d!\n",this->process_id,status.MPI_SOURCE);
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                int mes_tab[3] = {this->data.lamport_clock_value, this->data.team_ids[0], this->process_id};
                MPI_Send(&mes_tab, 3, MPI_INT, status.MPI_SOURCE, S3REQ ,MPI_COMM_WORLD);
            }
            else if(status.MPI_TAG == TREADY){
                if(DEBUG)printf("[SPEC_2_WFT]\t%d\tOtrzymuje TREADY od %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                memcpy(this->data.team_ids, &(message_buffor[1]), sizeof(int)*3);
                break;
            }else if(status.MPI_TAG == MREQ2){
                if(DEBUG)printf("[SPEC_2_WFT]\t%d\tWysyla MACK2 do %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                message = this->data.lamport_clock_value;
                MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK2 ,MPI_COMM_WORLD);
            }else if(status.MPI_TAG == SKREQ){
                if(DEBUG)printf("[SPEC_2_WFT]\t%d\tWysyla  SKACK do %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                message = this->data.lamport_clock_value;
                MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, TACK ,MPI_COMM_WORLD);
            }else if(status.MPI_TAG == S2REQ){
                if(DEBUG)printf("[SPEC_2_WFT]\t%d\tOtrzymuje S2REQ od %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.mission_unassigned+=1;
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
            }else if(status.MPI_TAG == MTAK2){
                if(DEBUG)printf("[SPEC_2_WFT]\t%d\tOtrzymuje MTAK2 od %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.mission_unassigned-=1;
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
            }
        }
    
};

        void wait_for_skeleton(){
        bool is_skeleton = true;
        int skack_count = 0;
        MPI_Status status;
        int message;
        int message_buffor[4];
        while(is_skeleton){
            if(skack_count == this->data.expert_count - this->data.initial_skeleton_count){
                this->data.lamport_clock_value+=1;
                message = this->data.lamport_clock_value;
                if(DEBUG)printf("[SPEC_2_WFS]\t%d\tBierze szkielet!\n",this->process_id);
                for(int i = 0; i<process_count; i++){
                    if(process_id == i) continue;
                    MPI_Send(&message, 1, MPI_INT, i, SKACK ,MPI_COMM_WORLD);
                }
                if(DEBUG)printf("[SPEC_2_WFS]]\t%d\tZaczyna brac szkielet!\n",this->process_id);
                sleep(3);
                if(DEBUG)printf("[SPEC_2_WFS]\t%d\tKonczy brac szkielet!\n",this->process_id);
                break;
            }
            MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(status.MPI_TAG == SKACK){
                if(DEBUG)printf("[SPEC_2_WFS]\t%d\tOtrzymuje SKACK od %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                skack_count+=1;
            }
            if(status.MPI_TAG == SKREQ){
                if(DEBUG)printf("[SPEC_2_WFS]\t%d\tLAMP: %d Otrzymuje SKERQ od %d LAMP: %d!\n",this->process_id, this->data.lamport_clock_value, status.MPI_SOURCE, message_buffor[0]);
                if((this->data.lamport_clock_value==message_buffor[0] && this->process_id<status.MPI_SOURCE) || (this->data.lamport_clock_value<message_buffor[0])){
                    if(DEBUG)printf("[SPEC_2_WFS]\t%d\tWysyla SKACK do %d!\n",this->process_id, status.MPI_SOURCE);
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                    message = this->data.lamport_clock_value;
                    MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, SKACK ,MPI_COMM_WORLD);
                }else{
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                }
            }else if(status.MPI_TAG == S2REQ){
                if(DEBUG)printf("[SPEC_2_WFS]\t%d\tOtrzymuje S2REQ od %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.mission_unassigned+=1;
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
            }else if(status.MPI_TAG == MTAK2){
                if(DEBUG)printf("[SPEC_2_WFS]\t%d\tOtrzymuje MTAK2 od %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.mission_unassigned-=1;
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
            }else if(status.MPI_TAG == MREQ2){
                if(DEBUG)printf("[SPEC_2_WFS]\t%d\tWysyla MACK2 do %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                message = this->data.lamport_clock_value;
                MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK2 ,MPI_COMM_WORLD);
            }
        }
    }

        void ressurection(){
            int id;
            int message;
            int message_buffor[4];
            MPI_Status status;
            for(int i = 0; i < 3; i++){
                id = this->data.team_ids[i];
                if(id == this->process_id)continue;
                this->data.lamport_clock_value +=1;
                message = this->data.lamport_clock_value;
                MPI_Send(&message, 1, MPI_INT, id, RREADY ,MPI_COMM_WORLD);
            }
            bool is_team_ready = false;
            int team_ready_counter = 0;
            while(!is_team_ready){
                MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                if(status.MPI_TAG == RREADY){
                    if(DEBUG)printf("[SPEC_2_RESSURECT]\t%d\tOtrzymuje RREADY od %d!\n",this->process_id, status.MPI_SOURCE);
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    team_ready_counter+=1;
                    if(team_ready_counter == 2)
                    {
                        if(DEBUG)printf("[SPEC_2_RESSURECT]\t%d\tZaczyna wskrzeszanie!\n",this->process_id);
                        sleep(5);
                        if(DEBUG)printf("[SPEC_2_RESSURECT]\t%d\tKonczy wskrzeszanie!\n",this->process_id);
                        break;
                        }
                }
                else if(status.MPI_TAG == MREQ2){
                    if(DEBUG)printf("[SPEC_2_RESSURECT]\t%d\tWysyla MACK2 do %d!\n",this->process_id, status.MPI_SOURCE);
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                    message = this->data.lamport_clock_value;
                    MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK2 ,MPI_COMM_WORLD);
                }else if(status.MPI_TAG == SKREQ){
                    if(DEBUG)printf("[SPEC_2_RESSURECT]\t%d\tWysyla SKACK do %d!\n",this->process_id, status.MPI_SOURCE);
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                    message = this->data.lamport_clock_value;
                    MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, SKACK ,MPI_COMM_WORLD);
                }else if(status.MPI_TAG == S2REQ){
                    if(DEBUG)printf("[SPEC_2_RESSURECT]\t%d\tOtzymuje S2REQ od %d!\n",this->process_id, status.MPI_SOURCE);
                    this->data.mission_unassigned+=1;
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                }else if(status.MPI_TAG == MTAK2){
                    if(DEBUG)printf("[SPEC_2_RESSURECT]\t%d\tOtrzymuje MTAK2 do %d!\n",this->process_id, status.MPI_SOURCE);
                    this->data.mission_unassigned-=1;
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                }
            }
        }

        void lifetime(){
            this->wait_for_S2REQ();
            this->wait_for_specialist_3();
            this->wait_for_team();
            this->wait_for_skeleton();
            this->ressurection();
        }
};