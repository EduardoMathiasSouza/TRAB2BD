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
    //Se nao visitou o vertice
    if (visitou[v] == -1)
    {
        visitou[v] = 1;//Vertice visitado
        path[v] = 1;
        struct node* temp = Grafo->adjLists[v];//Pega o primeiro vertice da lista de adjacencia do vertice v
        while (temp) //enquanto houver vertices na lista dele
        {
            if (visitou[temp->vertice] == -1 && testa_ciclo(Grafo, temp->vertice, visitou, path))//Se o vertice nao foi visitado e chama recursivamente para verificar se o vertice tem ciclo
                return 1;
            else if (path[temp->vertice]) //Se o vertice ja foi visitado e o vertice que ele esta conectado ao vertice v tem ciclo
                return 1;
            temp = temp->prox;
        }
    }
    path[v] = 0;
    return 0;
}

int detecta_ciclo(struct Graph *Grafo, int V)
{
    //Cria vetores para salvar se visitou e o se ja foi visitado anteriormente e inicializa eles com -1 e 0
    int *visitou = (int*)malloc(V * sizeof(int));
    int *path = (int*)malloc(V * sizeof(int));
    int i;
    for (i = 0; i < V; i++)
    {
        visitou[i] = -1;
        path[i] = 0;
    }
    for (i = 0; i < V; i++)
        if (visitou[i] == -1 && testa_ciclo(Grafo, i, visitou, path))//Se ainda nao visitou o vertice i e tiver um ciclo
            return 1;
    return 0;
}

void imprime_transacoes(int transacao_inicial,int transacao_final){
    for(int i = transacao_inicial; i < transacao_final; i++)
        fprintf(stdout, "%d,", i);
    fprintf(stdout, "%d ", transacao_final);
}

void monta_serialibilidade(struct Transacao *linha, int it, int inicio_transacao,int qntd_de_transacao_ativas){
    int transacao_inicial = _menor_transa(linha, it, inicio_transacao,qntd_de_transacao_ativas);//Acha o menor t das transacoes ativas
    int transacao_final = _maior_transa(linha, it, inicio_transacao, qntd_de_transacao_ativas);//Acha o maior t das transacoes ativas
    struct Graph *Grafo = criaGrafo(transacao_final+1);//Cria grafo 
    for(int i = inicio_transacao; i < it; i++){//Do inicio da transacao, ate a iteracao de commit, analisa se:
        int transacao_atual = linha[i].t;
        char acao_atual = linha[i].acao;
        char variavel_atual = linha[i].variavel;
        for(int j = i  + 1; j < it; j++){
            if(acao_atual == 'R'){
                if(linha[j].t != transacao_atual && linha[j].acao == 'W' && linha[j].variavel == variavel_atual)//Existe R em Ti -> W em Tj de uma variavel
                    adicionaAresta(Grafo,transacao_atual, linha[j].t);
            }
            if(acao_atual == 'W'){
                if(linha[j].t != transacao_atual && linha[j].acao == 'W' && linha[j].variavel == variavel_atual) //Existe W em Ti -> W em Tj de uma variavel
                   adicionaAresta(Grafo,transacao_atual, linha[j].t);
                if(linha[j].t != transacao_atual && linha[j].acao == 'R' && linha[j].variavel == variavel_atual)//Existe W em Ti -> R em Tj de uma variavel
                   adicionaAresta(Grafo,transacao_atual, linha[j].t);
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
            if(!achou){//Se existe variavel que ainda nao esta no vetor, adiciona-a no mesmo
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
    int igualavel = 1;
    char *variaveis = (char *) malloc (sizeof(char) *it); //aloca um vetor de char para variaveis
    int n_variaveis = conta_variaveis(linha,it,inicio_transacao,variaveis) + 1; //conta as variaveis e adiciona ao vetor
    for(int i = 0; i < n_variaveis; i++){
        int pos_atual = acha_ultimo_write(linha, it, inicio_transacao,variaveis[i]);//Posicao atual para analise eh o ultimo write feito nas transacoes com a variavel em questao
        for(int j = pos_atual; j > inicio_transacao - 1; j--){
            int tipo_atual = linha[pos_atual].t;//Tipo atual eh o t que esta sendo analisada
            if(linha[j].acao == 'R' && linha[j].t == tipo_atual && linha[j].variavel == variaveis[i]){//Se nesse t, existe um read da variavel em questao que culminará num write que nao eh o dele proprio, entao nao eh igualavel
                for(int k = j+1; k < pos_atual; k++){
                    if(linha[k].t != linha[j].t && linha[k].acao == 'W' && linha[k].variavel == variaveis[i]){
                        igualavel = 0;
                        break;
                    }
                }
            }
        }
    }
    return igualavel;//Caso contrario, eh igualavel
}

int main(){
    struct Transacao *linha = (struct Transacao *) malloc(5000 * sizeof(struct Transacao));
    int it;
    int t;
    char acao;
    char variavel;
    int qntd_de_transacao_ativas = 0;
    int inicio_transacao = 1;//Inicio transacao eh o ponto de partida para a analise das transacoes
    int count = 1;
    while(fscanf(stdin, "%d  %d  %c  %c", &it, &t, &acao, &variavel) != EOF){//Enquanto tem linha para ler
        if(t > qntd_de_transacao_ativas)//Se surgiu uma nova transacao ativa, soma a quantidade de transacoes ativas
            qntd_de_transacao_ativas++;
        adiciona_linha(linha,it,t,acao,variavel);//Adiciona a linha na estrutura Linha
        if(acao == 'C'){//Se acao lida for C
            if(verifica_commits(linha,it, qntd_de_transacao_ativas)){//Verifica se todas as transacoes ativas deram commit
                fprintf(stdout, "%d ", count);//Imprime o numero da analise de Seriabilidade e de Visao
                monta_serialibilidade(linha, it, inicio_transacao, qntd_de_transacao_ativas);//Monta seriabilidade
                if (monta_visao(linha, it, inicio_transacao, qntd_de_transacao_ativas))//Monta visao e se for equivalente, imprime SS
                    fprintf(stdout," SV\n");
                else                        
                    fprintf(stdout, " NV\n");//Se nao imprime NV
                count++;//Aumenta o contador de analises de Seriabilidade e de Visao
                inicio_transacao = it+1;//O Inicio da Transacao passa a ser o ponto apos o commit da transacao atual
                qntd_de_transacao_ativas = 0;//Zera o numero de transacoes ativas
            }
        }
        //printf("%d %d %c %c\n", it, linha[it].t, linha[it].acao, linha[it].variavel);
    };
}