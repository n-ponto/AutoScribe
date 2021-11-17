#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <cstring>

#include "TestingUtils.h"
#include "../Arduino_v2/Queue.h"

void queueSingle()
{
    // Create a new queue
    unsigned int size = 100;
    unsigned char buffer[size];
    Queue q; 
    queueInit(&q, buffer, 1, size);
    assert(isEmpty(&q), "empty at start");
    assert(!isFull(&q), "not full at start");
    
    // Add an element
    char expected = 0xFD;
    enqueue(&q, &expected);
    assert(!isEmpty(&q), "not empty after enqueue");
    assert(!isFull(&q), "not full after enqueue");

    // Remove the element
    char actual;
    dequeue(&q, &actual);
    assert(isEmpty(&q), "empty after dequeue");
    assert(!isFull(&q), "not full after dequeue");
    assert(actual == expected, "actual matches expected");
}

void queueMultiple()
{
    // Create a new queue
    unsigned int size = 20;
    unsigned char buffer[size];
    Queue q; 
    queueInit(&q, buffer, 1, size);
    assert(isEmpty(&q), "empty at start");
    assert(!isFull(&q), "not full at start");

    // Add a couple of items
    for (char i = 0; i < 15; i++)
    {
        enqueue(&q, &i);
    }
    assert(!isEmpty(&q), "not empty after enqueue");
    assert(!isFull(&q), "not full after enqueue");

    char actual;
    for (char i = 0; i < 15; i++)
    {
        dequeue(&q, &actual);
        assert(actual == i, "actual doesn't match expected");
    }
    assert(isEmpty(&q), "empty after dequeue");
    assert(!isFull(&q), "not full after dequeue");
}

void fillQueue()
{
    // Create a new queue
    unsigned char size = 20;
    unsigned char buffer[size];
    Queue q; 
    queueInit(&q, buffer, 1, size);
    assert(isEmpty(&q), "empty at start");
    assert(!isFull(&q), "not full at start");

    // Add a couple of items
    for (char i = 0; i < size; i++)
    {
        enqueue(&q, &i);
    }
    assert(!isEmpty(&q), "not empty after enqueue");
    assert(isFull(&q), "full after enqueue");

    char actual;
    for (char i = 0; i < size; i++)
    {
        dequeue(&q, &actual);
        assert(actual == i, "actual doesn't match expected");
    }
    assert(isEmpty(&q), "empty after dequeue");
    assert(!isFull(&q), "not full after dequeue");
}

void efef()
{
    // Create a new queue
    unsigned char size = 20;
    unsigned char buffer[size];
    Queue q; 
    queueInit(&q, buffer, 1, size);
    assert(isEmpty(&q), "empty at start");
    assert(!isFull(&q), "not full at start");

    // Fill the queue
    for (char i = 0; i < size; i++)
    {
        enqueue(&q, &i);
    }
    assert(!isEmpty(&q), "not empty after enqueue");
    assert(isFull(&q), "full after enqueue");

    // Empty the queue
    char actual;
    for (char i = 0; i < size; i++)
    {
        dequeue(&q, &actual);
        assert(actual == i, "actual doesn't match expected");
    }
    assert(isEmpty(&q), "empty after dequeue");
    assert(!isFull(&q), "not full after dequeue");

    // Fill the queue again
    for (char i = 0; i < size; i++)
    {
        enqueue(&q, &i);
    }
    assert(!isEmpty(&q), "not empty after enqueue");
    assert(isFull(&q), "full after enqueue");
}

void queueLongs()
{
    // Create a new queue
    unsigned char size = 20;
    unsigned char buffer[size*sizeof(long)];
    Queue q; 
    queueInit(&q, buffer, sizeof(long), size*sizeof(long));
    assert(isEmpty(&q), "empty at start");
    assert(!isFull(&q), "not full at start");

    // Fill the queue
    for (long i = 0; i < size; i++)
    {
        enqueue(&q, &i);
    }
    assert(!isEmpty(&q), "not empty after enqueue");
    assert(isFull(&q), "full after enqueue");

    // Empty the queue
    long actual;
    for (long i = 0; i < size; i++)
    {
        dequeue(&q, &actual);
        assert(actual == i, "actual doesn't match expected");
    }
    assert(isEmpty(&q), "empty after dequeue");
    assert(!isFull(&q), "not full after dequeue");

    // Fill the queue again
    for (long i = 0; i < size; i++)
    {
        enqueue(&q, &i);
    }
    assert(!isEmpty(&q), "not empty after enqueue");
    assert(isFull(&q), "full after enqueue");
}

void crazy()
{
    // Create a new queue
    unsigned char size = 20;
    unsigned char buffer[size];
    Queue q; 
    queueInit(&q, buffer, 1, size);
    assert(isEmpty(&q), "empty at start");
    assert(!isFull(&q), "not full at start");

    // Fill the queue
    for (char i = 0; i < size; i++)
    {
        enqueue(&q, &i);
    }
    assert(!isEmpty(&q), "not empty after first fill");
    assert(isFull(&q), "full after first fill");

    // Remove half of the elements
    char actual;
    for (char i = 0; i < 10; i++)
    {
        dequeue(&q, &actual);
        assert(actual == i, "first half doesn't match expected");
    }
    assert(!isEmpty(&q), "not empty after removing half");
    assert(!isFull(&q), "not full after removing half");

    // Add more elements
    char start = 234;
    for (char i = start; i < start+10; i++)
    {
        enqueue(&q, &i);
    }
    assert(!isEmpty(&q), "not empty after adding more");
    assert(isFull(&q), "full after adding more");

    // Remove everything
    for (char i = 10; i < 20; i++)
    {
        dequeue(&q, &actual);
        assert(actual == i, "second half doesn't match expected");
    }
    assert(!isEmpty(&q), "not empty after removing last of initial");
    assert(!isFull(&q), "not full after removing last of initial");
    for (char i = start; i < start+10; i++)
    {
        dequeue(&q, &actual);
        assert(actual == i, "weird values don't match");
    }
    assert(isEmpty(&q), "empty after removing everything");
    assert(!isFull(&q), "not full after dequeue");

    // Fill again
    for (char i = start; i < start+size; i++)
    {
        enqueue(&q, &i);
    }
    assert(!isEmpty(&q), "not empty after refilling");
    assert(isFull(&q), "full after refilling");
}

int
main(int argc, char *argv[])
{ 
    struct test tests[] = {
        {queueSingle,   "queueSingle"},
        {queueMultiple, "queueMultiple"},
        {fillQueue,     "fillQueue"},
        {efef,          "efef"},
        {queueLongs,    "queueLongs"},
        {crazy,         "crazy"},
        { 0, ""},
    };

    runTests("Queue", tests);
    
    exit(0);
}
