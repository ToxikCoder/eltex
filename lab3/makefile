FLAGS=-g -Wall -std=c11 -O2 
TARGET=-o lab

lab: lab.o
	gcc lab.o $(TARGET) $(FLAGS)
lab.o: lab.c headers.h
	gcc lab.c headers.h -c  $(FLAGS)
clean:
	rm lab lab.o headers.h.gch

