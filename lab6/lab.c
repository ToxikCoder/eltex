
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_WORKERS 4

int capacity = 100;
int performance = 4;

void work()
{
	pid_t  cur_pid = getpid();
	int count = 0;
	printf("Worker #%d is going to mine...\n", cur_pid);
	while(true)
	{
		if(capacity <= 0)
		{
			printf("Nothing to mine, going home\nWorker #%d visited mine %d times\n", cur_pid, count);
			exit(0);
		}
		++count;
		capacity -= performance;
		sleep((unsigned)rand() % 5);
	}
}

int main()
{
	srand((unsigned)getpid());
	pid_t pid;
	printf("Mine capacity: %d\nNumber of workers: %d\nSending them to mine...\n", capacity, NUM_WORKERS);
	for(int i = 0; i < NUM_WORKERS; ++i)
	{
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
