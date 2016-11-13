//
// Created by user on 13.11.16.
//
#include <stdio.h>

extern long power(short, unsigned int);

int main(void)
{
	short number = 0;
	unsigned int pow = 0;
	printf("Enter number and power: ");
	scanf("%hd %u", &number, &pow);
	printf("%hd ^ %u = %ld\n", number, pow, power(number, pow));
	return 0;
}
