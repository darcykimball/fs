CC = gcc
CCFLAGS = -std=c99 -Wall -g

fs_shell: fs_shell.o  shell.o fscommands.o fstree.o filesystem.o fsfile.o
	$(CC) -o fs_shell fs_shell.o shell.o fscommands.o fstree.o filesystem.o \
					fsfile.o

test_shell: test_shell.o shell.o
	$(CC) -o test_shell test_shell.o shell.o

test_shell.o: test_shell.c shell.o
	$(CC) $(CCFLAGS) -c test_shell.c

fs_shell.o: fs_shell.c
	$(CC) $(CCFLAGS) -c fs_shell.c

shell.o: shell.h shell.c
	$(CC) $(CCFLAGS) -c shell.c

fstree.o: fstree.h fstree.c
	$(CC) $(CCFLAGS) -c fstree.c

filesystem.o: filesystem.h filesystem.c
	$(CC) $(CCFLAGS) -c filesystem.c

fscommands.o: fscommands.h fscommands.c
	$(CC) $(CCFLAGS) -c fscommands.c

fsfile.o: fsfile.h fsfile.c
	$(CC) $(CCFLAGS) -c fsfile.c

all: test_shell fs_shell

clean:
	rm -rf *.o test_shell

.PHONY: all clean
