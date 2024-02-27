#include "Queue.h"


void addItem(Queue *queue, vertex u) {
    node *item = createNode(u);
    pthread_mutex_lock(&queue->mutex);
    if (queue->start == NULL) {
        queue->start = queue->end = item;
    } else {
        queue->end->next = item;
        queue->end = item;
    }
    pthread_mutex_unlock(&queue->mutex);
}

vertex popItem(Queue *queue) {
    if (!queue->start) {
        return -1;
    }
    pthread_mutex_lock(&queue->mutex);
    node *poped = queue->start;
    queue->start = queue->start->next;
    queue->end = queue->start ? queue->end:queue->start;
    vertex v = poped->v;
    free(poped);
    pthread_mutex_unlock(&queue->mutex);
    return v;
}

void initMyQueue(Queue* queue) {
    queue->end = queue->start = NULL;

    if (pthread_mutex_init(&(queue->mutex), NULL) != 0) {
        perror("mutex init has failed.");
        exit(1);
    }
}

int isQueueEmpty(Queue* queue) {
    pthread_mutex_lock(&queue->mutex);
    int isEmpty = (queue->start == NULL);
    pthread_mutex_unlock(&queue->mutex);

    /* Return the result */
    return isEmpty;
}
