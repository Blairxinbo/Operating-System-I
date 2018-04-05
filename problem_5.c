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

void* create_thread(void *arg);

struct spin_lock_t{
    volatile int current;
    volatile int waiting;
};
struct spin_lock_t s;

/*
 * atomic_xadd
 *
 * equivalent to atomic execution of this code:
 *
 * return (*ptr)++;
 *
 */
static inline int atomic_xadd (volatile int *ptr)
{
    register int val __asm__("eax") = 1;
    asm volatile ("lock xaddl %0,%1"
    : "+r" (val)
    : "m" (*ptr)
    : "memory"
    );
    return val;
}

void spin_lock (struct spin_lock_t *s);
void spin_unlock (struct spin_lock_t *s);


void spin_lock (struct spin_lock_t *s){
    volatile int on_line = atomic_xadd(&(s->waiting));
    while(on_line != s->current){}
}

void spin_unlock (struct spin_lock_t *s){
    atomic_xadd(&(s->current));
}

int max(int* Number){
    int i, max_number = 0;
    for(i = 0; i < NUM_threads; i++){
        if (Number[i] > max_number){
            max_number = Number[i];
        }
    }
    return max_number;
}



void* create_thread(void *arg){

    struct spin_lock_t * spin = (struct spin_lock_t *) arg;

    while (1){
        spin_lock(spin);

        assert (in_cs==0);
        in_cs++;
        assert (in_cs==1);
        in_cs++;
        assert (in_cs==2);
        in_cs++;
        assert (in_cs==3);
        in_cs=0;
        count_cs++;

        spin_unlock(spin);
    }
}


int main(int argc, char* argv[]){
    int i;
    struct spin_lock_t spin = {0,1};

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
        assert(pthread_create(&in_thread[i], NULL, (void *) &create_thread, &spin)==0);
    }

    sleep(execution_time);

    printf("critical section enters %d times. \n", (int)count_cs);

    return 0;

}


