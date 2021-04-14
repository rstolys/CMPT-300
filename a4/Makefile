TARGET = testArrStat
OBJS = array_stats_usr.o

CFLAGS = -Wall -g -std=c99 -D_POSIX_C_SOURCE=199309L
CC = gcc

all: clean $(TARGET) #test

%.o : %.c
	$(CC) -c $(CFLAGS) $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# run program
test: testArrStat
	./testArrStat

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
