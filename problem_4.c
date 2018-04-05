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
static inline int atomic_cmpxchg(volatile int *ptr, int old, int New);

struct spin_lock_t{
    volatile int lock;
};
struct spin_lock_t s;

void spin_lock (struct spin_lock_t *s);
void spin_unlock (struct spin_lock_t *s);


/*
 * atomic_cmpxchg
 *
 * equivalent to atomic execution of this code:
 *
 * if (*ptr == old) {
 *   *ptr = new;
 *   return old;
 * } else {
 *   return *ptr;
 * }
 *
 */
static inline int atomic_cmpxchg (volatile int *ptr, int old, int New){
  int ret;
  asm volatile ("lock cmpxchgl %2,%1"
    : "=a" (ret), "+m" (*ptr)
    : "r" (New), "0" (old)
    : "memory");
  return ret;
}

void spin_lock (struct spin_lock_t *s){
    while(atomic_cmpxchg(&(s->lock), 0, 1)==1){}
}

void spin_unlock (struct spin_lock_t *s){
    s->lock = 0;
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
    struct spin_lock_t spin;
    spin.lock = 0;
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


