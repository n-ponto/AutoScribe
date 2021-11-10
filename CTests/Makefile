CFLAGS = -Wall -O0 -g
CC = g++

OBJS_DIR = ./objs
ARD_DIR = ../Arduino_v2

all: queue
	./queue.exe

queue: QueueTests.o Queue.o
	${CC} ${CFLAGS} $(OBJS_DIR)/Queue.o $(OBJS_DIR)/QueueTests.o -o queue

QueueTests.o: QueueTests.cpp 
	mkdir -p $(OBJS_DIR)
	${CC} ${CFLAGS} -c QueueTests.cpp -o $(OBJS_DIR)/$@

Queue.o: $(ARD_DIR)/Queue.c
	cc ${CFLAGS} -c $(ARD_DIR)/Queue.c -o $(OBJS_DIR)/Queue.o

.PHONY: clean
clean:
	rm -r -f *.o *.exe $(OBJS_DIR)