#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>

/*
 * 1 - fork 4 clients
 * 2 - start listening
 * 3 - connect to server with every client
 * 4 - create a thread for every client
 * 5 - work with client socket in thread
 * 6 - close connections, when mine is empty
 * 7 - stop threads
 * 8 - close listening socket
 */

#define NUM_WORKERS 4
#define PERFORMANCE 4

struct resource
{
    pthread_mutex_t capacity_mutex;
    int capacity;
} mine = { PHTREAD_MUTEX_INITIALIZER, 200 };

void client(void); //client process
void work(int); //sends to server amount of gold extracted
void server(void); //server process
void* job(void*); //server-side job working with clients
int main(void);

void client()
{

}

void server()
{

}

int main()
{
    pid_t child_pid;
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
            client();
        }
    }
    server();
    for(short i = 0; i < NUM_WORKERS; ++i)
    {
        waitpid(children[i], NULL, 0);
        fprintf(stdout, "Base: Worker #%d returned to base\n", children[i]);
        fflush(stdout);
    }
    return 0;
}
