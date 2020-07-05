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
    int c[5];  // Conteúdo da chave de busca
    int PR[5]; // RRN
    int P[6];  // RRN dos descendentes
}

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

void atualiza_cabecalho(registro_cabecalho *regcb, FILE *fpI)
{
    fseek(fpI, 0, SEEK_SET)
        fwrite(&regcb->status, sizeof(char), 1, fpI);
    fwrite(&regcb->noRaiz, sizeof(int), 1, fpI);
    fwrite(&regcb->nroNiveis, sizeof(int), 1, fpI);
    fwrite(&regcb->proxRRN, sizeof(int), 1, fpI);
    fwrite(&regcb->nroChaves, sizeof(int), 1, fpI);
}

void recupera_cabecalho(registro_cabecalho *regdc, FILE *fpI, int RRN)
{
    fseek(fpI, 0, SEEK_SET);
    fread(&regdc->status, sizeof(char), 1, fpI);
    fread(&regdc->noRaiz, sizeof(int), 1, fpI);
    fread(&regdc->nroNiveis, sizeof(int), 1, fpI);
    fread(&regdc->proxRRN, sizeof(int), 1, fpI);
    fread(&regdc->nroChaves, sizeof(int), 1, fpI);
}

// Cria um novo nó e retorna seu RRN
int criar_no(registro_dados *regdb, FILE *fpI)
{
    registro_cabecalho *regcb = (registro_cabecalho *)malloc(sizeof(registro_cabecalho));
    recupera_cabecalho(regcb);
    regdb->nivel = 1;
    regdb->n = 0;
    for (int i = 0 i < 5 i++)
    {
        C[i] = -1 PR[i] = -1 P[i] = -1
    }
    P[5] = -1;

    fseek(fpI, (regcb->proxRRN + 1) * 72, SEEK_SET)

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

    atualiza_cabecalho(regcb, fpI);
    return regcb->proxRRN - 1;
}

void recupera_pagina(registro_dados *regdb, FILE *fpI, int RRN)
{
    fseek(fpI, (RRN + 1) * 72, SEEK_SET);
    fread(&regdb->nivel, sizeof(int), 1, fpI);
    fread(&regdb->n, sizeof(int), 1, fpI);
    fread(&regdb->C1, sizeof(int), 1, fpI);
    fread(&regdb->PR1, sizeof(int), 1, fpI);
    fread(&regdb->C2, sizeof(int), 1, fpI);
    fread(&regdb->PR2, sizeof(int), 1, fpI);
    fread(&regdb->C3, sizeof(int), 1, fpI);
    fread(&regdb->PR3, sizeof(int), 1, fpI);
    fread(&regdb->C4, sizeof(int), 1, fpI);
    fread(&regdb->PR4, sizeof(int), 1, fpI);
    fread(&regdb->C5, sizeof(int), 1, fpI);
    fread(&regdb->PR5, sizeof(int), 1, fpI);
    fread(&regdb->P1, sizeof(int), 1, fpI);
    fread(&regdb->P2, sizeof(int), 1, fpI);
    fread(&regdb->P3, sizeof(int), 1, fpI);
    fread(&regdb->P4, sizeof(int), 1, fpI);
    fread(&regdb->P5, sizeof(int), 1, fpI);
    fread(&regdb->P6, sizeof(int), 1, fpI);
}

void insercao(FILE *fpI)
{
    // Se busca já encontrou chave a ser inserida
    if (busca()) return;

    if(insere())
    {
        //se insere retorna 1 
        // criar novo nó para ser raiz e inserir essa chave promovida
        // pegar nó raiz do cabeçalho
        int no_raiz;
        cabeçalho->noRaiz = criar_raiz();
        //atualizar no raiz do cabeçalho
    }
}
/**
 * Função para criar um novo nó raiz quando houve split do anterior
 * 
 * @param no_raiz: RRN do nó raiz atual
 * @param promo_key: Chave promovida
 * @param promo_RRN: RRN da chave promovida
 * @param promo_r_child: Filho a direita da chave promovida
 * @return raiz: RRN do novo nó raiz
 */
int criar_raiz(FILE *fpI, int no_raiz, int promo_key, int promo_RRN, int promo_r_child)
{
    registro_dados *regdb;
    int nova_raiz = criar_no(regdb, fpI);
    regdb->C[0] = promo_key;
    regdb->PR[0] = promo_RRN;
    regdb->P[0] = no_raiz;
    regdb->P[1] = promo_r_child;
    return nova_raiz;
}

