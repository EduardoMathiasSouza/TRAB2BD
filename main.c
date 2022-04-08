#include <stdio.h>
#include <stdlib.h>
#include "grafos.h"

struct Transacao
{
    int t;
    char acao;
    char variavel;
    int Commit;
};

void adiciona_linha(struct Transacao *linha,int it, int t, char acao, char variavel){
    linha[it].t = t;
    linha[it].acao = acao;
    linha[it].variavel = variavel;
    linha[it].Commit = 0;
    if(acao == 'C'){
        linha[it].Commit = 1;}
}

int verifica_commits(struct Transacao *linha, int it, int qntd_de_transacao_ativas){
    int commits = 1;
    for(int i = it-1; i > 1; i--)
        if(linha[i].Commit)
            commits++;
    if(commits == qntd_de_transacao_ativas)
        return 1;
    return 0;
}

int _menor_transa(struct Transacao *linha, int it, int inicio_transacao,int qntd_de_transacao_ativas){
    int menor_transa = linha[inicio_transacao].t;
    for(int i = inicio_transacao; i < it; i++)
        if(linha[i].t < menor_transa)
            menor_transa = linha[i].t;
    return menor_transa;
}

int _maior_transa(struct Transacao *linha, int it, int inicio_transacao, int qntd_de_transacao_ativas){
    int maior_transa = linha[inicio_transacao].t;
    for(int i = inicio_transacao; i < it; i++)
        if(linha[i].t > maior_transa)
            maior_transa = linha[i].t; 
    return maior_transa;
}

int testa_ciclo(struct Graph* Grafo, int v, int visitou[], int *path)
{
    if (visitou[v] == -1)
    {
        visitou[v] = 1;
        path[v] = 1;
        struct node* temp = Grafo->adjLists[v];
        while (temp)
        {
            if (visitou[temp->vertex] == -1 && testa_ciclo(Grafo, temp->vertex, visitou, path))
                return 1;
            else if (path[temp->vertex])
                return 1;
            temp = temp->next;
        }
    }
    path[v] = 0;
    return 0;
}

int detecta_ciclo(struct Graph *Grafo, int V)
{
    int *visitou = (int*)malloc(V * sizeof(int));
    int *path = (int*)malloc(V * sizeof(int));
    int i;
    for (i = 0; i < V; i++)
    {
        visitou[i] = -1;
        path[i] = 0;
    }
    for (i = 0; i < V; i++)
        if (visitou[i] == -1 && testa_ciclo(Grafo, i, visitou, path))
            return 1;
    return 0;
}

void imprime_transacoes(int transacao_inicial,int transacao_final){
    for(int i = transacao_inicial; i < transacao_final; i++)
        fprintf(stdout, "%d,", i);
    fprintf(stdout, "%d ", transacao_final);
}

void monta_serialibilidade(struct Transacao *linha, int it, int inicio_transacao,int qntd_de_transacao_ativas){
    int transacao_inicial = _menor_transa(linha, it, inicio_transacao,qntd_de_transacao_ativas);
    int transacao_final = _maior_transa(linha, it, inicio_transacao, qntd_de_transacao_ativas);
    struct Graph *Grafo = createAGraph(transacao_final+1);
    for(int i = inicio_transacao; i < it; i++){
        int transacao_atual = linha[i].t;
        char acao_atual = linha[i].acao;
        char variavel_atual = linha[i].variavel;
        for(int j = i  + 1; j < it; j++){
            if(acao_atual == 'R'){
                if(linha[j].t != transacao_atual && linha[j].acao == 'W' && linha[j].variavel == variavel_atual)
                    addEdge(Grafo,transacao_atual, linha[j].t);
            }
            if(acao_atual == 'W'){
                if(linha[j].t != transacao_atual && linha[j].acao == 'W' && linha[j].variavel == variavel_atual)
                   addEdge(Grafo,transacao_atual, linha[j].t);
                if(linha[j].t != transacao_atual && linha[j].acao == 'R' && linha[j].variavel == variavel_atual)
                   addEdge(Grafo,transacao_atual, linha[j].t);
            }
        }
    }
    imprime_transacoes(transacao_inicial, transacao_final);
    if(detecta_ciclo(Grafo, Grafo->numVertices))
        fprintf(stdout, "NS");
    else
        fprintf(stdout, "SS");
}

int conta_variaveis(struct Transacao *linha, int it, int inicio_transacao, char *variaveis){
    int j = 0;
    variaveis[j] = linha[inicio_transacao].variavel;
    for(int i=inicio_transacao; i < it; i++)
    {
        int achou = 0;
        if(linha[i].variavel != '-'){
            for(int k=0; k< it; k++){
                if(variaveis[k] == linha[i].variavel)
                    achou = 1;
                }
            if(!achou){
                j++;
                variaveis[j] = linha[i].variavel;
                } 
        }
    }
    return j;
}

int acha_ultimo_write(struct Transacao *linha, int it, int inicio_transacao,char variavel_atual){
    for(int i = it; i > inicio_transacao; i--){
        if(linha[i].variavel == variavel_atual && linha[i].acao == 'W')
            return i;
    }
    return inicio_transacao;
}

int monta_visao(struct Transacao *linha, int it, int inicio_transacao,char variavel_atual){
    int serializavel = 1;
    char *variaveis = (char *) malloc (sizeof(char) *it);
    int n_variaveis = conta_variaveis(linha,it,inicio_transacao,variaveis) + 1;
    for(int i = 0; i < n_variaveis; i++){
        int pos_atual = acha_ultimo_write(linha, it, inicio_transacao,variaveis[i]);
        for(int j = pos_atual; j > inicio_transacao - 1; j--){
            int tipo_atual = linha[pos_atual].t;
            if(linha[j].acao == 'R' && linha[j].t == tipo_atual && linha[j].variavel == variaveis[i]){
                for(int k = j+1; k < pos_atual; k++){
                    if(linha[k].t != linha[j].t && linha[k].acao == 'W' && linha[k].variavel == variaveis[i]){
                        serializavel = 0;
                        break;
                    }
                }
            }
        }
    }
    return serializavel;
}

int main(){
    struct Transacao *linha = (struct Transacao *) malloc(5000 * sizeof(struct Transacao));
    int it;
    int t;
    char acao;
    char variavel;
    int qntd_de_transacao_ativas = 0;
    int inicio_transacao = 1;
    int count = 1;
    while(fscanf(stdin, "%d  %d  %c  %c", &it, &t, &acao, &variavel) != EOF){
        if(t > qntd_de_transacao_ativas)
            qntd_de_transacao_ativas++;
        adiciona_linha(linha,it,t,acao,variavel);
        if(acao == 'C'){
            if(verifica_commits(linha,it, qntd_de_transacao_ativas)){
                fprintf(stdout, "%d ", count);
                monta_serialibilidade(linha, it, inicio_transacao, qntd_de_transacao_ativas);
                if (monta_visao(linha, it, inicio_transacao, qntd_de_transacao_ativas))
                    fprintf(stdout," SV\n");
                else
                    fprintf(stdout, " NV\n");
                count++;
                inicio_transacao = it+1;
                qntd_de_transacao_ativas = 0;
            }
        }
        //printf("%d %d %c %c\n", it, linha[it].t, linha[it].acao, linha[it].variavel);
    };
}