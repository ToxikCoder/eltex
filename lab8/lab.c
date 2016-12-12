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

#define WORKERS 4
#define PERFORMANCE 4
#define IN 0
#define OUT 1

void work(int msgid_in, int msgid_out);
void pe(int r);

struct msgbuf
{
	long mtype;
	char mtext[1];
};

void work(int msgid_in, int msgid_out)
{
	int count = 0; //счётчик посещений шахты
	int sleeptime = 0; //общее время сна
	pid_t pid = getpid();
	//структура для отправки факта выполнения работы
	struct msgbuf send;
	send.mtype = pid;
	send.mtext[0] = 1;
	//структура для получения оставшегося объёма
	struct msgbuf rec;
	int length = sizeof(struct msgbuf);
	fprintf(stdout, "Worker #%d is starting to mine...\n", pid);
	fflush(stdout);

	while(true)
	{
		//получаем объём шахты
		int res = msgrcv(msgid_in, &rec, length, 1, 0);
		pe(res);
		//если шахта не пуста
		if(rec.mtext[0] > 0)
		{
			//пишем факт работы
			res = msgsnd(msgid_out, &send, length, 0);
			pe(res);
			printf("#%d worked\n", pid);
			fflush(stdout);
			++count;
			//спим
			int sl = rand() % 3;
			sleeptime += sl;
			sleep((unsigned) sleeptime);
		}
		else
		{
			fprintf(stdout, "Nothing to mine here. Worker #%d stats:\nVisits: %d, Sleep time: %d", pid, count, sleeptime);
			fflush(stdout);
			exit(0);
		}
	}
}

void pe(int r)
{
	if(r == -1)
	{
		perror("Error: ");
		fflush(stdout);
		exit(-1);
	}
}

int main(void)
{
	int workers[4];
	int capacity = 100;
	//генерируем ключи для очередей сообщений
	key_t keys[2];
	keys[IN] = ftok("/dev/null", '!');
	keys[OUT] = ftok("/dev/null", '%');

	//создаём очередь сообщений
	int mid_in = msgget(keys[IN], IPC_CREAT | 0600);
	int mid_out = msgget(keys[OUT], IPC_CREAT | 0600);
	//печатаем ошибку и выходим, если произошла ошибка чтения из очереди
	pe(mid_in);
	pe(mid_out);

	for(short i = 0; i < WORKERS; ++i)
	{
		fflush(stdout);
		int pid = fork();
		if(pid == 0)
			work(mid_out, mid_in);
		else
			workers[i] = pid;
	}

	int length = sizeof(struct msgbuf);
	//пишем в очередь объём шахты
	//считываем факт работы
	
	//структура для отправки объёма шахты
	struct msgbuf send;
	send.mtype = 1;
	send.mtext[0] = (char) capacity;
	//структура для принятия сообщений
	struct msgbuf rec;
		
	while(capacity > 0)
	{	
		//отправляем объём
		int res = msgsnd(mid_out, &send, length, 0);
		pe(res);
		res = msgsnd(mid_out, &send, length, 0);
		pe(res);
		res = msgsnd(mid_out, &send, length, 0);
		pe(res);
		res = msgsnd(mid_out, &send, length, 0);
		pe(res);
		printf("Messages sent\n");
		fflush(stdout);
		sleep(1);
		//читаем факт добычи золота
		for(short i = 0; i < WORKERS; ++i)
		{
			res = msgrcv(mid_in, &rec, length, workers[i], 0);
			pe(res);
			fprintf(stdout, "%d\n", rec.mtext[0]);
			fflush(stdout);
			//1-работник произвёл работу, вычитаем из оставшегося объёма производительность
			if(rec.mtext[0] == 1)
			{
				capacity -= PERFORMANCE;
				send.mtext[0] = capacity;
				fprintf(stdout, "Capacity: %d", capacity);
				fflush(stdout); 
				res = msgsnd(mid_out, &send, length, 0);
				pe(res);
				res = msgsnd(mid_out, &send, length, 0);
				pe(res);
				res = msgsnd(mid_out, &send, length, 0);
				pe(res);
				res = msgsnd(mid_out, &send, length, 0);
				pe(res);
			}
		}
	}
	//в шахте больше ничего нет
	send.mtext[0] = 0;
	msgsnd(mid_out, &send, 1, 0);
	msgsnd(mid_out, &send, 1, 0);
	msgsnd(mid_out, &send, 1, 0);
	msgsnd(mid_out, &send, 1, 0);
	//теперь работники должны завершить работу
	int* status = NULL;
	while(true)
	{
		pid_t pid = wait(status);
		if(pid > 0)
			printf("Base: Worker#%d returned from mine\n", pid);
		else
			break;
	}
	
	msgctl(mid_in, IPC_RMID, NULL);
	msgctl(mid_out, IPC_RMID, NULL);
	printf("All workers are at base");
	return 0;
}
