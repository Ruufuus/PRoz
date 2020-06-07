#include<iostream>
#include "constants.h"
#include "employer.cpp"
#include "specialist_1.cpp"
#include "specialist_2.cpp"
#include "specialist_3.cpp"
int main(int argc, char *argv[]){

    MPI_Init(&argc,&argv);
    specialist sp;
    sp.mission_unassigned = 0;
    sp.guild_table_count = 3;
    sp.initial_skeleton_count = 3;
    sp.expert_count = 1;
    sp.lamport_clock_value = 0;
    int process_id; 
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    if(process_id == 0){
    Employer employer;
    employer.process_lifetime();
    }
    else if(process_id == 1){
    Specialist_1 specialist_1__(sp);
    while(true)
    specialist_1__.lifetime();
    }
    else if(process_id == 2){
    Specialist_2 specialist_2__(sp);
    while(true)
    specialist_2__.lifetime();
    }
    else if(process_id == 3){
    Specialist_3 specialist_3__(sp);
    while(true)
    specialist_3__.lifetime();
    }



}