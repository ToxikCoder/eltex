#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#define NUM_WORKERS 4
#define PERFORMANCE 4

int main(void);
void* work(void*);
void err_exit(int r);

void err_exit(int r)
{
    if(r == -1)
    {
        perror("Error: ");
        fflush(stderr);
        exit(-1);
    }
}

struct resource
{
    pthread_mutex_t capacity_mutex;
    int capacity;
} mine = {PTHREAD_MUTEX_INITIALIZER, 100};

void* work(void* arg)
{
    int count = 0;
    unsigned worker = (unsigned) pthread_self();
    while(true)
    {
        int result = pthread_mutex_lock(&mine.capacity_mutex);
        if(result == -1)
            continue;

        if(mine.capacity > PERFORMANCE)
        {
            mine.capacity -= PERFORMANCE;
        }
        else if(mine.capacity == 0)
        {
            pthread_mutex_unlock(&mine.capacity_mutex);
            break;
        }
        else if(mine.capacity <= PERFORMANCE)
        {
            mine.capacity = 0;
        }
        ++count;
        fprintf(stdout, "Worker #%u: extracted %d, %d left\n", worker,
            PERFORMANCE, mine.capacity);
        fflush(stdout);
        pthread_mutex_unlock(&mine.capacity_mutex);
        unsigned sleep_time = rand() % 3;
        sleep(sleep_time);
    }
    fprintf(stdout, "Worker #%u: mine is empty, going home\n", worker);
    fprintf(stdout, "Worker #%u: mine is visited %d times\n", worker, count);
    fflush(stdout);
    pthread_exit(0);
}

int main()
{
    pthread_t workers[NUM_WORKERS];
    fprintf(stdout, "Workers are going to mine...\n");
    fflush(stdout);
    int result = 0;

    for(short i = 0; i < NUM_WORKERS; ++i)
    {
        result = pthread_create(&workers[i], NULL, work, NULL);
        err_exit(result);
    }
    for(short i = 0; i < NUM_WORKERS; ++i)
    {
        result = pthread_join(workers[i], NULL);
        err_exit(result);
    }
    fprintf(stdout, "Workers are at base\n");
    fflush(stdout);
    return 0;
}
