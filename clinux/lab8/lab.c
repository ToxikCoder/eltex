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
void work(int, int);
void control(int);
void check_error(int);

struct msgbuf
{
    long mtype;
    char mtext[1];
};

pid_t children[NUM_WORKERS];
int visit_counter[NUM_WORKERS];

/* Message types:
 * 1-4 - message from woker, text=1 - work cycle
 * 10+<child_type> - message from base for <child_type> child
 *   text=1 - work processed, worker should continue
 *   text=2 - mine empty, workers should finish working
 */

void check_error(int r)
{
    if(r == -1 && errno != ENOMSG)
    {
        perror("Error: ");
        fflush(stderr);
        exit(-1);
    }
}

void work(int msqid, int message_type)
{
    pid_t this_worker = getpid();
    struct msgbuf rec; //for receiving messages
    struct msgbuf message; //for sending messages
    message.mtype = message_type;
    message.mtext[0] = 1;
    bool first = true;
    while(true)
    {
        ssize_t size = msgrcv(msqid, &rec, 1, 10 + message_type, IPC_NOWAIT);
        check_error(size);
        if(size == 1 && rec.mtext[0] == 2)
            break; //0 - mine is empty, stop working
        if(first || (size == 1 && rec.mtext[0] == 1))
        {
            //base processed last delivery, continue mining
            first = false;
            int result = msgsnd(msqid, &message, 1, 0);
            check_error(result);
        }
    }
    fprintf(stdout, "Worker #%d: Mine is empty, going home\n", this_worker);
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
        for(short i = 1; i <= NUM_WORKERS; ++i)
        {
            ssize_t size = msgrcv(msqid, &message, 1, i, IPC_NOWAIT);
            check_error(size);
            if(size == 1 && message.mtext[0] != 1)
                continue;

            if(capacity > PERFORMANCE)
            {
                message.mtype = 10 + i;
                message.mtext[0] = 1;
                int r = msgsnd(msqid, &message, 1, 0);
                check_error(r);
                capacity -= PERFORMANCE;
            }
            else if(capacity <= PERFORMANCE)
            {
                capacity = 0;
                mine_empty = true;
            }
            visit_counter[i-1] += 1;
            fprintf(stdout,
                "Base: Mine capacity: %d, worker #%d extracted %d gold\n",
                capacity, children[i-1], PERFORMANCE);
            fflush(stdout);
            if(mine_empty)
                break;
        }
        if(mine_empty)
        {
            fprintf(stdout, "Base: Sending workers home...\n");
            fflush(stdout);
            message.mtext[0] = 2;
            for(short i = 1; i <= NUM_WORKERS; ++i)
            {
                message.mtype = 10 + i;
                int result = msgsnd(msqid, &message, 1, 0);
                check_error(result);
            }
            break;
        }
    }
    for(short i = 0; i < NUM_WORKERS; ++i)
    {
        fprintf(stdout, "Base: Worker #%d visited mine %d times\n",
            children[i], visit_counter[i]);
    }
}

int main(void)
{
    key_t key = ftok("/dev/null", '!');
    int queue = msgget(key, IPC_CREAT | 00660);
    check_error(queue);
    pid_t child_pid;
    for(short i = 1; i <= NUM_WORKERS; ++i)
    {
        child_pid = fork();
        //child process
        if(child_pid == 0)
        {
            work(queue, i);
        }
        else if(child_pid < 0)
        {
            perror("Failed to fork process: ");
            fflush(stderr);
            exit(-1);
        }
        else
            children[i-1] = child_pid;
    }
    control(queue);
    for(short i = 0; i < NUM_WORKERS; ++i)
    {
        waitpid(children[i], NULL, 0);
        fprintf(stdout, "Base: Worker #%d returned to base\n", children[i]);
        fflush(stdout);
    }
    int r = msgctl(queue, IPC_RMID, 0);
    check_error(r);
    return 0;
}
