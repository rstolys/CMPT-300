TARGET = myalloc
OBJS = main.o myalloc.o list.o

CFLAGS = -Wall -g -std=c99 -D_POSIX_C_SOURCE=199309L -lrt
CC = gcc -pthread

all: clean $(TARGET) #test

%.o : %.c
	$(CC) -c $(CFLAGS) $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# run program
test: myalloc
	./myalloc

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
