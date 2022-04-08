#include "grafos.h"
#include <stdio.h>
#include <stdlib.h>

//Cria um nodo
struct node* criaNodo(int v) {
  struct node* novoNodo = malloc(sizeof(struct node));
  novoNodo->vertice = v;
  novoNodo->prox = NULL;
  return novoNodo;
}

//Cria o grafo
struct Graph* criaGrafo(int vertices) {
  struct Graph* graph = malloc(sizeof(struct Graph));
  graph->numVertices = vertices;

  graph->adjLists = malloc(vertices * sizeof(struct node*));

  int i;
  for (i = 0; i < vertices; i++)
    graph->adjLists[i] = NULL;

  return graph;
}

//Adiciona Aresta (caso não exista repetida)
void adicionaAresta(struct Graph* graph, int s, int d) {
    for(struct node *atual = graph->adjLists[s]; atual != NULL; atual = atual->prox)
        if(atual->vertice == d) 
            return;
    struct node* novoNodo = criaNodo(d);
    novoNodo->prox = graph->adjLists[s];
    graph->adjLists[s] = novoNodo;
}

//Imprime o Grafo
void printGraph(struct Graph* graph) {
  int v;
  for (v = 0; v < graph->numVertices; v++) {
    struct node* temp = graph->adjLists[v];
    printf("\n vertice %d\n: ", v);
    while (temp) {
      printf("%d -> ", temp->vertice);
      temp = temp->prox;
    }
    printf("\n");
  }
}