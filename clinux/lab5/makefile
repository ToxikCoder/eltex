FLAGS=-g -Wall -std=c99 -O2 -pedantic
static: static.c libpower.a
	gcc static.c -o static $(FLAGS) -L. -lpower
libpower.a: power.c
	gcc -c power.c $(FLAGS)
	ar rc libpower.a power.o
	ranlib libpower.a
dynamic: dynamic.c libpowerdyn.so
	gcc dynamic.c -o dynamic $(FLAGS) -ldl
libpowerdyn.so: power.o
	gcc -shared power.o -o libpowerdyn.so $(FLAGS)
power.o: power.c
	gcc -fPIC -c power.c $(FLAGS)

clean:
	@echo "Cleaning up..."
	@rm power.o
	@rm libpowerdyn.so
	@rm dynamic
	@rm libpower.a
	@rm static