/**
 * Função para realizar o split de um nó
 * 
 * @param i_key: Nova chave a ser inserida
 * @param i_RRN: Filho a direita da nova chave a ser inserida
 * @param page: Página de disco corrente
 * @param promo_key: Chave promovida
 * @param promo_RRN: RRN da chave promovida
 * @param promo_r_child: Filho a direita da chave promovida
 * @param new_page: Nova página de disco
*/
void split(int i_key, int i_RRN, registro_dados *page, int *promo_key, int *promo_RRN, int *promo_r_child, registro_dados *new_page)
{
    // Encontra lugar correto para inserir nova chave
    int count = 0;
    while (i_key > C[count])
        count++;

    // Variáveis auxiliares para divisão da página, contendo todas as chaves pré-existentes + nova chave
    int C_aux[6];
    int PR_aux[6];

    // Preenche novas variáveis na ordenação correta
    for (int i = 0, j = 0; i < 6; i++, j++)
    {
        if (i == count)
            j++;
        C_aux[j] = page->C[i];
        PR_aux[j] = page->PR[i];
    }
    C_aux[count] = i_key;
    PR_aux[count] = i_RRN;

    // Corrige o primeiro nó com nova ordenação
    for (int i = 0; i < 3; i++)
    {
        page->C[i] = C_aux[i];
        page->PR[i] = PR_aux[i];
    }
    page->C[4] = -1;
    page->C[5] = -1;
    page->PR[4] = -1;
    page->PR[5] = -1;

    // Promove chave e cria nova página
    *promo_key = C_aux[3];
    *promo_RRN = PR_aux[3];
    *promo_r_child = inicializar_no(new_page);

    // Atualiza valores
    new_page->nivel = page->nivel;
    page->n = 3;
    new_page->n = 2;

    // Preenche o segundo nó
    for (int i = 4; i < 6; i++)
    {
        new_page->C[i] = C_aux[i];
        new_page->PR[i] = PR_aux[i];
    }
}

/**
 * @param current_RRN: RRN da página da árvore-B que está em uso
 * @param key: Chave a ser inserida
 * @param promo_key: Retornará na recursão a chave a ser promovida caso necesário
 * @param promo_r_child: Retornará o ponteiro para o filho direito de promo_key caso necessário
 * @return promocao: TRUE ou FALSE dependendo se ela é necessária, ou ERRO (-1)
 * */
int insere(int current_RRN, int key, int *promo_key, int *promo_r_child)
{
    // Caso-base - Caso tenha chego em um nó não existente, retorna
    if (current_RRN == -1)
    {
        *promo_key = key;
        *promo_r_child = -1;
        return 1;
    }

    // Recupera dados da página do current_RRN
    registro_dados *regdb = (registro_dados *)malloc(sizeof(registro_dados));
    recupera_pagina(regdb, current_RRN);

    // Procura próximo RRN onde chave deveria ser inserida
    int next_RRN = -1;

    for (int i = 0; i < 6; i++)
    {
        if (i == 5)
            next_RRN = regdb->P[5];

        else if (key < regdb->c[i])
        {
            RRN = regdb->P[i];
            break;
        }
    }

    // Chama recursão novamente até chegar em um nó folha
    int return_value = insere(next_RRN, key, promo_key, promo_r_child);

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
                regdb->C[0] = *promo_key;
                break;
            }

            // Caso não seja inserido na última posição, da shift right nos valores seguintes
            if (*promo_key < regdb->C[j] || regdb->C[j] == -1)
            {
                for (int i = 4; i > j; i--)
                    regdb->C[i] = regdb->C[i - 1];

                // Insere
                regdb->C[j] = *promo_key;
                break;
            }
        }

        return 0; // False para promoção
    }
    else // Caso não exista espaço no nó
    {
        // Inicializa variáveis com dados que serão retornados da função split
        int promo_key = -1;
        int promo_RRN = -1;
        int promo_r_child = -1;
        registro_dados *new_regdb = (registro_dados *)malloc(sizeof(registro_dados));

        split(*promo_key, *promo_r_child, regdb, &promo_key, &promo_RRN, &promo_r_child, new_regdb);
        
        // fwrrite page no current_RRN
        // fwrite newpage no RRN promo_r_child
        return 1; // True para promoção
    }
}

/**
 * @param RRN: Página a ser pesquisada
 * @param key: Chave sendo procurada
 * @param found_RRN: Página que contém a chave
 * @param found_pos: Posição da chave na página
 * @return found: TRUE ou FALSE caso tenha achado a chave ou não
 */
int busca(int RRN, int key, int *found_RRN, int *found_pos)
{
    // Caso base - caso tenha chego ao fim da árvore
    if (RRN == -1)
        return 0;

    registro_dados *regdb = (registro_dados *)malloc(sizeof(registro_dados));
    // pegar dados da pagina que tem rrn = RRN -> começa com o RRN da raiz
    recupera_pagina(regdb, RRN);
    // checar se key esta em algum dos Cs - registro_dados *regdb
    switch (key)
    {
    case regdb->C[0]:
        *found_RRN = regdb->PR1;
        *found_pos = 1;
        break;
    case regdb->C[1]:
        *found_RRN = regdb->PR2;
        *found_pos = 2;
        break;
    case regdb->C[2]:
        *found_RRN = regdb->PR3;
        *found_pos = 3;
        break;
    case regdb->C[3]:
        *found_RRN = regdbdb->PR4;
        *found_pos = 4;
        break;
    case regdb->C[4]:
        *found_RRN = regdb->PR5;
        *found_pos = 5;
        break;
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
  
        else if (key < regdb->c[i])
        {
            RRN = regdb->P[i];
            break;
        }
    }

    free(regdb);
    return busca(RRN, key, found_RRN, found_pos);
}