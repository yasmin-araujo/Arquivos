#include <stdio.h>
#include <stdlib.h>
#include "arvoreB.h"

//Struct para dos dados do registro do cabeçalho
struct _registro_cabecalho
{
    char status;
    int noRaiz;
    int nroNiveis;
    int proxRRN;
    int nroChaves;
};

// Struct para os dados de cada registro
struct _registro_dados
{
    int nivel;
    int n;     // Número de chaves presentes do nó
    int C[5];  // Conteúdo da chave de busca
    int PR[5]; // RRN
    int P[6];  // RRN dos descendentes
};

void criar_arvoreB(FILE *fpI)
{
    registro_cabecalho *regcb = (registro_cabecalho *)malloc(sizeof(registro_cabecalho));
    regcb->status = '1';
    regcb->noRaiz = -1;
    regcb->nroNiveis = 0;
    regcb->proxRRN = 0;
    regcb->nroChaves = 0;
    char lixo[56];
    for (int i = 0; i < 55; i++)
        lixo[i] = '$';

    fwrite(&regcb->status, sizeof(char), 1, fpI);
    fwrite(&regcb->noRaiz, sizeof(int), 1, fpI);
    fwrite(&regcb->nroNiveis, sizeof(int), 1, fpI);
    fwrite(&regcb->proxRRN, sizeof(int), 1, fpI);
    fwrite(&regcb->nroChaves, sizeof(int), 1, fpI);
    fwrite(lixo, sizeof(char), 55, fpI);
}

void atualiza_cabecalho_arvoreB(registro_cabecalho *regcb, FILE *fpI)
{
    //printf(" vamos ver agr, status %c  no raiz %d  nmr niveis %d proxrrn %d",regcb->status, regcb->noRaiz, regcb->nroNiveis, regcb->proxRRN);
    fseek(fpI, 0, SEEK_SET);
    fwrite(&regcb->status, sizeof(char), 1, fpI);
    fwrite(&regcb->noRaiz, sizeof(int), 1, fpI);
    fwrite(&regcb->nroNiveis, sizeof(int), 1, fpI);
    fwrite(&regcb->proxRRN, sizeof(int), 1, fpI);
    fwrite(&regcb->nroChaves, sizeof(int), 1, fpI);
}

void recupera_cabecalho_arvoreB(registro_cabecalho *regcb, FILE *fpI)
{
    fseek(fpI, 0, SEEK_SET);
    fread(&regcb->status, sizeof(char), 1, fpI);
    fread(&regcb->noRaiz, sizeof(int), 1, fpI);
    fread(&regcb->nroNiveis, sizeof(int), 1, fpI);
    fread(&regcb->proxRRN, sizeof(int), 1, fpI);
    fread(&regcb->nroChaves, sizeof(int), 1, fpI);
}

// Cria um novo nó e retorna seu RRN
int criar_no(FILE *fpI)
{
    registro_cabecalho *regcb = (registro_cabecalho *)malloc(sizeof(registro_cabecalho));
    recupera_cabecalho_arvoreB(regcb, fpI);

    registro_dados *regdb = (registro_dados *)malloc(sizeof(registro_dados));
    regdb->nivel = -1;
    regdb->n = 0;
    for (int i = 0; i < 5; i++)
    {
        regdb->C[i] = -1;
        regdb->PR[i] = -1;
        regdb->P[i] = -1;
    }
    regdb->P[5] = -1;

    fseek(fpI, (regcb->proxRRN + 1) * 72, SEEK_SET);

    fwrite(&regdb->nivel, sizeof(int), 1, fpI);
    fwrite(&regdb->n, sizeof(int), 1, fpI);
    for (int i = 0; i < 5; i++)
    {
        fwrite(&regdb->C[i], sizeof(int), 1, fpI);
        fwrite(&regdb->PR[i], sizeof(int), 1, fpI);
    }
    for (int i = 0; i < 6; i++)
    {
        fwrite(&regdb->P[i], sizeof(int), 1, fpI);
    }

    regcb->proxRRN++;
    atualiza_cabecalho_arvoreB(regcb, fpI);
    return regcb->proxRRN - 1;
}

void atualiza_pagina(registro_dados *regdb, FILE *fpI, int RRN)
{
    fseek(fpI, (RRN + 1) * 72, SEEK_SET);
    fwrite(&regdb->nivel, sizeof(int), 1, fpI);
    fwrite(&regdb->n, sizeof(int), 1, fpI);
    for (int i = 0; i < 5; i++)
    {
        fwrite(&regdb->C[i], sizeof(int), 1, fpI);
        fwrite(&regdb->PR[i], sizeof(int), 1, fpI);
    }
    for (int i = 0; i < 6; i++)
    {
        fwrite(&regdb->P[i], sizeof(int), 1, fpI);
    }
}

