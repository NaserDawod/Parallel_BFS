#include "SyncGraph/graph.h"
#include "ThreadPool/ThreadPool.h"

typedef struct {
    node *start;
    node *end;
} Queue;

struct Args {
    Graph *graph;
    vertex v;
    int *m;
    ThreadPool *q;
};

typedef struct Args Args;

void addItem(Queue *queue, vertex u) {
    node *item = createNode(u);
    if (queue->start == NULL) {
        queue->start = queue->end = item;
    } else {
        queue->end->next = item;
        queue->end = item;
    }
}

vertex popItem(Queue *queue) {
    if (!queue->start) {
        return -1;
    }
    node *poped = queue->start;
    queue->start = queue->start->next;
    queue->end = queue->start ? queue->end:queue->start;
    vertex v = poped->v;
    free(poped);
    return v;
}

Args* initArgs(Graph* graph, vertex v, int* m, ThreadPool* pool) {
    Args *args = malloc(sizeof(Args));
    args->graph = graph;
    args->v = v;
    args->m = m;
    args->q = pool;
    return args;
}

void distanceFromV(void *args) {
    Args *data = (Args *) args;
    Graph *graph = data->graph;
    vertex v = data->v;
    int* m = data->m;
    ThreadPool *q = data->q;
    free(data);
    printf("this is thread: %d\n", v);

    int* visited = (int*)calloc(graph->numVertices, sizeof(int));
    Queue queue = {NULL, NULL};
    addItem(&queue, v);
    m[v] = 0;
    visited[v] = 1;

    vertex u;
    while ((u = popItem(&queue)) != -1) {
        node* neighbors = graph->adjacencyLists[u];

        while (neighbors != NULL) {
            vertex neighbor = neighbors->v;
            if (!visited[neighbor]) {
                m[neighbor] = m[u] + 1;
                addItem(&queue, neighbor);
                visited[neighbor] = 1;
            }

            neighbors = neighbors->next;
        }
    }

    free(visited);
    --(q->runningThreads);
}

void bfs(Graph *graph, int **m) {
    int numVertices = graph->numVertices;
    Args *args;
    TaskQueue q;
    initQueue(&q);
    ThreadPool pool = {4, 0, &q};

    for (vertex v = 0; v < graph->numVertices; v++) {
        args = initArgs(graph, v, m[v], &pool);

        TaskData td = {distanceFromV, (void *)args};
        insert(pool.q, td);
        
        // distanceFromV(graph, m[v], v);    
    }
    runThreadPool(&pool);
}

