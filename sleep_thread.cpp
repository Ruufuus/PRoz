#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class sleeper {
public:
    bool* wake_up;

    sleeper(bool* wake_up){
        this->wake_up = wake_up;
    }

    void *worker(void){
        sleep(3);
        *wake_up = false;
    }

    static void *worker_helper(void *context){
        return ((sleeper *)context)->worker();
    }
    
    void go(){
        pthread_t my_thread;

        int ret = pthread_create(&my_thread, NULL, &worker_helper, NULL);
        pthread_exit(NULL);
    }
};