void recupera_pagina(registro_dados *regdb, FILE *fpI, int RRN)
{
    fseek(fpI, (RRN + 1) * 72, SEEK_SET);
    fread(&regdb->nivel, sizeof(int), 1, fpI);
    fread(&regdb->n, sizeof(int), 1, fpI);
    for (int i = 0; i < 5; i++)
    {
        fread(&regdb->C[i], sizeof(int), 1, fpI);
        fread(&regdb->PR[i], sizeof(int), 1, fpI);
    }
    for (int i = 0; i < 6; i++)
        fread(&regdb->P[i], sizeof(int), 1, fpI);
}

/**
 * @param RRN: Página a ser pesquisada
 * @param key: Chave sendo procurada
 * @param found_RRN: Página que contém a chave
 * @param found_pos: Posição da chave na página
 * @return found: TRUE ou FALSE caso tenha achado a chave ou não
 */
int busca(int RRN, int key, int *found_RRN, FILE *fpI)
{
    // Caso base - caso tenha chego ao fim da árvore
    if (RRN == -1)
        return 0;

    registro_dados *regdb = (registro_dados *)malloc(sizeof(registro_dados));
    // pegar dados da pagina que tem rrn = RRN -> começa com o RRN da raiz
    recupera_pagina(regdb, fpI, RRN);
    // checar se key esta em algum dos Cs - registro_dados *regdb
    for (int i = 0; i < 5; i++)
    {
        if (key == regdb->C[i])
        {
            *found_RRN = regdb->PR[i];
            break;
        }
    }

    // Se a chave existe retorna TRUE
    if (*found_RRN != -1)
    {
        free(regdb);
        return 1; // found
    }

    // Acha próximo RRN para continuar pesquisa
    for (int i = 0; i < 6; i++)
    {
        if (i == 5)
            RRN = regdb->P[5];

        else if (key < regdb->C[i])
        {
            RRN = regdb->P[i];
            break;
        }
    }

    free(regdb);
    return busca(RRN, key, found_RRN, fpI);
}

/**
 * @param current_RRN: RRN da página da árvore-B que está em uso
 * @param key: Chave a ser inserida
 * @param RRN: RRN da chave a ser inserida
 * @param promo_key: Retornará na recursão a chave a ser promovida caso necesário
 * @param promo_RRN: Retornará na recursão o RRN da chave a ser promovida caso necesário
 * @param promo_r_child: Retornará o ponteiro para o filho direito de promo_key caso necessário
 * @return promocao: TRUE ou FALSE dependendo se ela é necessária, ou ERRO (-1)
 * */
