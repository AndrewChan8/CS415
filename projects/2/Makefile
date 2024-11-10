all: part1 part2 part3 iobound cpubound

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

iobound: iobound.c
	gcc iobound.c -o iobound

cpubound: cpubound.c
	gcc cpubound.c -o cpubound

clean:
	rm -f *.o