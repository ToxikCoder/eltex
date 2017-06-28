#define _POSIX_SOURCE
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#define NUM_WORKERS 4
#define PERFORMANCE 4

int main(void);
void work(int);
void control(int);
void check_error(int);

struct msgbuf
{
    long mtype;
    char mtext[1];
};

pid_t children[NUM_WORKERS];

void check_error(int r)
{
    if(r == -1 && errno != ENOMSG)
    {
        perror("Error: ");
        fflush(stdout);
        exit(-1);
    }
}

void work(int msqid)
{
    int count = 0; //mine visits counter
	int sleeptime = 0; //sleep time
    pid_t this_worker = getpid();
    fprintf(stdout, "Worker #%d is going to mine\n", this_worker);
    fflush(stdout);
    struct msgbuf rec; //for receiving messages
    struct msgbuf message; //for sending messages
    message.mtype = this_worker;
    message.mtext[0] = 1;
    sleep(3);
    bool first = true;
    while(true)
    {
        ssize_t size = msgrcv(msqid, &rec, 1, this_worker, IPC_NOWAIT);
        check_error(size);
        if(size == 1 && rec.mtext[0] == 0)
            break; //0 - mine is empty, stop working
        if(first || rec.mtext[0] == 2)
        {
            //base is processed last delivery, continue mining
            first = false;
            int result = msgsnd(msqid, &message, 1, 0);
            check_error(result);
            ++count;
            int sl = rand() % 2;
            sleeptime += sl;
            sleep((unsigned) sleeptime);
        }
    }
    fprintf(stdout, "Mine is empty, worker #%d is going home\n", this_worker);
    fprintf(stdout, "Worker %d visited mine %d times and sleeped for %d\n",
        this_worker, count, sleeptime);
    fflush(stdout);
    exit(0);
}

void control(int msqid)
{
    struct msgbuf message;
    int capacity = 100;
    bool mine_empty = false;
    while(true)
    {
        for(short i = 0; i < NUM_WORKERS; ++i)
        {
            ssize_t r = msgrcv(msqid, &message, 1, children[i], IPC_NOWAIT);
            check_error(r);

            if(message.mtext[0] != 1)
                continue;

            if(capacity != 0)
            {
                message.mtype = children[i];
                message.mtext[0] = 2;
                int r = msgsnd(msqid, &message, 1, 0);
                check_error(r);
                capacity -= PERFORMANCE;
                fprintf(stdout,
                    "Mine capacity: %d, worker #%d extracted %d of gold\n",
                    capacity, children[i], PERFORMANCE);
                fflush(stdout);
            }
            else
            {
                mine_empty = true;
                break;
            }
        }
        if(mine_empty)
        {
            message.mtext[0] = 0;
            for(short i = 0; i < NUM_WORKERS; ++i)
            {
                message.mtype = children[i];
                int result = msgsnd(msqid, &message, 1, 0);
                check_error(result);
            }
            break;
        }
    }
}

int main(void)
{
    key_t key = ftok("/dev/null", '!');
    int queue = msgget(key, IPC_CREAT | 00660);
    check_error(queue);
    pid_t child_pid;
    for(short i = 0; i < NUM_WORKERS; ++i)
    {
        child_pid = fork();
        //child process
        if(child_pid == 0)
        {
            work(queue);
        }
        else
            children[i] = child_pid;
    }
    control(queue);
    sleep(2);
    int r = msgctl(queue, IPC_RMID, 0);
    check_error(r);
    return 0;
}
