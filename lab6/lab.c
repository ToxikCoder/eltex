
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>

#define NUM_WORKERS 4

int performance = 4;

void work()
{
	srand((unsigned)getpid());
	pid_t  cur_pid = getpid();
	int count = 0;
	float sleeptime = 0.0f;
	int capacity = rand() % 100;
	printf("Worker #%d is going to mine...\n", cur_pid);
	fflush(stdout);
	while(true)
	{
		if(capacity <= 0)
		{
			printf("Nothing to mine, going home\nWorker #%d visited mine %d times in %f seconds\n", cur_pid, count, sleeptime);
			fflush(stdout);
			exit(0);
		}
		++count;
		capacity -= performance;
		sleeptime += rand() % 5 - 1;
		sleep((unsigned)1);
	}
}

int main()
{
	srand((unsigned)getpid());
	pid_t pid;
	printf("Number of workers: %d\nSending them to mine...\n",  NUM_WORKERS);
	for(int i = 0; i < NUM_WORKERS; ++i)
	{
		fflush(stdout);
		pid = fork();
		if(pid == 0) work();
	}
	int* status = NULL;
	while(true)
	{
		pid = wait(status);
		if (pid > 0)
			printf("Base: worker #%d finished working\n", pid);
		else
			break;
	}
	printf("All workers finished working\n");
	return 0;
}
