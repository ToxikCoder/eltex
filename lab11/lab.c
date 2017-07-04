#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
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
 * message = <capacity>, 0 <= capacity < 128
 * Server2client: char message = 1 - work is processed, worker can continue
 * message = 0 - mine is empty, worker should finish working
 */

#define NUM_WORKERS 4
#define PERFORMANCE 5

struct client_packet
{
    short type : 2;
    short message : 7;
};

struct resource
{
    pthread_mutex_t capacity_mutex;
    int capacity;
} mine = { PTHREAD_MUTEX_INITIALIZER, 150 };

void client(int); //client process
void work(int); //sends to server amount of gold extracted
void server(void); //server process
void* job(void*); //server-side job working with clients
int main(void);
void handle_error(char*);
void stop_children(void); //stop child processes

pid_t children[NUM_WORKERS];
pthread_t workers[NUM_WORKERS];

void handle_error(char* message)
{
    perror(message);
    fflush(stderr);
    exit(-1);
}

void stop_children()
{
    for(short i = 0; i < NUM_WORKERS; ++i)
    {
        waitpid(children[i], NULL, 0);
        fprintf(stdout, "Base: Worker #%d returned to base\n", children[i]);
        fflush(stdout);
    }
}

void client(int client_num)
{
    srand((unsigned) getpid());
    //connecting to server
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
    {
        handle_error("Failed to create socket");
    }
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
    int r = 0, count = 0, extracted = 0;
    while(true)
    {
        if(first)
        {
            r = sendto(sock, &clp, sizeof(clp), 0, (struct sockaddr *) &addr,
                serv_len);
            if(r == -1)
            {
                handle_error("Failed to send job to server");
            }
            first = false;
        }
        r = recvfrom(sock, &serv_message, sizeof(char), 0, (struct sockaddr *) &addr,
            &serv_len);
        if(r == -1)
        {
            handle_error("Failed to receive message from server");
        }
        if(serv_message == 0)
        {
            fprintf(stdout, "Worker #%d: Mine is empty, going home\n", client_num);
            fflush(stdout);
            break;
        }
        else if(serv_message == 1)
        {
            r = sendto(sock, &clp, sizeof(clp), 0, (struct sockaddr *) &addr,
                serv_len);
            extracted += 5;
            ++count;
            fprintf(stdout, "Worker #%d: extracted %d gold\n", client_num, PERFORMANCE);
            fflush(stdout);
        }
        //sleep((unsigned) rand() % 3);
    }
    fprintf(stdout, "Worker #%d: going to base. Extracted %d in %d visits\n",
        client_num, extracted, count);
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
        int r = pthread_join(workers[i], NULL);
        if(r == -1)
        {
            handle_error("Failed to join thread\n");
        }
    }

    printf("Closing socket\n");
    fflush(stdout);
    close(sock);
}

void* job(void* arg)
{
    int* sock_ptr = (int *) arg; //pointer to server socket
    struct client_packet clp;
    memset(&clp, 0, sizeof(clp));
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(struct sockaddr_in);
    char serv_message = 0;
    int r = 0;
    while(true)
    {
        //locking the resource
        r = pthread_mutex_lock(&mine.capacity_mutex);
        if(r == -1)
        {
            continue;
        }
        //waiting for packet from client
        r = recvfrom(*sock_ptr, &clp, sizeof(clp), 0,
            (struct sockaddr *) &client_addr, &client_addr_len);
        if(r == -1)
        {
            handle_error("Failed to receive message from client");
        }

        if(mine.capacity > clp.message)
        {
            mine.capacity -= clp.message;
            serv_message = 1;
        }
        else if(mine.capacity <= PERFORMANCE)
        {
            mine.capacity = 0;
            serv_message = 0;
        }
        r = sendto(*sock_ptr, &serv_message, sizeof(char), 0,
            (struct sockaddr *) &client_addr, client_addr_len);
        if(r == -1)
        {
            handle_error("Failed to send message to client");
        }
        fprintf(stdout, "Base: worker #%d extracted %d, %d left\n", clp.type,
            clp.message, mine.capacity);
        fflush(stdout);
        if(mine.capacity == 0)
        {
            pthread_mutex_unlock(&mine.capacity_mutex);
            break;
        }
        pthread_mutex_unlock(&mine.capacity_mutex);
    }
    sock_ptr = NULL;
    fprintf(stdout, "Base: sending worker #%d to base\n", clp.type);
    fflush(stdout);
    pthread_exit(0);
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
            client(i);
        }
        else
        {
            children[i] = child_pid;
        }
    }
    server();
    stop_children();
    fprintf(stdout, "Base: goodbye!\n");
    fflush(stdout);
    return 0;
}
