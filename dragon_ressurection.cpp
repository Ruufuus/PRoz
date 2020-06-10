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
    if(argc != 7){
        if(process_id == 0){
            printf("Wpisano %d argumentow z 7!\n",argc);
            printf("Oczekiwane argumenty to: liczba_zleceniodawcow liczba_specjalistow_1 liczba_specjalistow_2 liczba_specjalistow_3 liczba_stolow_w_gildii poczatkowa_liczba_szkieletow!\n");
        }
        MPI_Finalize();
    }
    int employer_count = atoi(argv[1]);
    int spec_1_count = atoi(argv[2]);
    int spec_2_count = atoi(argv[3]);
    int spec_3_count = atoi(argv[4]);
    int table_count = atoi(argv[5]);
    int skeleton_count = atoi(argv[6]);

    if(process_id==0){
        printf("Liczba procesow: %d!\n",process_count);
        printf("Liczba zleceniodawcow: %d!\n",employer_count);
        printf("Liczba specjalistow 1: %d!\n",spec_1_count);
        printf("Liczba specjalistow 2: %d!\n",spec_2_count);
        printf("Liczba specjalistow 3: %d!\n",spec_3_count);
        printf("Liczba stolow w gildii: %d!\n",table_count);
        printf("Poczatkowa liczba szkieletow: %d!\n",skeleton_count);
    }
    if(process_id<employer_count){
        Employer employee;
        employee.process_lifetime();
    }else{
        specialist spec;
        spec.lamport_clock_value = 0;
        spec.mission_unassigned = 0;
        spec.initial_skeleton_count = skeleton_count;
        spec.guild_table_count = table_count;
        if(process_id < employer_count + spec_1_count){
            spec.expert_count = spec_1_count;
            Specialist_1 spec1(spec);
            while(true)
            spec1.lifetime();
        }else if(process_id < employer_count + spec_1_count + spec_2_count){
            spec.expert_count = spec_2_count;
            Specialist_2 spec2(spec);
            while(true)
            spec2.lifetime();
        }else if(process_id < employer_count + spec_1_count + spec_2_count + spec_3_count){
            spec.expert_count = spec_3_count;
            Specialist_3 spec3(spec);
            while(true)
            spec3.lifetime();
        }
    }
    

    MPI_Finalize();

}