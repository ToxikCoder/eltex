#define _POSIX_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>

#define NUM_WORKERS 4
#define PERFORMANCE 4
#define READ 0
#define WRITE 1

int capacity = 100;

void work(int out);
void serve();

static int count = 0;
static int sleeptime = 0;
pid_t children[NUM_WORKERS];
int ins[NUM_WORKERS];

static void onHome()
{
	fprintf(stdout, "Going home. Worker #%d visited mine %d times and sleeped for %d seconds\n", getpid(), count, sleeptime);
	fflush(stdout);
}

void work(int out)
{
	pid_t cur_pid = getpid();
	srand((unsigned) cur_pid);
	signal(SIGTERM, onHome);
	count = 0;
	sleeptime = 0;
	fprintf(stdout, "Worker #%d is going to mine...\n", cur_pid);
	fflush(stdout);
	int b = 1;
	while(true)
	{
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
		{			
			printf("All workers finished working\n");
			for(short i = 0; i < NUM_WORKERS; ++i)
				kill(children[i], SIGTERM);
			break;
		}
		for(short i = 0; i < NUM_WORKERS; ++i)
		{
			read(ins[i], &buf, sizeof(int));
			if(buf == 1 && capacity >= PERFORMANCE)
				capacity -= PERFORMANCE;
			else if(buf ==1 && capacity < PERFORMANCE)
				capacity = 0;

			fprintf(stdout, "Worker #%d extracted %d gold from mine. Left: %d\n", children[i], PERFORMANCE, capacity);
			fflush(stdout);
		}
	}
	sleep(2);
	printf("Mine is empty, workers are at base\n");
}


int main(void)
{
	pid_t pid;
	for(short i = 0; i < NUM_WORKERS; ++i)
	{
		int comm_main[2];	
		//pipes initialization
		if(pipe(comm_main) != 0)
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
			work(comm_main[WRITE]);
		}
		else
		{
			children[i] = pid;
			ins[i] = comm_main[READ];
			close(comm_main[WRITE]);	
			//serve(comm_main[READ]);
		}
	}

	serve();
	return 0;
}
