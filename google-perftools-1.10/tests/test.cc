#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lock.h"

#define NUM_THREAD 1
#define NUM_LOOP 1000000

static int num_thread = 1;
static int num_loop = 1000;
static int counter = 0;
static void *worker(void *arg) {
    //int *id = reinterpret_cast<int*>(arg);
    int * i = NULL;
    
    SpinLockHolder((SpinLock*)arg);
    
    for (int k = 0; k < num_loop; k++)  {
       //i = (int*)calloc(rand() % num_loop, sizeof(int));
       //free(i); 
	   counter++;
    }
    printf("Done with the thread, counter=%d.\n", counter);
    return NULL;
}

int main(int argc, char **argv)
{
    pthread_attr_t  attr;
    pthread_t id;
    pthread_t *thread_ids;
    SpinLock lock;

    int             ret;
    int *ids;
    
    if (argc > 1)  {
        num_thread = atol(argv[1]);
    }
    if (argc > 2) {
        num_loop = atol(argv[2]);
    }
    printf("thread:%d, loop:%d\n", num_thread, num_loop);

    pthread_attr_init(&attr);
    thread_ids = (pthread_t*)calloc(num_thread, sizeof(pthread_t));
    ids = (int*)calloc(num_thread, sizeof(int));
    for (int i = 0; i < num_thread; i++) {
        ids[i] = i;
        if ((ret = pthread_create(&thread_ids[i], &attr, worker, &lock)) != 0) {
           printf("Failed to create thread %d\n", i);
        }
    }

    for (int ii = 0; ii < num_thread; ++ii) {
        pthread_join(thread_ids[ii], NULL);
    }

    return 0;
}
