#include <stdio.h> 
#include <stdlib.h>
#include "listaadj.h"

int main()
{
    int n; // Tamanho da matriz quadrada (tabuleiro)
    scanf("%d", &n);
    
    int tabuleiro[n][n];

    GRAFO *lista_adj = criagrafo(n*n); 

    // Preenchendo matriz de adj
    for (int i = 0; i < n*n; i++){
        if (i+1 < n*n && i/n == (i+1)/n) // insere aresta com o próximo valor da mesma linha
            inserearesta(lista_adj, i, i+1);
        if (i+n < n*n)  // insere aresta com o próximo valor da mesma coluna
            inserearesta(lista_adj, i, i+n);
    } 

    exibelistaadj(lista_adj); // 0 1 2
    return 0;                 // 3 4 5 
}                             // 6 7 8 -> num % 3 = j // num/3 - i

    // 0: 1 3
    // 1: 0 2 4
    // 2: 1 5
    // 3: 0 4 6
    // 4: 1 3 5 7
    // 5: 2 4 8
    // 6: 3 7
    // 7: 4 6 8
    // 8: 5 7
    
//   0 1 2 3 4 5 6 7 8           
// 0 0 1 0 1 0 0 0 0 0
// 1 1 0 1 0 1 0 0 0 0
// 2 0 1 0 0 0 1 0 0 0
// 3 1 0 0 0 1 0 1 0 0
// 4 0 1 0 1 0 1 0 1 0
// 5 0 0 1 0 1 0 0 0 1
// 6 0 0 0 1 0 0 0 1 0
// 7 0 0 0 0 1 0 1 0 1
// 8 0 0 0 0 0 1 0 1 0

/** SCRIPT 0 FALHAS
1 - montar o tabuleiro -> matriz de adjacencia de tam N^2
    - tabuleiro nxn
    - cada casa do tabuleiro é um vértice
    - cada posição da matriz de adj é uma aresta do tabuleiro
    - cada aresta (onde duas casas se encontram) do tabuleiro
      é um vértice da matriz de adj     
2 - inserir o ghost - [rand(N),rand(N)]?
3 - mapear 
    - BFS
    - a partir da pos do fantasma, chamada recursiva p os 4 lados mandando como parametro (cnt+1)
    - caso base -> out of bounds
4 - inserir o pacman
5 - ir ate o fantasma
    - checar menor lado e ir
*/