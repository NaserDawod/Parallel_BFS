
#ifndef QUEUE
#define QUEUE

#include "../SyncGraph/graph.h"
#include "../ThreadPool/ThreadPool.h"

typedef struct {
    node *start;
    node *end;
    pthread_mutex_t mutex;
} Queue;

typedef struct {
    Graph *graph;
    vertex v;
    int *m;
    ThreadPool *q;
} Args;

typedef struct {
    Graph *graph;
    vertex v;
    int *m;
    int *visitid;
    Queue *levelK;
    ThreadPool *q;
} BFSArgs;

void addItem(Queue *queue, vertex u);

vertex popItem(Queue *queue);

void initMyQueue(Queue* queue);

int isQueueEmpty(Queue* queue);

#endif