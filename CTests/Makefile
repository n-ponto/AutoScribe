CFLAGS = -Wall -O0 -g
CC = g++

# File path to Arduino files
ARD_DIR = ../Arduino_v2
# Objects for testing utils
UTIL_OBJS = TestingUtils.o

all: queue draw

######################### QUEUE ############################
QUEUE_TEST_OBJS = QueueTests.o
QUEUE_ARD_OBJ = Queue.o
QUEUE_OBJS = $(QUEUE_TEST_OBJS) $(QUEUE_ARD_OBJ) $(UTIL_OBJS)

queue: $(QUEUE_OBJS)
	${CC} ${CFLAGS} $^ -o $@
	./queue.exe

QueueTests.o: QueueTests.cpp
	${CC} ${CFLAGS} -c QueueTests.cpp -o $@

$(QUEUE_ARD_OBJ): $(ARD_DIR)/Queue.c
	cc ${CFLAGS} -c $(ARD_DIR)/Queue.c -o Queue.o

######################### DRAWING ############################
DRAW_TEST_OBJS = DrawingTests.o DrawingMocks.o
DRAW_ARD_OBJ = Drawing.o
DRAW_OBJS = $(DRAW_TEST_OBJS) $(DRAW_ARD_OBJ) $(QUEUE_ARD_OBJ) $(UTIL_OBJS)

draw: $(DRAW_OBJS)
	${CC} ${CFLAGS} $^ -o $@
	./draw.exe

# Arduino Drawing.cpp file
$(DRAW_ARD_OBJ): $(ARD_DIR)/Drawing.cpp
	${CC} ${CFLAGS} -c $< -o $@ -DTESTING_MOCK

######################### GENERAL ############################

# For testing files from this directory
%.o: %.cpp
	${CC} ${CFLAGS} -c $< -o $@

.PHONY: clean
clean:
	rm -r -f *.o *.exe $(OBJS_DIR)