int insere_arvoreB(int current_RRN, int key, int RRN, int *promo_key, int *promo_RRN, int *promo_r_child, FILE *fpI)
{
    // Caso-base - Caso tenha chego em um nó não existente, retorna
    if (current_RRN == -1)
    {
        // printf("ok chegamos no nó folha\n");
        *promo_key = key;
        *promo_RRN = RRN;
        *promo_r_child = -1;
        return 1;
    }

    // Recupera dados da página do current_RRN
    registro_dados *regdb = (registro_dados *)malloc(sizeof(registro_dados));
    recupera_pagina(regdb, fpI, current_RRN);

    // Procura próximo RRN onde chave deveria ser inserida
    int next_RRN = -1;

    for (int i = 0; i < 6; i++)
    {
        // Se i = 5 -> última posição possível
        if (i == 5)
            next_RRN = regdb->P[5];
        // Checa se a key atual é menor que a chave que C[i]
        else if (key < regdb->C[i] || regdb->C[i] == -1)
        {
            next_RRN = regdb->P[i];
            break;
        }
    }

    // printf("next_RRN: %d\n", next_RRN);
    
    // Chama recursão novamente até chegar em um nó folha
    int return_value = insere_arvoreB(next_RRN, key, RRN, promo_key, promo_RRN, promo_r_child, fpI);

    // Caso não seja necessário promover nada, termina a função
    if (return_value == 0 || return_value == -1)
        return return_value;

    // Caso tenha espaço no nó para inserir uma nova chave
    if (regdb->n < MAX_CHAVES)
    {
        // Insere no local certo
        for (int j = 0; j < 5; j++)
        {
            // Caso esteja na última posição apenas insere
            if (j == 4)
            {
                regdb->C[4] = *promo_key;
                regdb->PR[4] = *promo_RRN;
                regdb->P[5] = *promo_r_child;
                regdb->n++;
                break;
            }

            // Caso não seja inserido na última posição, da shift right nos valores seguintes
            if (*promo_key < regdb->C[j] || regdb->C[j] == -1)
            {
                //printf("posicao pra inserir eh j = %d  \n", j);
                for (int i = 4; i > j; i--)
                {
                    regdb->C[i] = regdb->C[i - 1];
                    regdb->PR[i] = regdb->PR[i - 1];
                    regdb->P[i + 1] = regdb->P[i]; // Shift nos ponteiros a direita
                }

                // Insere
                regdb->C[j] = *promo_key;
                //printf("a chave que botamos foi promo_key: C[%d] = %d\n", j, regdb->C[j]);
                regdb->PR[j] = *promo_RRN;
                regdb->P[j + 1] = *promo_r_child;
                regdb->n++;
                break;
            }
            //printf("INSERE_ARVOREB: \nainda não inserimos e número de chaves no nó = regdb->n: %d \n", regdb->n);
            // for (int k = 0; k < 5; k++)
                //printf("chave regdbn->C[%d]: %3d | RRN da chave regdb->PR[%d]: %3d \n", k, regdb->C[k], k, regdb->PR[k]);
            //printf("\n");
        }
        atualiza_pagina(regdb, fpI, current_RRN);
        return 0; // False para promoção
    }
    else // Caso não exista espaço no nó
    {
        // Inicializa variáveis com dados que serão retornados da função split
        int split_promo_key = -1;
        int split_promo_RRN = -1;
        int split_promo_r_child = -1;
        registro_dados *new_regdb = (registro_dados *)malloc(sizeof(registro_dados));

        split(*promo_key, *promo_RRN, *promo_r_child, regdb, &split_promo_key, &split_promo_RRN, &split_promo_r_child, new_regdb, fpI);

        atualiza_pagina(regdb, fpI, current_RRN);
        atualiza_pagina(new_regdb, fpI, split_promo_r_child); // Novo nó criado no split

        *promo_key = split_promo_key;
        *promo_RRN = split_promo_RRN;
        *promo_r_child = split_promo_r_child;

        return 1; // True para promoção
    }
}

void insercao_arvoreB(FILE *fpI, int key, int RRN)
{
    //Recupera o RRN da raiz, para começar a busca a partir desse nó
    registro_cabecalho *regcb = (registro_cabecalho *)malloc(sizeof(registro_cabecalho));
    recupera_cabecalho_arvoreB(regcb, fpI);
    int RRN_raiz = regcb->noRaiz;

    // printf("\nCOMECEI BORA FAZER UMA NOVA INSERCAO\n");
    // printf("raiz: %d, prox RRN: %d, key: %d, RRN: %d\n", RRN_raiz, regcb->proxRRN, key, RRN);

    int found_RRN = -1;
    // Se busca já encontrou chave a ser inserida
    if (busca(RRN_raiz, key, &found_RRN, fpI))
    {
        // printf("achei a chave, sem insercao pra vc\n");
        return;
    }

    int promo_key = -1;
    int promo_r_child = -1;
    int promo_RRN = -1;

    // 1 quando houver split na raiz
    if (insere_arvoreB(RRN_raiz, key, RRN, &promo_key, &promo_RRN, &promo_r_child, fpI))
    {
        criar_raiz(fpI, RRN_raiz, promo_key, promo_RRN, promo_r_child);
    }

    recupera_cabecalho_arvoreB(regcb, fpI);
    regcb->nroChaves++;
    atualiza_cabecalho_arvoreB(regcb, fpI);
}

// relatorio - 7/7
// esse trabalho - 7/7
// prova orgcomp - 9/7
// -- Trab poo dia 10
// mini atividade PDI 9/7
// projeto final PDI - 13/7

// trabalho 2 sarita - 17/7 - SITE WEB ESTATICO apenas html
// trab poo 17/7
// prova calculus 18/7
// prova fisica 22/7

/**
 * Função para criar um novo nó raiz quando houve split do anterior
 * 
 * @param no_raiz: RRN do nó raiz atual
 * @param promo_key: Chave promovida
 * @param promo_RRN: RRN da chave promovida
 * @param promo_r_child: Filho a direita da chave promovida
 * @return raiz: RRN do novo nó raiz
 */
