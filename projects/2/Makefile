all: part1 iobound cpubound

part1.o:
	gcc -c part1.c

part1: part1.o
	gcc -o part1 part1.o

iobound: iobound.c
	gcc iobound.c -o iobound

cpubound: cpubound.c
	gcc cpubound.c -o cpubound

clean:
	rm -f *.o