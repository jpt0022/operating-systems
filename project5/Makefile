scheduler: scheduler.o node.o
	gcc scheduler.o node.o -o scheduler

scheduler.o: scheduler.c
	gcc -c scheduler.c

node.o: node.c node.h

clean:
	rm *.o scheduler