void criar_raiz(FILE *fpI, int no_raiz, int promo_key, int promo_RRN, int promo_r_child)
{
    //printf("e vamos de nova raiz: ");
    registro_cabecalho *regcb = (registro_cabecalho *)malloc(sizeof(registro_cabecalho));
    registro_dados *regdb = (registro_dados *)malloc(sizeof(registro_dados));
    int nova_raiz = criar_no(fpI); // RRN da nova raiz

    //printf("foi de %d para %d\n", no_raiz, nova_raiz);

    recupera_cabecalho_arvoreB(regcb, fpI);
    recupera_pagina(regdb, fpI, nova_raiz);
    regdb->nivel = ++regcb->nroNiveis;
    regdb->n++;
    regdb->C[0] = promo_key;
    regdb->PR[0] = promo_RRN;
    regdb->P[0] = no_raiz;
    regdb->P[1] = promo_r_child;
    regcb->noRaiz = nova_raiz;
    atualiza_pagina(regdb, fpI, nova_raiz);
    atualiza_cabecalho_arvoreB(regcb, fpI);
}

/**
 * Função para realizar o split de um nó
 * 
 * @param i_key: Nova chave a ser inserida
 * @param i_RRN: RRN da nova chave a ser inserida
 * @param i_r_child: Filho a direita da nova chave a ser inserida
 * @param page: Página de disco corrente
 * @param promo_key: Chave promovida
 * @param promo_RRN: RRN da chave promovida
 * @param promo_r_child: Filho a direita da chave promovida
 * @param new_page: Nova página de disco
*/
void split(int i_key, int i_RRN, int i_r_child, registro_dados *page, int *promo_key, int *promo_RRN, int *promo_r_child, registro_dados *new_page, FILE *fpI)
{
    // Encontra lugar correto para inserir nova chave
    int count = 0;
    while (count < MAX_CHAVES && i_key > page->C[count])
        count++;

    // Variáveis auxiliares para divisão da página, contendo todas as chaves pré-existentes + nova chave
    int C_aux[6];
    int PR_aux[6];
    int P_aux[7];

    P_aux[0] = page->P[0];

    // Preenche novas variáveis na ordenação correta
    for (int i = 0, j = 0; i < 6; i++, j++)
    {
        if (i == count)
            j++;
        C_aux[j] = page->C[i];
        PR_aux[j] = page->PR[i];
        P_aux[j + 1] = page->P[i + 1];
    }

    C_aux[count] = i_key;
    PR_aux[count] = i_RRN;
    P_aux[count + 1] = i_r_child;

    // Corrige o primeiro nó com nova ordenação
    for (int i = 0; i <= 2; i++)
    {
        page->C[i] = C_aux[i];
        page->PR[i] = PR_aux[i];
    }
    page->C[3] = -1;
    page->C[4] = -1;
    page->PR[3] = -1;
    page->PR[4] = -1;
    page->P[4] = -1;
    page->P[5] = -1;

    // Promove chave e cria nova página
    *promo_key = C_aux[3];
    *promo_RRN = PR_aux[3];
    *promo_r_child = criar_no(fpI);
    recupera_pagina(new_page, fpI, *promo_r_child);

    // Atualiza valores
    new_page->nivel = page->nivel;
    page->n = 3;
    new_page->n = 2;

    // Preenche o segundo nó
    for (int i = 4; i <= 5; i++)
    {
        new_page->C[i - 4] = C_aux[i];
        new_page->PR[i - 4] = PR_aux[i];
        new_page->P[i - 4] = P_aux[i];
    }
    new_page->P[2] = P_aux[6];

    //printf("SPLIT:\nnó antigo\n(%d) ", page->P[0]);
    // for (int i = 0; i <= 4; i++)
    // {
        //printf("%d (%d) ", page->C[i], page->P[i+1]);
    // }
    //printf("\nnó novo\n(%d) ", new_page->P[0]);
    // for (int i = 0; i <= 4; i++)
    // {
       // printf("%d (%d) ", new_page->C[i], new_page->P[i+1]);
    // }
    //printf("\n");
}

/**
 * DUVIDAS/PENDENCIAS DO MAL
 * 
 * 1 - status
 * 
 * 
 * 1- inserir ponteiros P no lugar certo
 * 2 - inicialzar nó???
 * 3 - busca - precisa do found_pos (acho q n provavelmente era pra versao anterior da inserção)
 * 
 * 
 * se fosse inserir em posição != primeira
 *      coloca no ponteiro da direita
 * se inserir na primeira
 *      shift todos os C e ponteiros 1x
 *      coloca no ponteiro da esquerda
 *      NA VDD só bota na direita mesmo pq o anterior é do nó menor :o
 * 
 * 
 * split de nao no folha
 * ultimo ponteiro vira primeiro ponteiro do novo nó irmao
*/
