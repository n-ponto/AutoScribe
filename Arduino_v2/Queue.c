/*
Queue buffer implementation used by the Manual Control and Drawing modes
- circular FIFO design
- used to store instructions for drawing mode
*/

typedef struct queue {
    unsigned char *buffer;  // Pointer to the start of the buffer
    unsigned int front;     // Index of the next element in the queue
    unsigned int end;       // Index of the next empty space in the queue
    unsigned int sz;        // Size of the queue (count of elements)
} Queue;

void
dequeue(Queue *q, void* out)
{

}

void
queue(Queue *q, void* in)
{

}

void
isEmpty(Queue *q)
{

}

void
isFull(Queue *q)
{

}