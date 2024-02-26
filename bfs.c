#include "SyncGraph/graph.h"
#include "ThreadPool/ThreadPool.h"

typedef struct {
    node *start;
    node *end;
} Queue;

typedef struct {
    Graph *graph;
    vertex v;
    int *m;
    ThreadPool *q;
} Args;

typedef struct {
    Graph *graph;
    vertex origin;
    vertex v;
    int *m;
    int *visitid;
    ThreadPool *q;
} BFSArgs;


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

BFSArgs* initBFSArgs(Graph* graph, vertex origin, vertex v, int* m, int* visited, ThreadPool* pool) {
    BFSArgs *args = malloc(sizeof(BFSArgs));
    args->graph = graph;
    args->origin = origin;
    args->v = v;
    args->m = m;
    args->visitid = visited;
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

void bfsVisit(void *args) {
    BFSArgs *data = (BFSArgs *) args;
    Graph *graph = data->graph;
    vertex origin = data->origin;
    vertex v = data->v;
    int* m = data->m;
    int *visited = data->visitid;
    ThreadPool *q = data->q;
    free(data);

    BFSArgs *newArgs;
    TaskQueue in_q;
    initQueue(&in_q);
    ThreadPool pool = {2, 0, &in_q};

    node* neighbors = graph->adjacencyLists[v];
    while (neighbors) {
        vertex neighbor = neighbors->v;
        if (!visited[neighbor]) {
            m[neighbor] = m[v] + 1;
            visited[v] = 1;

            newArgs = initBFSArgs(graph, v, neighbor, m, visited, &pool);
            TaskData td = {bfsVisit, (void *)newArgs};
            insert(pool.q, td);
        }

        neighbors = neighbors->next;
    }
    runThreadPool(&pool);
    --(q->runningThreads);
}

void parallelBFS(Graph *graph, int *m, vertex v) {
    // Args *data = (Args *) args;
    // Graph *graph = data->graph;
    // vertex v = data->v;
    // int* m = data->m;
    // ThreadPool *q = data->q;
    // free(data);

    int* visited = (int*)calloc(graph->numVertices, sizeof(int));
    Queue queue = {NULL, NULL};
    // addItem(&queue, v);
    m[v] = 0;
    visited[v] = 1;

    BFSArgs *newArgs;
    TaskQueue in_q;
    initQueue(&in_q);
    ThreadPool pool = {2, 0, &in_q};

    node* neighbors = graph->adjacencyLists[v];
    while (neighbors) {
        vertex neighbor = neighbors->v;
        m[neighbor] = m[v] + 1;
        visited[v] = 1;

        newArgs = initBFSArgs(graph, v, neighbor, m, visited, &pool);
        TaskData td = {bfsVisit, (void *)newArgs};
        insert(pool.q, td);

        neighbors = neighbors->next;
    }

    runThreadPool(&pool);
    free(visited);
    // --(q->runningThreads);
}

void bfs(Graph *graph, int **m) {
    for (vertex v = 0; v < graph->numVertices; v++) {
        parallelBFS(graph, m[v], v);    
    }
}

// void bfs(Graph *graph, int **m) {
//     int numVertices = graph->numVertices;
//     Args *args;
//     TaskQueue q;
//     initQueue(&q);
//     ThreadPool pool = {2, 0, &q};

//     for (vertex v = 0; v < graph->numVertices; v++) {
//         args = initArgs(graph, v, m[v], &pool);

//         TaskData td = {parallelBFS, (void *)args};
//         insert(pool.q, td);
        
//         // distanceFromV(graph, m[v], v);    
//     }
//     runThreadPool(&pool);
// }

