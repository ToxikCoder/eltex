#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#define NUM_WORKERS 4
#define PERFORMANCE 4
#define READ 0
#define WRITE 1

int capacity = 100;

void work(int in, int out);
void serve(int in, int out);

/* Соглашение
 * База пишет в pipe сколько осталось в шахте, читает единички,
 * вычитая PERFORMANCE на каждую единицу. 
 * Пишет в канал 0, если шахта пуста
 * Работник получает из канала объём шахты, пишет в канал
 * единицу на каждый цикл работы
 */

void work(int in, int out)
{
	pid_t cur_pid = getpid();
	srand((unsigned) cur_pid);
	int count = 0;
	int sleeptime = 0;
	printf("Worker #%d is going to mine...\n", cur_pid);
	fflush(stdout);
	int b = 1;
	int res = 0;
	while(true)
	{
		res = read(in, &capacity, sizeof(int));
		if(res == 0)
			exit(1);
		if(capacity >= 0)
		{
			res = write(out, &b, sizeof(int));
			++count;
			int sl = rand() % 3 - 1;
			sleeptime +=  sl;
			sleep((unsigned)sl);
		}
		else
		{
			printf("Going home. Worker #%d visited mine %d times and sleeped for %d seconds\n", cur_pid, count, sleeptime);
			fflush(stdout);
			exit(0);
		}
	}
}

void serve(int in, int out)
{
	int buf, res;
	int z = 0;
	while(true)
	{
		if(capacity <= 0)
		{
			res = write(out, &z, sizeof(int));
			exit(0);
		}

		res = write(out, &capacity, sizeof(int));

		if(res == -1)
			exit(-1);

		res = read(in, &buf, sizeof(int));
		if(res == 0)
			exit(1);
		if(buf == 1)
			capacity -= PERFORMANCE;
	}
}

int main(void)
{
	//нужно эти два повторить для каждого форка
	int comm_main[2];
	int comm_work[2];
	if((pipe(comm_main) && pipe(comm_work)) != 0)
		exit(-1);

	pid_t pid;
	for(short i = 0; i < NUM_WORKERS; ++i)
	{
		fflush(stdout);
		pid = fork();
		if(pid == 0)
		{
			close(comm_main[WRITE]);
			close(comm_work[READ]);
			work(comm_main[READ], comm_work[WRITE]);
		}
	}
	close(comm_main[READ]);
	close(comm_work[WRITE]);
	serve(comm_work[READ], comm_main[WRITE]);

	int* status = NULL;
	while(true)
	{
		pid = wait(status);
		if(pid > 0)
		{
			printf("Base: worker #%d finished working\n", pid);
			fflush(stdout);
		}
		else
			break;
	}
	printf("All workers finished working\n");
	return 0;
}
