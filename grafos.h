#include <stdio.h>
#include <stdlib.h>

//Estrutura Nodo
struct node{
    int vertice;
    struct node* prox;
};

//Estrutura Grafo (num de vertices e lista de adjacencias)
struct Graph{
    int numVertices;
    struct node** adjLists;
};

struct node* criaNodo(int v);
struct Graph* criaGrafo(int vertices);
void adicionaAresta(struct Graph* graph, int s, int d);
void printGraph(struct Graph* graph);