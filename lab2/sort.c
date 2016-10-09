#include "sort.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

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

int sort(char* string, int size, enum SORT_ORDER order)
{
	return -1;
}

int main()
{
	char string[MAXLEN];
	int result = inp_str(string, MAXLEN);
	if(result <= 0)
		printf("Error\n");
	else
		printf("%d symbols read: %s\n", result, string);

	return 0;
}
