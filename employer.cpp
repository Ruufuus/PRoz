#include "constants.h"
class Employer {
    private:
        int process_id; 

    Employer(){
        process_id = MPI_COMM_RANK;
    }

};