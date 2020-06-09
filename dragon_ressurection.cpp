#include<iostream>
#include "constants.h"
#include "employer.cpp"
#include "specialist_1.cpp"
#include "specialist_2.cpp"
#include "specialist_3.cpp"
int main(int argc, char *argv[]){

    MPI_Init(&argc,&argv);
    int process_count;
    int process_id; 
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    if(process_id==0)printf("Liczba procesow: %d!\n",process_count);
    int temp = 0;
    specialist sp;
    sp.mission_unassigned = 0;
    sp.guild_table_count = 3;
    sp.initial_skeleton_count = 3;
    for(int i = process_count - 1; i>0; i-=3)
    temp++;
    if(process_id==0)printf("Liczba specjalistow 1: %d\n",temp);
    sp.expert_count = temp;
    sp.lamport_clock_value = 0;
    specialist sp2;
    sp2.mission_unassigned = 0;
    sp2.guild_table_count = 3;
    sp2.initial_skeleton_count = 3;
    temp = 0;
    for(int i = process_count - 2; i>0; i-=3)
    temp++;
    if(process_id==0)printf("Liczba specjalistow 2: %d\n",temp);
    sp2.expert_count = temp;
    sp2.lamport_clock_value = 0;
    specialist sp3;
    sp3.mission_unassigned = 0;
    sp3.guild_table_count = 3;
    sp3.initial_skeleton_count = 3;
    temp = 0;
    for(int i = process_count - 3; i>0; i-=3)
    temp++;
    if(process_id==0)printf("Liczba specjalistow 3: %d\n",temp);
    sp3.expert_count = temp;
    sp3.lamport_clock_value = 0;
    if(process_id == 0){
    Employer employer;
    printf("%d jako employer!\n",process_id);
    employer.process_lifetime();
    }
    else if((process_id-1) % 3 == 0){
    Specialist_1 specialist_1__(sp);
    printf("%d jako specialist 1!\n",process_id);
    while(true)
    specialist_1__.lifetime();
    }
    else if((process_id-1) % 3 == 1){
    Specialist_2 specialist_2__(sp2);
    printf("%d jako specialist 2!\n",process_id);
    while(true)
    specialist_2__.lifetime();
    }
    else if((process_id-1) % 3 == 2){
    Specialist_3 specialist_3__(sp3);
    printf("%d jako specialist 3!\n",process_id);
    while(true)
    specialist_3__.lifetime();
    }

    MPI_Finalize();

}