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
            int message_buffor[4];
            for(int i = 0; i<process_count; i++){
                if(process_id == i) continue;
                MPI_Send(&message, 1, MPI_INT, i, MREQ1 ,MPI_COMM_WORLD);
            }
            int ack_count = 0;
            while(!is_mission){
                if(ack_count >= this->data.expert_count - this->data.mission_unassigned - 1){
                    this->data.mission_unassigned-=1;
                    this->data.lamport_clock_value+=1;
                    message = this->data.lamport_clock_value;
                    if(DEBUG)printf("[SPEC_1_WFM]\t%d\tWysyla MTAK1 i MACK1!\n",this->process_id);
                    for(int i = 0; i<process_count; i++){
                        if(process_id == i) continue;
                        MPI_Send(&message, 1, MPI_INT, i, MTAK1 ,MPI_COMM_WORLD);
                    }
                    break;
                }
                MPI_Status status;
                MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                if(status.MPI_TAG == MISSION){
                    this->data.mission_unassigned+=1;
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    if(DEBUG)printf("[SPEC_1_WFM]\t%d %d %d\tOtrzymal MISSION od %d!\n",process_id,ack_count,this->data.mission_unassigned,status.MPI_SOURCE);
                }
                else if(status.MPI_TAG == MTAK1){
                    this->data.mission_unassigned-=1;
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    ack_count += 1;
                    if(DEBUG)printf("[SPEC_1_WFM]\t%d %d %d\totrzymal MTAK1 od %d!\n",process_id,ack_count,this->data.mission_unassigned,status.MPI_SOURCE);
                }else if(status.MPI_TAG == TREQ){
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                    message = this->data.lamport_clock_value;
                    MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, TACK ,MPI_COMM_WORLD);
                    if(DEBUG)printf("[SPEC_1_WFM]\t%d\totrzymal TREQ od %d!\n",process_id,status.MPI_SOURCE);
                }
                else if(status.MPI_TAG == MREQ1){
                    if(DEBUG)printf("[SPEC_1_WFM]\t%d\tLAMP: %d otrzymal MREQ1 od %d LAMP: %d!\n",process_id,this->data.lamport_clock_value,status.MPI_SOURCE, message_buffor[0]);
                    if((this->data.lamport_clock_value==message_buffor[0] && this->process_id<status.MPI_SOURCE) 
                    || (this->data.lamport_clock_value<message_buffor[0])){
                        if(DEBUG)printf("[SPEC_1_WFM]\t%d\twysyla MACK1 do %d!\n",process_id,status.MPI_SOURCE);
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                        message = this->data.lamport_clock_value;
                        MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK1 ,MPI_COMM_WORLD);
                    }else{
                        this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    }
                }else if(status.MPI_TAG == MACK1){
                    if(DEBUG)printf("[SPEC_1_WFM]\t%d %d %d\totrzymal MACK1 od %d LAMP: %d!\n",process_id,ack_count+1,this->data.mission_unassigned,status.MPI_SOURCE, message_buffor[0]);
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    ack_count+=1;
                }
            }
        }

        void wait_for_specialist_2(){
            this->data.lamport_clock_value+=1;
            int message[2] = {this->data.lamport_clock_value, this->process_id};
            if(DEBUG)printf("[SPEC_1_WFS2]\t%d\tWysyla S2REQ!\n",this->process_id);
            for(int i = 0; i<process_count; i++){
                if(process_id == i) continue;
                MPI_Send(&message, 2, MPI_INT, i, S2REQ ,MPI_COMM_WORLD);
            }
        }

        void wait_for_team(){
        bool is_team_ready = true;
        MPI_Status status;
        int message;
        int message_buffor[4];
        while(is_team_ready){
            MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(status.MPI_TAG == S2IFREQ){
                if(DEBUG)printf("[SPEC_1_WFT]\t%d\tWysyla S2REQ do %d!\n",this->process_id,status.MPI_SOURCE);
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                int mes_tab[2] = {this->data.lamport_clock_value, this->process_id};
                MPI_Send(&mes_tab, 2, MPI_INT, status.MPI_SOURCE, S2REQ ,MPI_COMM_WORLD);
            }
            else if(status.MPI_TAG == TREADY){
                if(DEBUG)printf("[SPEC_1_WFT]\t%d\tOtrzymuje TREADY od %d (%d %d %d)!\n",this->process_id, status.MPI_SOURCE,message_buffor[1],message_buffor[2],message_buffor[3]);
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                memcpy(this->data.team_ids, &(message_buffor[1]), sizeof(int)*3);
                break;
            }else if(status.MPI_TAG == MREQ1){
                if(DEBUG)printf("[SPEC_1_WFT]\t%d\tWysyla MACK1 do %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                message = this->data.lamport_clock_value;
                MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK1 ,MPI_COMM_WORLD);
            }else if(status.MPI_TAG == TREQ){
                if(DEBUG)printf("[SPEC_1_WFT]\t%d\tWysyla  TACK do %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                message = this->data.lamport_clock_value;
                MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, TACK ,MPI_COMM_WORLD);
            }else if(status.MPI_TAG == MISSION){
                if(DEBUG)printf("[SPEC_1_WFT]\t%d\tOtrzymuje MISSION od %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.mission_unassigned+=1;
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
            }else if(status.MPI_TAG == MTAK1){
                if(DEBUG)printf("[SPEC_1_WFT]\t%d\tOtrzymuje MTAK1 od %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.mission_unassigned-=1;
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
            }
        }
    
};

        int wait_for_table(){
        bool is_table = true;
        int tack_count = 0;
        int rready_count = 0;
        MPI_Status status;
        int message;
        int message_buffor[4];
        while(is_table){
            if(tack_count >= this->data.expert_count - this->data.guild_table_count){
                this->data.lamport_clock_value+=1;
                message = this->data.lamport_clock_value;
                if(DEBUG)printf("[SPEC_1_WFTABLE]\t%d\tBierze stol!\n",this->process_id);
                for(int i = 0; i<process_count; i++){
                    if(process_id == i) continue;
                    MPI_Send(&message, 1, MPI_INT, i, TACK ,MPI_COMM_WORLD);
                }
                if(DEBUG)printf("[SPEC_1_WFTABLE]\t%d\tZaczyna pap. robote!\n",this->process_id);
                //sleep(rand()%1+1);
                if(DEBUG)printf("[SPEC_1_WFTABLE]\t%d\tKonczy pap. robote!\n",this->process_id);
                break;
            }
            MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if(status.MPI_TAG == TACK){
                if(DEBUG)printf("[SPEC_1_WFTABLE]\t%d\tOtrzymuje TACK od %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                tack_count+=1;
            }else if(status.MPI_TAG == RREADY){
                if(DEBUG)printf("[SPEC_1_WFTABLE]\t%d\tOtrzymuje RREADY od %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                rready_count+=1;
            }else 
            if(status.MPI_TAG == TREQ){
                if(DEBUG)printf("[SPEC_1_WFT]\t%d\tLAMP: %d Otrzymuje TREQ od %d LAMP: %d!\n",this->process_id, this->data.lamport_clock_value, status.MPI_SOURCE, message_buffor[0]);
                if((this->data.lamport_clock_value==message_buffor[0] && this->process_id<status.MPI_SOURCE) || (this->data.lamport_clock_value<message_buffor[0])){
                    if(DEBUG)printf("[SPEC_1_WFT]\t%d\tWysyla TACK do %d!\n",this->process_id, status.MPI_SOURCE);
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                    message = this->data.lamport_clock_value;
                    MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, TACK ,MPI_COMM_WORLD);
                }else{
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                }
            }else if(status.MPI_TAG == MISSION){
                if(DEBUG)printf("[SPEC_1_WFTABLE]\t%d\tOtrzymuje MISSION od %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.mission_unassigned+=1;
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
            }else if(status.MPI_TAG == MTAK1){
                if(DEBUG)printf("[SPEC_1_WFTABLE]\t%d\tOtrzymuje MTAK1 od %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.mission_unassigned-=1;
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
            }else if(status.MPI_TAG == MREQ1){
                if(DEBUG)printf("[SPEC_1_WFTABLE]\t%d\tWysyla MACK1 do %d!\n",this->process_id, status.MPI_SOURCE);
                this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                message = this->data.lamport_clock_value;
                MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK1 ,MPI_COMM_WORLD);
            }
        }
        return rready_count;
    }

        void ressurection(int rready_counter){
            int id;
            int message;
            int message_buffor[4];
            MPI_Status status;
            for(int i = 1; i < 3; i++){
                id = this->data.team_ids[i];
                if(DEBUG)printf("[SPEC_1_RESSURECT]\t%d\tWysyla RREADY do %d!\n",this->process_id, id);
                this->data.lamport_clock_value +=1;
                message = this->data.lamport_clock_value;
                MPI_Send(&message, 1, MPI_INT, id, RREADY ,MPI_COMM_WORLD);
            }
            bool is_team_ready = false;
            int team_ready_counter = rready_counter;
            while(!is_team_ready){
                if(team_ready_counter == 2)
                {
                    if(DEBUG)printf("[SPEC_1_RESSURECT]\t%d\tZaczyna wskrzeszanie!\n",this->process_id);
                    //sleep(rand()%1+1);
                    if(DEBUG)printf("[SPEC_1_RESSURECT]\t%d\tKonczy wskrzeszanie!\n",this->process_id);
                    break;
                }
                MPI_Recv(&message_buffor, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                if(status.MPI_TAG == RREADY){
                    if(DEBUG)printf("[SPEC_1_RESSURECT]\t%d\tOtrzymuje RREADY od %d!\n",this->process_id, status.MPI_SOURCE);
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                    team_ready_counter+=1;
                }
                else if(status.MPI_TAG == MREQ1){
                    if(DEBUG)printf("[SPEC_1_RESSURECT]\t%d\tWysyla MACK1 do %d!\n",this->process_id, status.MPI_SOURCE);
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                    message = this->data.lamport_clock_value;
                    MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, MACK1 ,MPI_COMM_WORLD);
                }else if(status.MPI_TAG == TREQ){
                    if(DEBUG)printf("[SPEC_1_RESSURECT]\t%d\tWysyla TACK do %d!\n",this->process_id, status.MPI_SOURCE);
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+2;
                    message = this->data.lamport_clock_value;
                    MPI_Send(&message, 1, MPI_INT, status.MPI_SOURCE, TACK ,MPI_COMM_WORLD);
                }else if(status.MPI_TAG == MISSION){
                    if(DEBUG)printf("[SPEC_1_RESSURECT]\t%d\tOtzymuje MISSION od %d!\n",this->process_id, status.MPI_SOURCE);
                    this->data.mission_unassigned+=1;
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                }else if(status.MPI_TAG == MTAK1){
                    if(DEBUG)printf("[SPEC_1_RESSURECT]\t%d\tOtrzymuje MTAK1 do %d!\n",this->process_id, status.MPI_SOURCE);
                    this->data.mission_unassigned-=1;
                    this->data.lamport_clock_value = std::max(this->data.lamport_clock_value,message_buffor[0])+1;
                }
            }
        }

        void lifetime(){
            this->wait_for_mission();
            this->wait_for_specialist_2();
            this->wait_for_team();
            this->ressurection(this->wait_for_table());
        }
};