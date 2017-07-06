#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "headers.h"

struct WORKER** db;

/*
 * allocates memory for array of pointers to struct
 * @return int - error code
 */
int init()
{
	db = (struct WORKER**) malloc(sizeof(struct WORKER*) * DBSIZE);
	if(db == NULL)
	{
		fprintf(stderr, "Error while allocating space for database");
		return -1;
	}

	for(int i = 0; i < DBSIZE; ++i)
	{
		db[i] = (struct WORKER*) malloc(sizeof(struct WORKER));
		if(db[i] == NULL)
		{
			fprintf(stderr, "Error while allocating space for database strings");
			return -1;
		}
		db[i]->lastName = (char*) malloc(sizeof(char) * LASTNAMELENGTH);
	}	
	return 0;
}

void release()
{
	for(int i = 0; i < DBSIZE; ++i)
	{
		free(db[i]);
	}
	free(db);
}

void bubbleSort(struct WORKER** db, int size)
{
	struct WORKER* p = NULL;
	for(int i = 0; i < size - 1; ++i)
	{
		bool swapped = false;
		for(int j = 0; j < size - i - 1; ++j)
		{
			if(db[j]->wage < db[j+1]->wage)
			{
				p = db[j];
				db[j] = db[j+1];
				db[j+1] = p;
				swapped = true;
			}
		}
		if(!swapped)
			break;
	}
}

/*
 * prints every string available in the "database"
 * @return void
 */
void PrintDBContent()
{
	printf("|%15s|%17s|%15s|%15s|\n", "Last name", "Department №", "Year of birth", "Annual wage, $");
	printf("|---------------------------------------------------------------|\n");
	for(int i = 0; i < DBSIZE; ++i)
		printf("|%15s|%15d|%15d|%15lu|\n", db[i]->lastName, db[i]->departmentNumber, db[i]->birthYear, db[i]->wage);
}

int main()
{
	if(init() == -1)
		exit(-1);
	
	for(int i = 0; i < DBSIZE; ++i)
	{
		struct WORKER* p = db[i];
		printf("Enter values below in format LAST_NAME DEPARTMENT_NUMBER BIRTH_YEAR ANNUAL_WAGE\n");
		scanf("%s %hu %d %lu", p->lastName, &p->departmentNumber, &p->birthYear, &p->wage);
		db[i] = p;
	}
	PrintDBContent();
	bubbleSort(db, DBSIZE);
	printf("Sorting...\n");
	PrintDBContent();
	release();
	return 0;
}
