#include <stdio.h>
#include <stdlib.h>

struct node{
    int vertex;
    struct node* next;
};

struct Graph{
    int numVertices;
    struct node** adjLists;
};

struct node* createNode(int v);
struct Graph* createAGraph(int vertices);
void addEdge(struct Graph* graph, int s, int d);
void printGraph(struct Graph* graph);