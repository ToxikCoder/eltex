#define _POSIX_SOURCE
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

#define NUM_WORKERS 4
#define PERFORMANCE 4

int main(void);
void work(void);
int get_shm_id(void);
int get_sem_id(void);
void check_error(int);

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

pid_t children[NUM_WORKERS];
struct sembuf lock = {0, -1, 0}; //lock resource
struct sembuf release = {0, 1, 0}; //release resource

void check_error(int c)
{
    if(c == -1)
    {
        perror("Error: ");
        fflush(stderr);
        exit(-1);
    }
}

int get_shm_id()
{
    key_t key = ftok("/dev/null", '&');
    return shmget(key, sizeof(int), IPC_CREAT | 0666);
}

int get_sem_id()
{
    key_t key = ftok("/dev/null", '%');
    return semget(key, 1, IPC_CREAT | 0666);
}

void work()
{
    int count = 0;
    int shmid = get_shm_id();
    check_error(shmid);
    int* ptr;
    int semid = get_sem_id();
    bool mine_empty = false;
    while(true)
    {
        //try to block shared memory
        int r = semop(semid, &lock, 1);
        if(r == -1)
            continue;

        ptr = shmat(shmid, NULL, IPC_CREAT);
        if(ptr == (int *) -1)
        {
            semop(semid, &release, 1);
            perror("Failed to bind to shared memory. ");
            fflush(stderr);
            exit(-1);
        }
        if(*ptr > PERFORMANCE)
        {
            *ptr -= PERFORMANCE;
        }
        else if(*ptr == 0)
        {
            mine_empty = true;
            //release shared memory
            semop(semid, &release, 1);
            shmdt(ptr);
            break;
        }
        else if(*ptr <= PERFORMANCE)
        {
            *ptr = 0;
            mine_empty = true;
        }
        ++count;
        fprintf(stdout, "Worker #%d: extracted 4 gold. ", getpid());
        fprintf(stdout, "Capacity: %d\n", *ptr);
        fflush(stdout);
        //release shared memory
        semop(semid, &release, 1);
        shmdt(ptr);
        if(mine_empty)
            break;

        sleep(1);
    }
    fprintf(stdout, "Worker #%d: mine is empty, going home. Visited mine %d times\n",
        getpid(), count);
    exit(0);
}

int main(void)
{
    pid_t child_pid;
    //creating shared memory with capacity
    int shmid = get_shm_id();
    check_error(shmid);
    int *capacity;
    capacity = shmat(shmid, NULL, IPC_CREAT);
    if(capacity == (int *) -1)
    {
        perror("Failed to allocate shared memory. ");
        fflush(stderr);
        exit(-1);
    }
    *capacity = 100; //mine capacity
    shmdt(capacity);

    //creating semaphore for shared memory
    int semid = get_sem_id();
    union semun arg;
    arg.val = 1;
    int r = semctl(semid, 0, SETVAL, arg);
    check_error(r);

    for(short i = 0; i < NUM_WORKERS; ++i)
    {
        child_pid = fork();
        if(child_pid < 0)
        {
            perror("Failed to fork process.");
            fflush(stderr);
            exit(-1);
        }
        else if(child_pid == 0)
        {
            work();
        }
    }
    for(short i = 0; i < NUM_WORKERS; ++i)
    {
        waitpid(children[i], NULL, 0);
        fprintf(stdout, "Base: Worker #%d returned to base\n", children[i]);
        fflush(stdout);
    }
    r = shmctl(shmid, IPC_RMID, NULL);
    check_error(r);
    r = semctl(semid, 0, IPC_RMID);
    check_error(r);
    return 0;
}
