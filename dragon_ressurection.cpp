#include<iostream>
#include "constants.h"
#include "employer.cpp"
int main(int argc, char *argv[]){

    MPI_Init(&argc,&argv);
    Employer employer;
    employer.process_lifetime();
    return 0;
}