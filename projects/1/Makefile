# all:
# 	gcc main.c string_parser.c command.c -o pseudo-shell

# clean:
# 	rm -f pseudo-shell

all: pseudo-shell

pseudo-shell: command.o main.o string_parser.o
	gcc -o pseudo-shell command.o main.o string_parser.o

command.o: command.c command.h
	gcc -c command.c

string_parser.o: string_parser.c string_parser.h
	gcc -c string_parser.c

main.o: main.c
	gcc -c main.c

clean:
	rm -f pseudo-shell *.o
