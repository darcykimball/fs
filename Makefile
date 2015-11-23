CC = gcc
CCFLAGS = -Wall -std=c99

test_shell: test_shell.o shell.o
	$(CC) -o test_shell test_shell.o shell.o

test_shell.o: test_shell.c shell.o
	$(CC) $(CCFLAGS) -c test_shell.c

shell.o: shell.h shell.c
	$(CC) $(CCFLAGS) -c shell.c

all: test_shell

clean:
	rm -rf *.o test_shell

.PHONY: all clean
