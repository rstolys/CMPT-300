## This is a simple Makefile with lots of comments 
## Check Unix Programming Tools handout for more info.

# Define what compiler to use and the flags.
CC=cc
CXX=CC
#CCFLAGS= -g -std=c99 -Wall -Werror
CCFLAGS= -g -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Werror

all: shell test

# Compile all .c files into .o files
# % matches all (like * in a command)
# $< is the source file (.c file)
%.o : %.c
	$(CC) -c $(CCFLAGS) $<


shell: shell.o 
	$(CC) -o shell shell.o $(CCFLAGS)

test: shell
	./shell

clean:
	rm -f core *.o shell
