#include "SyncGraph/graph.h"
#include "ThreadPool/ThreadPool.h"
#include "MyQueue/Queue.h"

Args* initArgs(Graph* graph, vertex v, int* m, ThreadPool* pool) {
    Args *args = malloc(sizeof(Args));
    args->graph = graph;
    args->v = v;
    args->m = m;
    args->q = pool;
    return args;
}

BFSArgs* initBFSArgs(Graph* graph, Queue* levelK, vertex v, int* m, int* visited, ThreadPool* pool) {
    BFSArgs *args = malloc(sizeof(BFSArgs));
    args->graph = graph;
    args->v = v;
    args->m = m;
    args->levelK = levelK;
    args->visitid = visited;
    args->q = pool;
    return args;
}

SetArgs* initI(vertex v, int* m, ThreadPool *q) {
    SetArgs *args = malloc(sizeof(SetArgs));
    args->v = v;
    args->m = m;
    args->q = q;
    return args;
}

void setInfinety(void *args) {
    SetArgs *data = (SetArgs *) args;
    vertex v = data->v;
    int* m = data->m;
    ThreadPool *q = data->q;
    free(data);
    m[v] = -1;
    --(q->runningThreads);
}

void expandVertex(void *args) {
    BFSArgs *data = (BFSArgs *) args;
    Graph *graph = data->graph;
    vertex v = data->v;
    int* m = data->m;
    int *visited = data->visitid;
    Queue *levelK = data->levelK;
    ThreadPool *q = data->q;
    free(data);

    node* neighbors = graph->adjacencyLists[v];
    while (neighbors) {
        vertex neighbor = neighbors->v;

        pthread_mutex_lock(&graph->num_visits_mutexes[neighbor]);
        if (!visited[neighbor]) {
            m[neighbor] = m[v] + 1;
            addItem(levelK, neighbor);
            visited[neighbor] = 1;
        }
        pthread_mutex_unlock(&graph->num_visits_mutexes[neighbor]);
        neighbors = neighbors->next;
    }

    --(q->runningThreads);
}

void parallelBFS(void *args) {
    Args *data = (Args *) args;
    Graph *graph = data->graph;
    vertex v = data->v;
    int* m = data->m;
    ThreadPool *q = data->q;
    free(data);

    TaskQueue in_q;
    initQueue(&in_q);
    ThreadPool pool = {2, 0, &in_q};

    int* visited = (int*)calloc(graph->numVertices, sizeof(int));
    for (int i = 0; i < graph->numVertices; i++) {
        SetArgs *newArgs = initI(i, m, &pool);
        TaskData td = {setInfinety, (void *)newArgs};
        insert(pool.q, td);
    }
    runThreadPool(&pool);
    
    m[v] = 0;
    visited[v] = 1;

    Queue levelK = {NULL, NULL};
    initMyQueue(&levelK);
    addItem(&levelK, v);

    vertex u;
    while (!isQueueEmpty(&levelK)) {

        while ((u = popItem(&levelK)) != -1) {
            BFSArgs *newArgs = initBFSArgs(graph, &levelK, u, m, visited, &pool);
            TaskData td = {expandVertex, (void *)newArgs};
            insert(pool.q, td);
        }

        runThreadPool(&pool);
    }

    free(visited);
    --(q->runningThreads);
}

void bfs(Graph *graph, int **m) {
    int numVertices = graph->numVertices;
    TaskQueue q;
    initQueue(&q);
    ThreadPool pool = {2, 0, &q};

    for (vertex v = 0; v < graph->numVertices; v++) {
        Args *args = initArgs(graph, v, m[v], &pool);

        TaskData td = {parallelBFS, (void *)args};
        insert(pool.q, td);
    }
    runThreadPool(&pool);
}

