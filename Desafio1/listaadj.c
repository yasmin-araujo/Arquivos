#include "listaadj.h"
#include <stdio.h>
#include <stdlib.h>

struct _no {
    int vertice;
    struct _no *proximo;
};

struct _lista {
    int tamanho;
    NO* inicio;
    NO* fim;
};

struct _grafo {
    int numvertices;
    LISTA** listaadj;  
};


GRAFO* criagrafo(int n){
    GRAFO* g = malloc(sizeof(GRAFO));
    g->listaadj = malloc(sizeof(LISTA*) * n);
    
    for(int i = 0; i < n; i++){
        g->listaadj[i] = malloc(sizeof(LISTA));
        NO* novo = malloc(sizeof(NO));
        novo->proximo = NULL;
        novo->vertice = i;
        g->listaadj[i]->inicio = novo;
        g->listaadj[i]->fim = novo;
        g->listaadj[i]->tamanho = 1;
    }
    
    g->numvertices = n;

    return g;
}


void inserearesta(GRAFO* grafo, int i , int j){
    //verifica se a aresta já existe

    // if (i < 0 || j < 0 || i > grafo->numvertices*grafo->numvertices || j > grafo->numvertices*grafo->numvertices){
    //     return;
    // }

    NO* existe = grafo->listaadj[i]->inicio->proximo;
    int flag = 1;
    if(existe == NULL) flag = 0;
    while(flag){
        if(existe == NULL) break;  //caso não seja encontrada a aresta
        if(existe->vertice == j) return; //caso a aresta tenha sido encontrada
        existe = existe->proximo;
    }

    NO* novo1 = malloc(sizeof(NO));
    NO* novo2 = malloc(sizeof(NO));
    novo1->proximo = NULL;
    novo1->vertice = j;
    novo2->proximo = NULL;
    novo2->vertice = i;

    grafo->listaadj[i]->fim->proximo = novo1;
    grafo->listaadj[i]->fim = novo1;
    grafo->listaadj[i]->tamanho++;

        
    grafo->listaadj[j]->fim->proximo = novo2;
    grafo->listaadj[j]->fim = novo2;
    grafo->listaadj[j]->tamanho++;

}


void removearesta(GRAFO* grafo, int i, int j){
    //removendo a aresta  na lista do vertice i
    NO* remove_j = grafo->listaadj[i]->inicio->proximo;
    NO* aux_j  = grafo->listaadj[i]->inicio;
    while(1){
        if(remove_j == NULL) return; //caso a aresta não tenha sido encontrada
        if(remove_j->vertice == j) break;
        aux_j = remove_j;
        remove_j = remove_j->proximo;
    }
    NO* temp_j = remove_j;    
    aux_j->proximo = remove_j->proximo;
    free(temp_j);

    //removendo a aresta na lista do vertice j
    NO* remove_i = grafo->listaadj[j]->inicio->proximo;
    NO* aux_i = grafo->listaadj[j]->inicio;
    while(1){
        if(remove_i == NULL) return; //caso a aresta não tenha sido encontrada
        if(remove_i->vertice == i) break;
        aux_i = remove_i;
        remove_i = remove_i->proximo;
    }
    
    NO* temp_i = remove_i;
    aux_i->proximo = remove_i->proximo;
    free(temp_i);
}

//exibe o grafo na forma de lista de adjacência
void exibelistaadj(GRAFO* grafo){
    NO* aux;
    for(int i = 0; i < grafo->numvertices; i++){
        printf("%d: ",grafo->listaadj[i]->inicio->vertice);
        aux = grafo->listaadj[i]->inicio->proximo;
        while(aux != NULL){
            printf("%d ", aux->vertice);
            aux = aux->proximo;
        }
        printf("\n");
    }
    printf("\n");
}


void deletagrafo(GRAFO* grafo){
    for(int i = 0; i< grafo->numvertices; i ++){
        NO* remove = grafo->listaadj[i]->inicio->proximo;
        NO* aux = grafo->listaadj[i]->inicio;
        while(remove != NULL){
            free(aux);
            aux = remove;
            remove = remove->proximo;
        }
        free(grafo->listaadj[i]);
    }
    free(grafo->listaadj);
    free(grafo);
    return;
}
