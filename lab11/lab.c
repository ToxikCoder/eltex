#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <wait.h>

/*
 * 1 - fork 4 clients
 * 2 - open server socket
 * 3 - connect to server with every client
 * 4 - create a thread for every client
 * 5 - work with client socket in thread
 * 6 - close connections, when mine is empty
 * 7 - stop threads
 * 8 - close server socket
 */

/*
 * Client-server protocol
 *
 * Client2server: client_packet: type = 0-3 for each client,
 * message = <extracted gold>, 0 < extracted gold < 64
 * Server2client: char message = 1 - work is processed, worker can continue
 * message = 0 - mine is empty, worker should finish working
 */

#define NUM_WORKERS 4
#define PERFORMANCE 5

typedef unsigned char uchar;

struct client_packet
{
    uchar type : 2;
    uchar message : 6;
};

struct resource
{
    pthread_mutex_t capacity_mutex;
    int capacity;
} mine = { PTHREAD_MUTEX_INITIALIZER, 150 };

void client(uchar); //client process
void server(void); //server process
void* job(void*); //server-side job working with clients
int main(void);
void handle_error(char*);

pid_t children[NUM_WORKERS];
pthread_t workers[NUM_WORKERS];
int count[NUM_WORKERS];

void handle_error(char* message)
{
    perror(message);
    fflush(stderr);
    exit(-1);
}

void client(uchar client_num)
{
    srand((unsigned) getpid());
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
    {
        handle_error("Failed to create socket");
    }
    //server address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(10000);
    socklen_t serv_len = sizeof(addr);

    struct client_packet clp;
    memset(&clp, 0, sizeof(clp));
    clp.type = client_num;
    clp.message = PERFORMANCE;
    char serv_message = 0;
    bool first = true;
    while(true)
    {
        if(first)
        {
            if(sendto(sock, &clp, sizeof(clp), 0, (struct sockaddr *) &addr,
                      serv_len) == -1)
            {
                handle_error("Failed to send job to server");
            }
            first = false;
        }
        if(recvfrom(sock, &serv_message, sizeof(char), 0,
                    (struct sockaddr *) &addr, &serv_len) == -1)
        {
            close(sock);
            handle_error("Failed to receive message from server");
        }
        if(serv_message == 0)
        {
            fprintf(stdout, "Worker #%d: Mine is empty, going home\n",
                    client_num);
            fflush(stdout);
            break;
        }
        else if(serv_message == 1)
        {
            if(sendto(sock, &clp, sizeof(clp), 0, (struct sockaddr *) &addr,
                   serv_len) == -1)
            {
                handle_error("Failed to send new portion of gold");
            }
        }
        sleep((unsigned) rand() % 2);
    }
    fprintf(stdout, "Worker #%d: going to base\n", client_num);
    fflush(stdout);
    close(sock);
    exit(0);
}

void server()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
    {
        handle_error("Failed to create socket");
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(10000);
    if(bind(sock, (struct sockaddr *) &addr, sizeof(addr)) == -1)
    {
        handle_error("Failed to bind socket to address");
        close(sock);
    }
    for(short i = 0; i < NUM_WORKERS; ++i)
    {
        if(pthread_create(&workers[i], NULL, &job, (void *) &sock) == -1)
        {
            close(sock);
            handle_error("Failed to create threads");
        }
    }
    //wait threads to stop working.
    for(short i = 0; i < NUM_WORKERS; ++i)
    {
        if(pthread_join(workers[i], NULL) == -1)
        {
            handle_error("Failed to join thread\n");
        }
    }
    close(sock);
}

void* job(void* arg)
{
    int* sock_ptr = arg; //pointer to server socket
    struct client_packet clp;
    memset(&clp, 0, sizeof(clp));
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(struct sockaddr_in);
    char serv_message = 0;
    while(true)
    {
        //locking the resource
        if(pthread_mutex_lock(&mine.capacity_mutex) == -1)
        {
            continue;
        }
        //waiting for packet from client
        if(recvfrom(*sock_ptr, &clp, sizeof(clp), 0,
                    (struct sockaddr *) &client_addr, &client_addr_len) == -1)
        {
            handle_error("Failed to receive message from client");
        }

        if(mine.capacity > clp.message)
        {
            mine.capacity -= clp.message;
            serv_message = 1;
            count[clp.type] += 1;
        }
        else if(mine.capacity == 0)
        {
            serv_message = 0;
        }
        else if(mine.capacity <= clp.message)
        {
            mine.capacity = 0;
            serv_message = 0;
            count[clp.type] += 1;
        }
        //sending mine and processing status
        if(sendto(*sock_ptr, &serv_message, sizeof(char), 0,
                  (struct sockaddr *) &client_addr, client_addr_len) == -1)
        {
            handle_error("Failed to send message to client");
        }
        if(mine.capacity == 0)
        {
            pthread_mutex_unlock(&mine.capacity_mutex);
            break;
        }
        fprintf(stdout, "Base: worker #%d extracted %d, %d left\n", clp.type,
                clp.message, mine.capacity);
        fflush(stdout);
        pthread_mutex_unlock(&mine.capacity_mutex);
    }
    sock_ptr = NULL;
    fprintf(stdout, "Base: sending worker #%d to base. This worker visited mine "
            "%d times\n", clp.type, count[clp.type]);
    fflush(stdout);
    pthread_exit(0);
}

int main()
{
    pid_t child_pid;
    for(char i = 0; i < NUM_WORKERS; ++i)
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
            client(i);
        }
        else
        {
            children[i] = child_pid;
        }
    }
    server();
    for(uchar i = 0; i < NUM_WORKERS; ++i)
    {
        waitpid(children[i], NULL, 0);
    }
    fprintf(stdout, "Base: workers are at base. Goodbye!\n");
    fflush(stdout);
    return 0;
}
