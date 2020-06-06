#include<iostream>
#include "constants.h"
#include "employer.cpp"
#include "specialist_1.cpp"
int main(int argc, char *argv[]){

    MPI_Init(&argc,&argv);
    Employer employer;
    specialist sp;
    sp.mission_unassigned = 0;
    sp.expert_count = 3;
    sp.lamport_clock_value = 0;
    Specialist_1 specialist_1__(sp);
    int process_id; 
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    if(process_id == 0)
    employer.process_lifetime();
    else
    specialist_1__.wait_for_mission();

    return 0;
}