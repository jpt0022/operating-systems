vm_sim: vmm.o address.o
	gcc vmm.o address.o -o vm_sim

vmm.o: vmm.c
	gcc -c vmm.c

address.o: address.c address.h

clean:
	rm *.o vm_sim
