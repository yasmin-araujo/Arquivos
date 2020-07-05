/**
 * Trabalho 3 - Organização de Arquivos
 * 
 * 11218809 - Flávio Salles
 * 11219004 - Yasmin Osajima de Araujo
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binarioNaTela.h"

//Struct para dos dados do registro do cabeçalho
typedef struct _registro_cabecalho
{
    char status;
    int RRNproxRegistro;
    int numeroRegistrosInseridos;
    int numeroRegistrosRemovidos;
    int numeroRegistrosAtualizados;
} registro_cabecalho;

// Struct para os dados de cada registro
typedef struct _registro_dados
{
    int campo1;
    int campo2;
    char cidadeMae[105];
    char cidadeBebe[105];
    int idNascimento;
    int idadeMae;
    char dataNascimento[11];
    char sexoBebe;
    char estadoMae[3];
    char estadoBebe[3];
} registro_dados;

/**
 * Função para atualizar os dados de cabeçalho no arquivo gerado
 * 
 * @param fpG: Ponteiro para arquivo gerado
 * @param regc: Ponteiro para struct do cabeçalho
 * @param insereLixo: Flag para inserção de lixo no registro
*/
void atualiza_cabecalho(FILE *fpG, registro_cabecalho *regc, int insereLixo)
{
    // Vai para o começo do arquivo
    fseek(fpG, 0, SEEK_SET);

    // Escreve os dados
    fwrite(&regc->status, sizeof(char), 1, fpG);
    fwrite(&regc->RRNproxRegistro, sizeof(int), 1, fpG);
    fwrite(&regc->numeroRegistrosInseridos, sizeof(int), 1, fpG);
    fwrite(&regc->numeroRegistrosRemovidos, sizeof(int), 1, fpG);
    fwrite(&regc->numeroRegistrosAtualizados, sizeof(int), 1, fpG);

    // Caso true -> está criando o cabeçalho, portanto escreve o lixo
    if (insereLixo)
    {
        char lixo[111];
        for (int i = 0; i < 111; i++)
            lixo[i] = '$';
        fwrite(&lixo, sizeof(char), 111, fpG);
    }
}

/**
 * Função para recuperar os dados do arquivo gerado
 * 
 * @param fpG: Ponteiro para arquivo gerado
 * @param regd: Ponteiro para struct de dados
*/
void recupera_registro(FILE *fpG, registro_dados *regd)
{
    // Variável auxiliar para lixo dos campos variáveis
    char lixo[105];

    // Caso o RRN pedido seja uma posição inválida (fora dos limites do arquivo)
    char testa;
    if (fread(&testa, 1, 1, fpG) == 0)
    {
        // Salva na variável apenas para representar que o registro não existe
        regd->campo1 = -1;
        return;
    }

    fseek(fpG, -1, SEEK_CUR);

    // Leitura dos dados

    fread(&regd->campo1, sizeof(int), 1, fpG);

    if (regd->campo1 == -1)
    {
        // Move ponteiro para o próximo registro caso esteja excluído
        regd->campo1 = -1;
        fseek(fpG, 124, SEEK_CUR);
        return;
    }

    fread(&regd->campo2, sizeof(int), 1, fpG);
    fread(regd->cidadeMae, sizeof(char), regd->campo1, fpG);
    fread(regd->cidadeBebe, sizeof(char), regd->campo2, fpG);
    regd->cidadeMae[regd->campo1] = '\0'; // Insere '\0' no final da string de tamanho variável
    regd->cidadeBebe[regd->campo2] = '\0';
    int tamanho = 105 - 8 - regd->campo1 - regd->campo2; // Tamanho do lixo nos campos variáveis
    fread(lixo, sizeof(char), tamanho, fpG);
    fread(&regd->idNascimento, sizeof(int), 1, fpG);
    fread(&regd->idadeMae, sizeof(int), 1, fpG);
    fread(regd->dataNascimento, sizeof(char), 10, fpG);
    fread(&regd->sexoBebe, sizeof(char), 1, fpG);
    fread(regd->estadoMae, sizeof(char), 2, fpG);
    fread(regd->estadoBebe, sizeof(char), 2, fpG);
}

