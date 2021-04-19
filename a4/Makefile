ARRAY_TARGET = array_stats_test
ARRAY_FILE = test/array_stats_test.c
PROC_TARGET = process_ancestors_test
PROC_FILE = test/process_ancestors_test.c
OBJS = array_stats_test.o process_ancestors_test.o

CFLAGS = -std=c99 -D _GNU_SOURCE -static
CC = gcc 

all: clean astats proc_ancestors

astats: 
	$(CC) $(CFLAGS) $(ARRAY_FILE) -o $(ARRAY_TARGET)

proc_ancestors:
	$(CC) $(CFLAGS) $(PROC_FILE) -o $(PROC_TARGET)

transfer:
	scp -P 10022 $(ARRAY_TARGET) $(PROC_TARGET) ubuntu@localhost:

clean:
	rm -f $(ARRAY_TARGET)
	rm -f $(PROC_TARGET)
	rm -f $(OBJS)
