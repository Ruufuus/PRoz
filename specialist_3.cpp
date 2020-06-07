#include "constants.h"

class Specialist_3: public Thread {
    private:
     specialist data;
    
    public:
        Specialist_3(specialist data){
            this->data = data;
        }
}