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

    static void worker(bool * wake_up_){
        sleep(5);
        *wake_up_ = !*wake_up_;
    }

    static void *worker_helper(void *wake_up_){
        worker((bool*)wake_up_);
        pthread_exit(NULL);
    }
    
    void go(){
        pthread_t my_thread;
        int ret = pthread_create(&my_thread, NULL, &worker_helper, (void*)this->wake_up);
    }
};