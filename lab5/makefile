FLAGS=-g -Wall -std=c99 -O2 -pedantic
static: static.c libpower.a
	gcc static.c -o static $(FLAGS) -L. -lpower
libpower.a: power.c
	gcc -c power.c $(FLAGS)
	ar rc libpower.a power.o
	ranlib libpower.a