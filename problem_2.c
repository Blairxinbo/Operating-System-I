

//
// Created by xinbo on 29/03/2018.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

volatile int in_cs=0;
volatile int count_cs =0;
volatile int* Number;
volatile int* Entering;
volatile int* count;
int NUM_threads;
int execution_time;
int* pid;


int max(int* Number){
    int i, max_number = 0;
    for(i = 0; i < NUM_threads; i++){
        if (Number[i] > max_number){
            max_number = Number[i];
        }
    }
    return max_number;
}

void lock(int thread) {

    int j;
    Entering[thread] = 1;
    Number[thread] = 1 + max((int *)Number);
    Entering[thread] = 0;
    for (j = 0; j<= NUM_threads; j++){
        if (j != thread) {
            while(Entering[j]) {sched_yield();
            }
            while(Number[j] != 0 && ((Number[thread] > Number[j])
                                     || (Number[thread] == Number[j] && thread > j))){assert(sched_yield() == 0);}
        }

    }
}

void unlock(int thread){
    Number[thread] = 0;
}

void* create_thread(void *arg){

    int threads = *((int *)arg);

    while (1){
        count[threads]+=1;
        lock(threads);

        assert (in_cs==0);
        in_cs++;
        assert (in_cs==1);
        in_cs++;
        assert (in_cs==2);
        in_cs++;
        assert (in_cs==3);
        in_cs=0;
        count_cs++;

        unlock(threads);
    }
}


int main(int argc, char* argv[]){
    int i;
    int input = argc;
    if (input != 3){
        printf("Please input the correct number of arguments.\n");
        return 1;
    }

    NUM_threads = atoi(argv[1]);
    execution_time = atoi(argv[2]);

    assert((Entering = calloc(NUM_threads, sizeof(int)))!= NULL);
    assert((count = calloc(NUM_threads, sizeof(int)))!= NULL);
    assert((Number = calloc(NUM_threads, sizeof(int)))!= NULL);
    assert((pid = malloc(sizeof(int) * NUM_threads))!= NULL);

    pthread_t in_thread[NUM_threads];
    for (i = 0; i < NUM_threads; i++){
        pid[i] = i;
        assert(pthread_create(&in_thread[i], NULL, (void *) &create_thread, pid+i)==0);
    }

    sleep(execution_time);
    for (i=0; i < NUM_threads; i++){
        printf("Thread enters %d times. \n", count[i]);
    }
    printf("critical section enters %d times. \n", (int)count_cs);

    return 0;

}