/**
 * Função para imprimir os dados do registro
 * 
 * @param regd: Ponteiro para struct de dados
*/
void imprime_registro(registro_dados *regd)
{
    // Variável auxiliar para formatar sexo do bebe
    char sexoBebe[10];

    // Se arquivo não estiver removido
    if (regd->campo1 != -1)
    {
        // Formatação dos dados
        if (regd->cidadeBebe[0] == '\0')
            strcpy(regd->cidadeBebe, "-");
        if (regd->estadoBebe[0] == '\0')
            strcpy(regd->estadoBebe, "-");
        if (regd->dataNascimento[0] == '\0')
            strcpy(regd->dataNascimento, "-");

        switch (regd->sexoBebe)
        {
        case '1':
            strcpy(sexoBebe, "MASCULINO");
            break;
        case '2':
            strcpy(sexoBebe, "FEMININO");
            break;
        case '0':
        default:
            strcpy(sexoBebe, "IGNORADO");
            break;
        }

        // Impressão dos dados formatados
        printf("Nasceu em %s/%s, em %s, um bebê de sexo %s.\n", regd->cidadeBebe, regd->estadoBebe, regd->dataNascimento, sexoBebe);
    }
}

/**
 * Função para buscar registros de acordo com um critério de busca simples ou combinada definido pelo usuário
 * 
 * @param fpG: Ponteiro para arquivo a ser lido 
 * @return RRN: Array de RRNs dos registros que seguem os critérios
*/
int *busca_combinada(FILE *fpG)
{
    fseek(fpG, 5, SEEK_SET);

    // Número de registros no arquivo para alocar o *RRN
    int numreg;
    fread(&numreg, sizeof(int), 1, fpG);

    int *RRN = malloc(sizeof(int) * numreg);

    // Aloca dinamicamente uma struct para o registro de dados
    registro_dados *regd = (registro_dados *)malloc(sizeof(registro_dados));

    // Confere se alocação foi realizada com sucesso
    if (regd == NULL)
    {
        printf("Falha no processamento do arquivo.");
        free(RRN);
        return NULL;
    }

    // Inicialização de variáveis auxiliares
    char cidadeMae[105];
    char cidadeBebe[105];
    int idNascimento = -1;
    int idadeMae = -1;
    char dataNascimento[11];
    char sexoBebe = '3';
    char estadoMae[3];
    char estadoBebe[3];
    cidadeMae[0] = '\0';
    cidadeBebe[0] = '\0';
    dataNascimento[0] = '\0';
    estadoMae[0] = '\0';
    estadoBebe[0] = '\0';

    // Leitura do número de campos utilizados na busca
    int m;
    scanf("%d", &m);

    for (int i = 0; i < m; i++)
    {
        char nomecampo[20];
        char valor[128];
        int valorint;

        // Leitura do campo a ser buscado
        scanf("%s", nomecampo);

        // Leitura do valor a ser buscado no campo definido
        if (!strcmp(nomecampo, "cidadeMae"))
        {
            scanf(" \"%[^\"]\"", valor);
            strcpy(cidadeMae, valor);
        }
        else if (!strcmp(nomecampo, "cidadeBebe"))
        {
            scanf(" \"%[^\"]\"", valor);
            strcpy(cidadeBebe, valor);
        }
        else if (!strcmp(nomecampo, "idNascimento"))
        {
            scanf("%d", &valorint);
            idNascimento = valorint;
        }
        else if (!strcmp(nomecampo, "idadeMae"))
        {
            scanf("%d", &valorint);
            idadeMae = valorint;
        }
        else if (!strcmp(nomecampo, "dataNascimento"))
        {
            scanf(" \"%[^\"]\"", valor);
            strcpy(dataNascimento, valor);
        }
        else if (!strcmp(nomecampo, "sexoBebe"))
        {
            scanf(" \"%[^\"]\"", valor);
            sexoBebe = valor[0];
        }
        else if (!strcmp(nomecampo, "estadoMae"))
        {
            scanf(" \"%[^\"]\"", valor);
            strcpy(estadoBebe, valor);
        }
        else if (!strcmp(nomecampo, "estadoBebe"))
        {
            scanf(" \"%[^\"]\"", valor);
            strcpy(estadoBebe, valor);
        }
    }

    fseek(fpG, 128, SEEK_SET);

    // Contador de registros que não foram removidos
    int cnt_rem = 0;

    // Ler dados até o final do arquivo
    char teste;
    int i = -1;
    while (fread(&teste, 1, 1, fpG) != 0)
    {
        fseek(fpG, -1, SEEK_CUR);
        i++;

        // Leitura de um registro
        recupera_registro(fpG, regd);

        // Checa se campo é um dos pedidos na busca
        if (cidadeMae[0] != '\0')
        {
            // Checa se o campo tem o valor pedido -> se não tem pula para o próximo registro
            if (strcmp(cidadeMae, regd->cidadeMae))
                continue;
        }
        if (cidadeBebe[0] != '\0')
        {
            if (strcmp(cidadeBebe, regd->cidadeBebe))
                continue;
        }
        if (idNascimento != -1)
        {
            if (idNascimento != regd->idNascimento)
                continue;
        }
        if (idadeMae != -1)
        {
            if (idadeMae != regd->idadeMae)
                continue;
        }
        if (dataNascimento[0] != '\0')
        {
            if (strcmp(dataNascimento, regd->dataNascimento))
                continue;
        }
        if (sexoBebe != '3')
        {
            if (sexoBebe != regd->sexoBebe)
                continue;
        }
        if (estadoMae[0] != '\0')
        {
            if (strcmp(estadoMae, regd->estadoMae))
                continue;
        }
        if (estadoBebe[0] != '\0')
        {
            if (strcmp(estadoBebe, regd->estadoBebe))
                continue;
        }

        // Chegou até o final então campo bateu com os critérios buscados
        // Se não estiver excluído adiciona no array de RRNs a ser retornado
        if (regd->campo1 != -1)
        {
            RRN[cnt_rem] = i;
            cnt_rem++;
        }
    }

    // Se nenhum registro foi lido
    if (cnt_rem == 0)
        return NULL;

    // Coloca marcador de final de valores válidos no array
    RRN[cnt_rem] = -1;

    // Libera espaço alocado dinamicamente na memória e retorna array de RRNs
    free(regd);
    return RRN;
}

