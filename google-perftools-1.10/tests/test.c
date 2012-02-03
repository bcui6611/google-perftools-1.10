#include <stdio.h>
#include <pthread.h>

#define NUM_THREAD 1
#define NUM_LOOP 100000
static void *worker(void *arg) {
    int *id = arg;
    int * i = NULL;
 
    for (int k = 1; k < NUM_LOOP; k++)  {
       i = calloc(k, sizeof(int));

       free(i);
    }
    printf("within thread %d.\n", *id);
    return NULL;
}

int main(int agrc, char **argv)
{
    pthread_attr_t  attr;
    pthread_t id;
    pthread_t *thread_ids;

    int             ret;
    int *ids;

    pthread_attr_init(&attr);

    thread_ids = calloc(NUM_THREAD, sizeof(pthread_t));
    ids = calloc(NUM_THREAD, sizeof(int));
    for (int i = 0; i < NUM_THREAD; i++) {
        ids[i] = i;
        if ((ret = pthread_create(&thread_ids[i], &attr, worker, &ids[i])) != 0) {
           printf("Failed to create thread %d\n", i);
        }
    }

    for (int ii = 0; ii < NUM_THREAD; ++ii) {
        pthread_join(thread_ids[ii], NULL);
    }

    return 0;
}
