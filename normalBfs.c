// #include "SyncGraph/graph.h"
// #include "ThreadPool/ThreadPool.h"

// typedef struct {
//     node *start;
//     node *end;
// } Queue;

// struct dfsArgs {
//     Graph *graph;
//     vertex v;
//     int *m;
//     ThreadPool *q;
// };

// typedef struct dfsArgs dfsArgs;

// void addItem(Queue *queue, vertex u) {
//     node *item = createNode(u);
//     if (queue->start == NULL) {
//         queue->start = queue->end = item;
//     } else {
//         queue->end->next = item;
//         queue->end = item;
//     }
// }

// vertex popItem(Queue *queue) {
//     if (!queue->start) {
//         return -1;
//     }
//     node *poped = queue->start;
//     queue->start = queue->start->next;
//     queue->end = queue->start ? queue->end:queue->start;
//     vertex v = poped->v;
//     free(poped);
//     return v;
// }


// void distanceFromV(Graph *graph, int *m, vertex v) {
//     int* visited = (int*)calloc(graph->numVertices, sizeof(int));
//     Queue queue = {NULL, NULL};
//     addItem(&queue, v);
//     m[v] = 0;
//     visited[v] = 1;

//     vertex u;
//     while ((u = popItem(&queue)) != -1) {
//         node* neighbors = graph->adjacencyLists[u];

//         while (neighbors != NULL) {
//             vertex neighbor = neighbors->v;

//             if (!visited[neighbor]) {
//                 m[neighbor] = m[u] + 1;
//                 addItem(&queue, neighbor);
//                 visited[neighbor] = 1;
//             }

//             neighbors = neighbors->next;
//         }
//     }
// }


// void bfs(Graph *graph, int **m) {
//     for (vertex v = 0; v < graph->numVertices; v++) {
//         distanceFromV(graph, m[v], v);    
//     }
// }

