#ifndef ARVOREB_H
    #define ARVOREB_H

    #define MAX_CHAVES 5

    typedef struct _registro_cabecalho registro_cabecalho;
    typedef struct _registro_dados registro_dados;

    void criar_arvoreB(FILE *fpI);

    void atualiza_cabecalho_arvoreB(registro_cabecalho *regcb, FILE *fpI);

    void recupera_cabecalho_arvoreB(registro_cabecalho *regcb, FILE *fpI);

    int criar_no(FILE *fpI);
    
    void atualiza_pagina(registro_dados *regdb, FILE* fpI, int RRN);
    
    void recupera_pagina(registro_dados *regdb, FILE *fpI, int RRN);
    
    int busca(int RRN, int key, int *found_RRN, FILE *fpI);

    int insere_arvoreB(int current_RRN, int key, int RRN, int *promo_key, int *promo_RRN, int *promo_r_child, FILE *fpI);
    
    void insercao_arvoreB(FILE *fpI, int key, int RRN);
    
    void criar_raiz(FILE *fpI, int no_raiz, int promo_key, int promo_RRN, int promo_r_child);

    void split(int i_key, int i_RRN, int i_r_child, registro_dados *page, int *promo_key, int *promo_RRN, int *promo_r_child, registro_dados *new_page, FILE *fpI);

#endif