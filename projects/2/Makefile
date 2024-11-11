all: part1 part2 part3 part4 part5 iobound cpubound

part1.o:
	gcc -c part1.c

part1: part1.o
	gcc -o part1 part1.o

part2.o:
	gcc -c part2.c

part2: part2.o
	gcc -o part2 part2.o

part3.o:
	gcc -c part3.c

part3: part3.o
	gcc -o part3 part3.o

part4.o:
	gcc -c part4.c

part4: part4.o
	gcc -o part4 part4.o

part5.o:
	gcc -c part5.c

part5: part5.o
	gcc -o part5 part5.o

iobound: iobound.c
	gcc iobound.c -o iobound

cpubound: cpubound.c
	gcc cpubound.c -o cpubound

clean:
	rm -f *.o part1 part2 part3 part4 part5