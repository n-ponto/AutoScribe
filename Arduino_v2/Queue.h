#ifdef __cplusplus
extern "C"
{
#endif

#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct {
    // Contants: should be read only
    unsigned char *buffer;  // Pointer to the start of the buffer
    unsigned int   bufSz;   // Size of the buffer provided in bytes
    unsigned int   memSz;   // Size of the memory type
    // Hold the state of the queue
    unsigned int head;      // Index of the next element in the queue
    unsigned int tail;      // Index of the next empty space in the queue
    unsigned int curSz;     // Current size of the queue (count of elements)
} Queue;

void queueInit(Queue *q, unsigned char *buffer, unsigned int memSz, unsigned int bufSz);
void dequeue(Queue *q, void* out);
void enqueue(Queue *q, void* in);
char isEmpty(Queue *q);
char isFull(Queue *q);

#endif //QUEUE_H_

#ifdef __cplusplus
}
#endif