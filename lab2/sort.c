#include "sort.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"

int inp_str(char* string, int maxlen)
{
	//won't read anything if maximum length of string to read is equal or less than 0
	if( maxlen <= 0) return -1;
	
	//if memory of string is not allocated
	if(string == NULL) 
		string = (char *) malloc(sizeof(char) * maxlen);

	printf("Enter string: ");
	fgets(string, maxlen, stdin);
	return strlen(string);
}

void out_str(char* string, int length, int number)
{
	
}

void swap(char* i, char* j)
{
	int temp = *i;
	*i = *j;
	*j = temp;
}

int sort(char* string, int size, enum SORT_ORDER order)
{
	int shuffles = 0;
	if(string == NULL || size <= 1) return -1;
	for(int i = 0; i < size - 2; ++i)
	{
		bool swapped = false;
		for(int j = 0; j < size - i - 2; ++j)
		{
			if(order == ASCENDING)
			{
				if(string[j] > string[j+1])
				{
					swap(&string[j], &string[j+1]);
					swapped = true;
					++shuffles;
				}
			}
			else
			{
				if(string[j] < string[j+1])
				{
					swap(&string[j], &string[j+1]);
					swapped = true;
					++shuffles;
				}
			}
		}
		if(!swapped)
			break;
	}
	return shuffles;
}

int main()
{
	char string[MAXLEN];
	int result = inp_str(string, MAXLEN);
	if(result > 0)
	{
		printf("%d symbols read: %s", result, string);
		result = sort(string, result, ASCENDING);
		if(result > 0)
		{
			//sorted successfully
			printf("%d reshuffles, resulting string: %s", result, string);

		}
	}

	return 0;
}
