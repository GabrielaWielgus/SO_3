all: producent konsument

konsument: konsument.c sem.o
	gcc -o konsument konsument.c

producent: producent.c sem.o
	gcc -o producent producent.c

sem.o: sem.c
	gcc -c sem.c

clean:
	rm -rf *~
