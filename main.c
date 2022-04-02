#include <stdio.h>
#include <stdlib.h>

struct Transacao
{
    int t;
    char acao;
    char variavel;
    int Commit;
};

void zera_matriz(int **m, const size_t n_linhas, const size_t n_colunas)
{
    for (size_t i = 0; i < n_linhas; i++)
    {
        for (size_t j = 0; j < n_colunas; j++)
        {
            m[i][j] = 0.0;
        }
    }
}

void **aloca_matriz(const size_t n_linhas, const size_t n_colunas, const size_t tam)
{
    void **matriz = malloc(n_linhas * sizeof(void *));
    if (matriz == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < n_linhas; i++)
    {
        matriz[i] = malloc(n_colunas * tam);
        if (matriz[i] == NULL)
        {
            for (size_t j = 0; j < i; j++)
            {
                free(matriz[j]);
            }
            free(matriz);
            return NULL;
        }
    }
    return matriz;
}

void adiciona_linha(struct Transacao *linha,int it, int t, char acao, char variavel){
    printf("ITERACAO: %d\n", it);
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

void monta_serialibilidade(struct Transacao *linha, int it, int inicio_transacao,int qntd_de_transacao_ativas){
    int transacao_inicial = _menor_transa(linha, it, inicio_transacao,qntd_de_transacao_ativas);
    int transacao_final = _maior_transa(linha, it, inicio_transacao, qntd_de_transacao_ativas);
    printf("Menor: %d, Maior: %d\n", transacao_inicial, transacao_final);
    int **M = (int **)aloca_matriz(transacao_final+1,transacao_final+1, sizeof(int));
    zera_matriz(M,transacao_final+1, transacao_final+1);
    for(int i = inicio_transacao; i < it; i++){
        int transacao_atual = linha[i].t;
        char acao_atual = linha[i].acao;
        char variavel_atual = linha[i].variavel;
        for(int j = i  + 1; j < it; j++){
            if(acao_atual == 'R'){
                if(linha[j].t != transacao_atual && linha[j].acao == 'W' && linha[j].variavel == variavel_atual){
                    M[transacao_atual][linha[j].t] = 1;
                }
            }
            if(acao_atual == 'W'){
                if(linha[j].t != transacao_atual && linha[j].acao == 'W' && linha[j].variavel == variavel_atual){
                    M[transacao_atual][linha[j].t] = 1;
                }
                if(linha[j].t != transacao_atual && linha[j].acao == 'R' && linha[j].variavel == variavel_atual){
                    M[transacao_atual][linha[j].t] = 1;
                }   
            }
        }
    }
    for(int i = transacao_inicial; i < transacao_final+1; i++){
        for(int j = transacao_inicial; j < transacao_final+1; j++){
            printf("%d  ", M[i][j]);
        }
        printf("\n");
    }
}

int main(){
    struct Transacao *linha = (struct Transacao *) malloc(5000 * sizeof(struct Transacao));
    int it;
    int t;
    char acao;
    char variavel;
    int qntd_de_transacao_ativas = 0;
    int inicio_transacao = 1;

    while(fscanf(stdin, "%d  %d  %c  %c", &it, &t, &acao, &variavel) != EOF){
        if(t > qntd_de_transacao_ativas)
            qntd_de_transacao_ativas++;
        adiciona_linha(linha,it,t,acao,variavel);
        if(acao == 'C'){
            if(verifica_commits(linha,it, qntd_de_transacao_ativas)){
                monta_serialibilidade(linha, it, inicio_transacao, qntd_de_transacao_ativas);
                //monta_igual();
                inicio_transacao = it;
                qntd_de_transacao_ativas = 0;
            }
        }
        printf("%d %d %c %c\n", it, linha[it].t, linha[it].acao, linha[it].variavel);
    };
}