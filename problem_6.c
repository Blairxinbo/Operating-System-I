//
// Created by xinbo on 29/03/2018.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
volatile int in_circle =0;
int execution_time;
int NUM_threads;

//inline float create_random(float low, float high);
//void* Monte();

static inline int atomic_xadd(volatile int *ptr)
{
    register int val __asm__("eax") = 1;
    asm volatile ("lock xaddl %0,%1"
    : "+r" (val)
    : "m" (*ptr)
    : "memory"
    );
    return val;
}


inline float create_random(float min, float max){
    return (max-min)*((((float)rand())/(float)RAND_MAX)) + min;
}

void* Monte(){
    int i;
    float x,y;
    for(i=0; i<200;i++){
        x = create_random(-1,1);
        y = create_random(-1,1);
        if((x*x+y*y)<=1){
            atomic_xadd(&in_circle);
        }
    }
    return 0;
}

int main(int argc, char* argv[]){
    float pi_val;
    int i;
    int input = argc;
    if (input != 3){
        printf("Please input the correct number of arguments.\n");
        return 1;
    }

    NUM_threads = atoi(argv[1]);
    execution_time = atoi(argv[2]);


    pthread_t in_thread[NUM_threads];
    for (i = 0; i < NUM_threads; i++){
        //pid[i] = i;
        assert(pthread_create(&in_thread[i], NULL, (void *) &Monte, NULL)==0);
    }

    sleep(execution_time);

    pi_val = 4*((in_circle)/(float)(200*NUM_threads));
    printf("The value of pi is %f.\n", pi_val);
    return 0;

}