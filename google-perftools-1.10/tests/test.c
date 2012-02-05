#include <stdio.h>
#include <pthread.h>

#define NUM_THREAD 1
#define NUM_LOOP 1000000

static int num_thread = 1;
static int num_loop = 1000;

static void *worker(void *arg) {
    int *id = arg;
    int * i = NULL;
 
    for (int k = 1; k < num_loop; k++)  {
       i = calloc(k, sizeof(int));
       free(i);
    }
    printf("within thread %d.\n", *id);
    return NULL;
}

int main(int argc, char **argv)
{
    pthread_attr_t  attr;
    pthread_t id;
    pthread_t *thread_ids;

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
    thread_ids = calloc(num_thread, sizeof(pthread_t));
    ids = calloc(num_thread, sizeof(int));
    for (int i = 0; i < num_thread; i++) {
        ids[i] = i;
        if ((ret = pthread_create(&thread_ids[i], &attr, worker, &ids[i])) != 0) {
           printf("Failed to create thread %d\n", i);
        }
    }

    for (int ii = 0; ii < num_thread; ++ii) {
        pthread_join(thread_ids[ii], NULL);
    }

    return 0;
}