/**
 * Função para inserir registros novos com os dados contidos no regd recebido como parâmetro
 * 
 * @param fpG: Ponteiro para arquivo a ser lido 
 * @param regd: Ponteiro para struct de dados
*/
void insere(FILE *fpG, registro_dados *regd)
{
    // Trata a parte de lixo
    int tamanholixo = 105 - strlen(regd->cidadeMae) - strlen(regd->cidadeBebe) - 8;
    char *lixo = (char *)malloc(sizeof(char) * tamanholixo);
    for (int i = 0; i < tamanholixo; i++)
        lixo[i] = '$';

    // Caso o sexo do bebê seja um campo nulo
    if (regd->sexoBebe == '\0')
        regd->sexoBebe = '0';

    // Pega tamanho dos campos variáveis
    int tamanho1 = strlen(regd->cidadeMae);
    int tamanho2 = strlen(regd->cidadeBebe);

    // Escrita dos dados
    fwrite(&tamanho1, sizeof(int), 1, fpG);
    fwrite(&tamanho2, sizeof(int), 1, fpG);
    fwrite(regd->cidadeMae, sizeof(char), strlen(regd->cidadeMae), fpG);
    fwrite(regd->cidadeBebe, sizeof(char), strlen(regd->cidadeBebe), fpG);
    fwrite(lixo, sizeof(char), tamanholixo, fpG);
    fwrite(&regd->idNascimento, sizeof(int), 1, fpG);
    fwrite(&regd->idadeMae, sizeof(int), 1, fpG);
    fwrite(&regd->dataNascimento, sizeof(char), 10, fpG);
    fwrite(&regd->sexoBebe, sizeof(char), 1, fpG);
    fwrite(&regd->estadoMae, sizeof(char), 2, fpG);
    fwrite(&regd->estadoBebe, sizeof(char), 2, fpG);
    free(lixo);
}

