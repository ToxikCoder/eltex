//
// Created by user on 13.11.16.
//

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

long (*power)(short num, int pow);

int main(void)
{
	short number = 0;
	unsigned int pow = 0;
	long result = 0;
	printf("Enter number and power: ");
	scanf("%hd %u", &number, &pow);

	//opening dynamic library
	void* handle = dlopen("./libpowerdyn.so", RTLD_LAZY);
	if(!handle)
	{
		fprintf(stderr, "dlopen() %s\n", dlerror());
		exit(-1);
	}
	//fetching function's address
	power = dlsym(handle, "power");
	//calling function
	result = (*power)(number, pow);
	dlclose(handle);

	printf("%hd ^ %u = %ld\n", number, pow, result);

	return 0;
}
