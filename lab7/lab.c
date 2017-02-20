#define _POSIX_SOURCE
#define _XOPEN_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#define NUM_WORKERS 4
#define PERFORMANCE 4
#define READ 0
#define WRITE 1

int capacity = 100;

void work(int out, int empty);
void serve();

static int count = 0;
static int sleeptime = 0;
pid_t children[NUM_WORKERS];
int ins[NUM_WORKERS];
int empty[NUM_WORKERS];

void work(int out, int empty)
{
	pid_t cur_pid = getpid();
	srand((unsigned) cur_pid);
	fcntl(empty, F_SETFL, fcntl(empty, F_GETFL) | O_NONBLOCK);
	count = 0;
	sleeptime = 0;
	fprintf(stdout, "Worker #%d is going to mine...\n", cur_pid);
	fflush(stdout);
	int b = 1;
	int buf = 0;
	while(true)
	{
		int r = read(empty, &buf, sizeof(int));
		if(r > 0)
		{
			if(buf == 1)
			{
				fprintf(stdout, "Going home. Worker #%d visited mine %d times and sleeped for %d seconds\n", getpid(), count, sleeptime);
				fflush(stdout);
				exit(0);
			}
		}
		else if(r < 0 && errno != EAGAIN)
		{
			perror("Some error ocurred. THe pipe is possibly closed.");
			exit(-1);
		}
		
		write(out, &b, sizeof(int));
		++count;
		int sl = rand() % 3;
		sleeptime +=  sl;
		fflush(stdout);
		sleep((unsigned)sl);
	}
}

void serve()
{
	int buf = 0;
	while(true)
	{
		if(capacity <= 0)
			break;

		for(short i = 0; i < NUM_WORKERS; ++i)
		{
			if(capacity <= 0)
			{	
				int mine_empty = 1;
				//write 1 to notify about empty mine
				for(int i = 0; i < NUM_WORKERS; ++i)
				{
					write(empty[i], &mine_empty, sizeof(int));
				}
			}
			else
			{
				read(ins[i], &buf, sizeof(int));
				if(buf == 1 && capacity >= PERFORMANCE)
					capacity -= PERFORMANCE;
				else if(buf == 1 && capacity < PERFORMANCE)
					capacity = 0;

				fprintf(stdout, "Worker #%d extracted %d gold from mine. Left: %d\n", children[i], PERFORMANCE, capacity);
				fflush(stdout);
			}
		}
	}
	sleep(2);
	fprintf(stdout, "Mine is empty, workers are at base\n");
	fflush(stdout);
}


int main(void)
{
	pid_t pid;
	for(short i = 0; i < NUM_WORKERS; ++i)
	{
		int comm_main[2];
		int empty_notifier[2];	
		//pipes initialisation
		if( (pipe(comm_main) != 0) || (pipe(empty_notifier) != 0) )
		{
			fprintf(stderr, "Error creating pipes for communication\n");
			fflush(stderr);
			exit(-1);
		}

		fflush(stdout);
		pid = fork();
		if(pid == 0)
		{
			close(comm_main[READ]);	
			close(empty_notifier[WRITE]);
			work(comm_main[WRITE], empty_notifier[READ]);
		}
		else
		{
			children[i] = pid;
			ins[i] = comm_main[READ];
			empty[i] = empty_notifier[WRITE];
			close(comm_main[WRITE]);
			close(empty_notifier[READ]);
		}
	}

	serve();

	return 0;
}