/**
 * Funcionalidade 1 
 *  
 * Leitura de vários registros obtidos a partir de um arquivo de entrada (fpE)
 * e gravação em um arquivo de dados de saída (fpG)
 * 
 * @param fpE: Ponteiro para arquivo de entrada
 * @param fpG: Ponteiro para arquivo gerado
 * @param regc: Ponteiro para struct do cabeçalho
 */
void leitura_registros(FILE *fpE, FILE *fpG, registro_cabecalho *regc)
{
    // Aloca dinamicamente uma struct para o registro de dados
    registro_dados *regd = (registro_dados *)malloc(sizeof(registro_dados));

    // Confere se alocação foi realizada com sucesso
    if (regd == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Lê primeira linha com títulos dos campos
    char primeiraLinha[128];
    fgets(primeiraLinha, 128, fpE);

    // Move ponteiro para local de inserção do próximo registro
    fseek(fpG, (regc->RRNproxRegistro + 1) * 128, SEEK_SET);

    // Checa se não está no final do arquivo lendo um byte
    char testa;
    while (fread(&testa, 1, 1, fpE) != 0)
    {
        // Volta byte lido para teste
        fseek(fpE, -1, SEEK_CUR);

        // Formatando dados para caso leitura seja nula
        strcpy(regd->cidadeMae, "");
        strcpy(regd->cidadeBebe, "");
        strcpy(regd->dataNascimento, "$$$$$$$$$$");
        strcpy(regd->dataNascimento, "\0");
        strcpy(regd->estadoMae, "$$");
        strcpy(regd->estadoMae, "\0");
        strcpy(regd->estadoBebe, "$$");
        strcpy(regd->estadoBebe, "\0");
        regd->idadeMae = -1;
        regd->sexoBebe = '0';

        // Leitura de dados
        fscanf(fpE, "%[^,]", regd->cidadeMae);
        getc(fpE); // Leitura da vírgula
        fscanf(fpE, "%[^,]", regd->cidadeBebe);
        getc(fpE);
        fscanf(fpE, "%d", &regd->idNascimento);
        getc(fpE);
        fscanf(fpE, "%d", &regd->idadeMae);
        getc(fpE);
        fscanf(fpE, "%[^,]", regd->dataNascimento);
        getc(fpE);
        fscanf(fpE, "%[^,]", &regd->sexoBebe);
        getc(fpE);
        fscanf(fpE, "%[^,]", regd->estadoMae);
        getc(fpE);
        fscanf(fpE, "%[^\n]", regd->estadoBebe);
        getc(fpE);

        insere(fpG, regd);

        // Atualiza dados do cabeçalho
        regc->RRNproxRegistro++;
        regc->numeroRegistrosInseridos++;
    }

    // Atualizar cabeçalho
    atualiza_cabecalho(fpG, regc, 0);
    free(regd);
}

/**
 * Funcionalidade 2
 * 
 * Recuperação dos dados armazenados no arquivo de dados mostrando-os de forma 
 * organizada na saída padrão para permitir distinção de campos e registros
 * 
 * @param fpG: Ponteiro para arquivo a ser lido 
*/
void recupera_dados(FILE *fpG)
{
    // Move ponteiro para o começo do arquivo
    fseek(fpG, 128, SEEK_SET);

    // Aloca dinamicamente uma struct para o registro de dados
    registro_dados *regd = (registro_dados *)malloc(sizeof(registro_dados));

    // Confere se alocação foi realizada com sucesso
    if (regd == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Contador de registros lidos e de registros que não foram removidos
    int cnt = 0;
    int cnt_rem = 0;

    // Variável auxiliar para lixo dos campos variáveis
    char lixo[105];

    // Lê e imprime registros até o final do arquivo
    char teste;
    while (fread(&teste, 1, 1, fpG) != 0)
    {
        fseek(fpG, -1, SEEK_CUR);
        cnt++;

        recupera_registro(fpG, regd);

        // Checa se não foi excluído
        if (regd->campo1 != -1)
        {
            imprime_registro(regd);
            cnt_rem++;
        }
    }

    // Se nenhum registro foi lido ou se todos estão removidos
    if (cnt == 0 || cnt_rem == 0)
    {
        printf("Registro Inexistente.");
        return;
    }

    free(regd);
}

/**
 * Funcionalidade 3
 * 
 * Recuperação dos dados de todos os registros de acordo com um critério de busca
 * 
 * @param fpG: Ponteiro para arquivo gerado 
*/
void buscar_registros(FILE *fpG)
{
    // Armazena array de registros encontrados de acordo com a busca
    int *RRN = busca_combinada(fpG);
    if (RRN == NULL)
    {
        printf("Registro Inexistente.");
        return;
    }

    // Aloca dinamicamente uma struct para o registro de dados
    registro_dados *regd = (registro_dados *)malloc(sizeof(registro_dados));

    int i = 0;

    // Mostra todos os registros buscados
    while (RRN[i] != -1)
    {
        fseek(fpG, 128 * (RRN[i] + 1), SEEK_SET);
        recupera_registro(fpG, regd);
        imprime_registro(regd);
        i++;
    }

    // Libera memória alocada dinamicamente
    free(regd);
    free(RRN);
}

/**
 * Funcionalidade 4
 * 
 * Recuperação dos dados de um registro de acordo com o RRN fornecido
 * 
 * @param fpG: Ponteiro para arquivo gerado 
*/
void buscaRRN(FILE *fpG)
{
    // Aloca dinamicamente uma struct para o registro de dados
    registro_dados *regd = (registro_dados *)malloc(sizeof(registro_dados));

    // Confere se alocação foi realizada com sucesso
    if (regd == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Recebe valor do RRN
    int RRN;
    scanf("%d", &RRN);

    // Recupera e mostra registro caso não excluído
    fseek(fpG, 128 * (RRN + 1), SEEK_SET);
    recupera_registro(fpG, regd);

    if (regd->campo1 == -1)
        printf("Registro Inexistente.");
    else
        imprime_registro(regd);

    // Libera memória alocada dinamicamente
    free(regd);
}

/**
 * Funcionalidade 5
 * 
 * N remoções lógicas nos registros baseadas em critérios de busca simples ou combinada
 * 
 * @param fpG: Ponteiro para arquivo gerado 
 * @param regc: Ponteiro para a estrutura de cabeçalho
*/
void remove_registros(FILE *fpG, registro_cabecalho *regc)
{
    // Deixa status como 0 no cabeçalho
    regc->status = '0';
    atualiza_cabecalho(fpG, regc, 0);

    // Aloca dinamicamente uma struct para o registro de dados
    registro_dados *regd = (registro_dados *)malloc(sizeof(registro_dados));

    // Confere se alocação foi realizada com sucesso
    if (regd == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Leitura do número de buscas a serem feitas
    int n;
    scanf("%d", &n);

    // Marcador a ser inserido nos registros removidos
    int removido = -1;

    for (int i = 0; i < n; i++)
    {
        int *RRN = busca_combinada(fpG);

        if (RRN == NULL)
            continue;

        int j = 0;

        // Remove todos os registros armazenados no array de RRNs
        while (RRN[j] != -1)
        {
            fseek(fpG, 128 * (RRN[j] + 1), SEEK_SET);
            fwrite(&removido, sizeof(int), 1, fpG);

            // Atualiza números no cabeçalho
            regc->numeroRegistrosRemovidos++;
            regc->numeroRegistrosInseridos--;
            j++;
        }
    }

    // Atualiza o status para 1
    regc->status = '1';
    atualiza_cabecalho(fpG, regc, 0);

    free(regd);
}

/**
 * Funcionalidade 6
 * 
 * Inserção de registros adicionais, baseado na abordagem estática de registros logicamente removidos.
 * 
 * @param fpG: Ponteiro para arquivo gerado 
 * @param regc: Ponteiro para a estrutura de cabeçalho
*/
void insere_registro(FILE *fpG, registro_cabecalho *regc)
{
    // Deixa status como 0 no cabeçalho
    regc->status = '0';
    atualiza_cabecalho(fpG, regc, 0);

    // Aloca dinamicamente uma struct para o registro de dados
    registro_dados *regd = (registro_dados *)malloc(sizeof(registro_dados));

    // Confere se alocação foi realizada com sucesso
    if (regd == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Leitura do número de registros a serem inseridos
    int n;
    scanf("%d", &n);

    // Declaração de variáveis auxiliares para leitura de inteiros
    char strIdadeMae[4];
    char strIdNascimento[4];

    for (int i = 0; i < n; i++)
    {
        // Leitura dos dados a serem inseridos

        scan_quote_string(regd->cidadeMae);

        scan_quote_string(regd->cidadeBebe);

        scan_quote_string(strIdNascimento);
        // Define valor como -1 caso valor lido tenha sido nulo
        if (!strcmp(strIdNascimento, ""))
            regd->idNascimento = -1;
        else
            regd->idNascimento = atoi(strIdNascimento);

        scan_quote_string(strIdadeMae);
        // Define valor como -1 caso valor lido tenha sido nulo
        if (!strcmp(strIdadeMae, ""))
            regd->idadeMae = -1;
        else
            regd->idadeMae = atoi(strIdadeMae);

        scan_quote_string(regd->dataNascimento);
        // Caso valor lido tenha sido nulo insere $ no lugar do lixo
        if (!strcmp(regd->dataNascimento, ""))
        {
            strcpy(regd->dataNascimento, "$$$$$$$$$$");
            regd->dataNascimento[0] = '\0';
        }

        scan_quote_string(&regd->sexoBebe);

        scan_quote_string(regd->estadoMae);
        // Caso valor lido tenha sido nulo insere $ no lugar do lixo
        if (!strcmp(regd->estadoMae, ""))
        {
            strcpy(regd->estadoMae, "$$");
            regd->estadoMae[0] = '\0';
        }

        scan_quote_string(regd->estadoBebe);
        // Caso valor lido tenha sido nulo insere $ no lugar do lixo
        if (!strcmp(regd->estadoBebe, ""))
        {
            strcpy(regd->estadoBebe, "$$");
            regd->estadoBebe[0] = '\0';
        }

        // Insere no final do arquivo
        fseek(fpG, 0, SEEK_END);
        insere(fpG, regd);

        // Atualiza números do cabeçalho
        regc->numeroRegistrosInseridos++;
        regc->RRNproxRegistro++;
    }

    // Atualiza o status para 1
    regc->status = '1';
    atualiza_cabecalho(fpG, regc, 0);
}

/**
 * Funcionalidade 7
 * 
 * Atualização de um ou mais campos de um ou mais registros identificados por seu RRN
 * 
 * @param fpG: Ponteiro para arquivo gerado 
 * @param regc: Ponteiro para a estrutura de cabeçalho
*/
void atualiza_registro(FILE *fpG, registro_cabecalho *regc)
{
    // Deixa status como 0 no cabeçalho
    regc->status = '0';
    atualiza_cabecalho(fpG, regc, 0);

    // Aloca dinamicamente uma struct para o registro de dados
    registro_dados *regd = (registro_dados *)malloc(sizeof(registro_dados));

    // Confere se alocação foi realizada com sucesso
    if (regd == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Leitura do número de registros a serem atualizados
    int n;
    scanf("%d", &n);

    for (int i = 0; i < n; i++)
    {
        // Recebe valor do RRN
        int RRN;
        scanf("%d", &RRN);

        // Recupera registro
        int start = 128 * (RRN + 1);
        fseek(fpG, start, SEEK_SET);
        recupera_registro(fpG, regd);

        // Leitura do número de campos a serem atualizados
        int m;
        scanf("%d", &m);

        for (int j = 0; j < m; j++)
        {
            char nomecampo[20];
            char valor[128];
            int valorint;

            // Leitura do campo a ser atualizado
            scanf("%s", nomecampo);

            // Checa se é o campo a ser atualizado
            if (!strcmp(nomecampo, "cidadeMae"))
            {
                // Leitura do valor
                scan_quote_string(valor);
                if (regd->campo1 == -1)
                    continue;
                strcpy(regd->cidadeMae, valor);

                // Escrita do tamanho do campo cidadeMae em campo1
                fseek(fpG, start, SEEK_SET);
                regd->campo1 = strlen(valor);
                fwrite(&regd->campo1, sizeof(int), 1, fpG);

                // Junta valor das cidades Mae e Bebe e escreve no arquivo
                fseek(fpG, 4, SEEK_CUR);
                char cidades[105];
                strcpy(cidades, valor);
                strcat(cidades, regd->cidadeBebe);
                fwrite(cidades, sizeof(char), regd->campo1 + regd->campo2, fpG);
            }
            else if (!strcmp(nomecampo, "cidadeBebe"))
            {
                // Leitura do valor
                scan_quote_string(valor);
                if (regd->campo1 == -1)
                    continue;
                strcpy(regd->cidadeBebe, valor);

                // Escrita do tamanho do campo cidadeBebe em campo2
                fseek(fpG, start + 4, SEEK_SET);
                regd->campo2 = strlen(valor);
                fwrite(&regd->campo2, sizeof(int), 1, fpG);

                // Escreve valor da cidade Bebe no arquivo
                fseek(fpG, regd->campo1, SEEK_CUR);
                fwrite(valor, sizeof(char), regd->campo2, fpG);
            }
            else if (!strcmp(nomecampo, "idNascimento"))
            {
                // Leitura do valor
                scan_quote_string(valor);
                if (regd->campo1 == -1)
                    continue;
                valorint = atoi(valor);

                // Escreve valor no arquivo
                fseek(fpG, start + 105, SEEK_SET);
                fwrite(&valorint, sizeof(int), 1, fpG);
            }
            else if (!strcmp(nomecampo, "idadeMae"))
            {
                // Leitura do valor
                scan_quote_string(valor);
                if (regd->campo1 == -1)
                    continue;

                // Caso valor seja nulo define como -1
                if (!strcmp(valor, ""))
                    valorint = -1;
                else
                    valorint = atoi(valor);

                // Escreve valor no arquivo
                fseek(fpG, start + 109, SEEK_SET);
                fwrite(&valorint, sizeof(int), 1, fpG);
            }
            else if (!strcmp(nomecampo, "dataNascimento"))
            {
                // Leitura do valor
                scan_quote_string(valor);
                if (regd->campo1 == -1)
                    continue;

                // Caso valor seja nulo escreve $ no lugar do lixo
                if (!strcmp(valor, ""))
                {
                    strcpy(valor, "$$$$$$$$$$");
                    valor[0] = '\0';
                }

                // Escreve valor no arquivo
                fseek(fpG, start + 113, SEEK_SET);
                fwrite(valor, sizeof(char), 10, fpG);
            }
            else if (!strcmp(nomecampo, "sexoBebe"))
            {
                // Leitura do valor
                scan_quote_string(valor);
                if (regd->campo1 == -1)
                    continue;

                // Escreve valor no arquivo
                fseek(fpG, start + 123, SEEK_SET);
                fwrite(valor, sizeof(char), 1, fpG);
            }
            else if (!strcmp(nomecampo, "estadoMae"))
            {
                // Leitura do valor
                scan_quote_string(valor);
                if (regd->campo1 == -1)
                    continue;

                // Caso valor seja nulo escreve $ no lugar do lixo
                if (!strcmp(valor, ""))
                {
                    strcpy(valor, "$$");
                    valor[0] = '\0';
                }

                // Escreve valor no arquivo
                fseek(fpG, start + 124, SEEK_SET);
                fwrite(valor, sizeof(char), 2, fpG);
            }
            else if (!strcmp(nomecampo, "estadoBebe"))
            {
                // Leitura do valor
                scan_quote_string(valor);
                if (regd->campo1 == -1)
                    continue;

                // Caso valor seja nulo escreve $ no lugar do lixo
                if (!strcmp(valor, ""))
                {
                    strcpy(valor, "$$");
                    valor[0] = '\0';
                }

                // Escreve valor no arquivo
                fseek(fpG, start + 126, SEEK_SET);
                fwrite(valor, sizeof(char), 2, fpG);
            }
        }

        // Se campo não foi excluído atualiza número do cabeçalho
        if (regd->campo1 != -1)
            regc->numeroRegistrosAtualizados++;
    }

    // Atualiza o status para 1
    regc->status = '1';
    atualiza_cabecalho(fpG, regc, 0);

    free(regd);
}

int main()
{
    // Nomes dos arquivos
    char arquivoEntrada[100];
    char arquivoGerado[100];
    char arquivoIndiceIdNascimento[100];

    // Opção de funcionalidade a executar
    int opc;
    scanf("%d", &opc);

    // Ponteiros para arquivos
    FILE *fpE;
    FILE *fpG;
    FILE *fpI;

    // Aloca dinamicamente uma struct dos registros do cabeçalho
    registro_cabecalho *regc = (registro_cabecalho *)malloc(sizeof(registro_cabecalho));

    if (regc == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return 0;
    }

    // Inicializa os dados do cabeçalho
    regc->status = '0';
    regc->RRNproxRegistro = 0;
    regc->numeroRegistrosInseridos = 0;
    regc->numeroRegistrosRemovidos = 0;

    // Funcionalidades
    if (opc == 1)
    {
        // Leitura dos nomes dos arquivos
        scanf("%s", arquivoEntrada);
        scanf("%s", arquivoGerado);

        // Abertura dos arquivos
        fpE = fopen(arquivoEntrada, "r");
        fpG = fopen(arquivoGerado, "w+b");
        if (fpE == NULL || fpG == NULL)
        {
            printf("Falha no processamento do arquivo.");
            return 0;
        }

        // Chama funcionalidade 1
        leitura_registros(fpE, fpG, regc);

        regc->status = '1';
        atualiza_cabecalho(fpG, regc, 1);

        // Fecha arquivos
        fclose(fpG);
        fclose(fpE);

        binarioNaTela(arquivoGerado);
    }
    else if (opc >= 2 && opc <= 7)
    {
        // Leitura dos nomes do arquivo
        scanf("%s", arquivoGerado);

        // Abertura do arquivo
        fpG = fopen(arquivoGerado, "rb+");
        if (fpG == NULL)
        {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }

        // Checagem de consistência dos dados pelo status
        fread(&regc->status, sizeof(char), 1, fpG);

        // Caso arquivo esteja inconsistente
        if (regc->status == '0')
        {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }

        // Obtenção dos dados do cabeçalho
        fread(&regc->RRNproxRegistro, sizeof(int), 1, fpG);
        fread(&regc->numeroRegistrosInseridos, sizeof(int), 1, fpG);
        fread(&regc->numeroRegistrosRemovidos, sizeof(int), 1, fpG);
        fread(&regc->numeroRegistrosAtualizados, sizeof(int), 1, fpG);

        fseek(fpG, 0, SEEK_SET);

        // Chama funcionalidade
        switch (opc)
        {
        case 2:
            recupera_dados(fpG);
            break;
        case 3:
            buscar_registros(fpG);
            break;
        case 4:
            buscaRRN(fpG);
            break;
        case 5:
            remove_registros(fpG, regc);
            break;
        case 6:
            insere_registro(fpG, regc);
            break;
        case 7:
            atualiza_registro(fpG, regc);
            break;
        }

        // Fecha arquivo
        fclose(fpG);

        // Chama binarioNaTela para funcionalidades necessárias
        if (opc > 4 && opc < 8)
            binarioNaTela(arquivoGerado);
    }
    else if(opc == 8)
    {
        // Leitura dos nomes dos arquivos
        scanf("%s", arquivoEntrada);
        scanf("%s", arquivoIndiceIdNascimento);

        // Abertura dos arquivos
        fpE = fopen(arquivoEntrada, "r");
        fpI = fopen(arquivoIndiceIdNascimento, "w+b");
        if (fpE == NULL || fpI == NULL)
        {
            printf("Falha no processamento do arquivo.");
            return 0;
        }

        // Chama funcionalidade aqui
        // Muda status pra 0 tb

        // Fecha arquivos
        fclose(fpI);
        fclose(fpE);

        binarioNaTela(arquivoGerado);
    }

    // Libera espaço alocado dinâmicamente na memória
    free(regc);

    return 0;
}