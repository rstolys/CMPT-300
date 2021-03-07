## This is a simple Makefile with lots of comments 
## Check Unix Programming Tools handout for more info.

# Define what compiler to use and the flags.
CC=cc -pthread
CXX=CC
#CCFLAGS= -g -std=c99 -Wall -Werror -lrt
CCFLAGS= -g -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Werror -lrt

all: candykids test

# Compile all .c files into .o files
# % matches all (like * in a command)
# $< is the source file (.c file)
%.o : %.c
	$(CC) -c $(CCFLAGS) $<


# Build candykids if necessary
candykids: bbuff.o stats.o candykids.o
	$(CC) -o candykids bbuff.o stats.o candykids.o

# run program
test: candykids
	./candykids 3 1 3

clean:
	rm -f core *.o candykids
