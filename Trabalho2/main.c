/**
 * Trabalho 1 - Organização de Arquivos
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
 * @param regc: Ponteiro para struct de dados
*/
void recupera_registro(FILE *fpG, registro_dados *regd)
{
    // Variável auxiliar para lixo dos campos variáveis
    char lixo[105];

    // Leitura dos dados
    fread(&regd->campo1, sizeof(int), 1, fpG);
    fread(&regd->campo2, sizeof(int), 1, fpG);
    fread(regd->cidadeMae, sizeof(char), regd->campo1, fpG);
    fread(regd->cidadeBebe, sizeof(char), regd->campo2, fpG);
    regd->cidadeBebe[regd->campo2] = '\0'; // Insere '\0' no final da string de tamanho variável
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
 * @param regc: Ponteiro para struct do cabeçalho
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
    if(regd == NULL)
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
        strcpy(regd->cidadeMae,"");
        strcpy(regd->cidadeBebe,"");
        strcpy(regd->dataNascimento, "$$$$$$$$$$");
        strcpy(regd->dataNascimento, "\0");
        strcpy(regd->estadoMae, "$$");
        strcpy(regd->estadoMae, "\0");
        strcpy(regd->estadoBebe, "$$");
        strcpy(regd->estadoBebe, "\0");
        regd->idadeMae = -1;
        regd->sexoBebe = '0';

        // Leitura de dados
        fscanf(fpE, "%[^,]",regd->cidadeMae);
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

        // Tratando a parte de lixo
        int tamanholixo = 105 - strlen(regd->cidadeMae) - strlen(regd->cidadeBebe) - 8;
        char *lixo = (char *)malloc(sizeof(char) * tamanholixo);
        for (int i = 0; i < tamanholixo; i++)
            lixo[i] = '$';

        // Caso o sexo do bebê seja um campo nulo
        if(regd->sexoBebe == '\0') regd->sexoBebe = '0';
        
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

        // Atualiza dados do cabeçalho
        regc->RRNproxRegistro++;
        regc->numeroRegistrosInseridos++;
    }

    // Atualizar cabeçalho
    atualiza_cabecalho(fpG, regc, 0);
    free(regd);
}

/**
 *  Funcionalidade 2
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
    if(regd == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Contador de registros lidos e de registros que não foram removidos
    int cnt = 0;
    int cnt_rem = 0;

    // Variável auxiliar para lixo dos campos variáveis
    char lixo[105];

    // Ler dados até o final do arquivo
    char teste;
    while (fread(&teste, 1, 1, fpG) != 0)
    {
        fseek(fpG, -1, SEEK_CUR);
        cnt++;

        recupera_registro(fpG, regd);
        if (regd->campo1 != -1)
        {
            imprime_registro(regd);
            cnt_rem++;
        }
    }

    // Se nenhum registro foi lido ou se todos estão removidos
    if(cnt == 0 || cnt_rem == 0)
    {
        printf("Registro inexistente.");
        return;
    }

    free(regd);
}

/**
 *  Funcionalidade 3
 * 
 * Recuperação dos dados de todos os registros de acordo com um critério de busca 
 * simples ou combinada determinada pelo usuário
 * 
 * @param fpG: Ponteiro para arquivo gerado 
*/
void busca(FILE *fpG)
{
    // Aloca dinamicamente uma struct para o registro de dados
    registro_dados *regd = (registro_dados *)malloc(sizeof(registro_dados));

    // Confere se alocação foi realizada com sucesso
    if(regd == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }

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
    int m;

    scanf("%d", &m);

    for(int i = 0; i < m; i++)
    {
        char nomecampo[20];
        char valor[128];
        scanf("%s", nomecampo);
        scanf("%s", valor);

        if(!strcmp(nomecampo, "cidadeMae"))
            strcpy(cidadeMae, valor);
        
        if(!strcmp(nomecampo,"cidadeBebe"))
            strcpy(cidadeBebe, valor);
        
        if(!strcmp(nomecampo, "idNascimento"))
            idNascimento = atoi(valor);
        
        if(!strcmp(nomecampo,"idadeMae"))
            idadeMae = atoi(valor);
            
        if(!strcmp(nomecampo, "dataNascimento"))
            strcpy(dataNascimento, valor);
        
        if(!strcmp(nomecampo,"sexoBebe"))
            sexoBebe = valor[0];
        
        if(!strcmp(nomecampo, "estadoMae"))
            strcpy(estadoBebe, valor);
        
        if(!strcmp(nomecampo,"estadoBebe"))
            strcpy(estadoBebe, valor);   
    }
    
    fseek(fpG, 128, SEEK_SET);

    // Contador de registros que não foram removidos
    int cnt_rem = 0;

    // Ler dados até o final do arquivo
    char teste;
    while (fread(&teste, 1, 1, fpG) != 0)
    {
        fseek(fpG, -1, SEEK_CUR);
        
        recupera_registro(fpG, regd);
        
        if(cidadeMae[0] != '\0')
        {
            if(strcmp(cidadeMae, regd->cidadeMae)) 
                continue;
        }
        if(cidadeBebe[0] != '\0')
        {
            if(strcmp(cidadeBebe, regd->cidadeBebe)) 
                continue;
        }
        if(idNascimento != -1)
        {
            if(idNascimento != regd->idNascimento) 
                continue;
        }
        if(idadeMae != -1)
        {
            if(idadeMae != regd->idadeMae)
                continue;
        }
        if(dataNascimento[0] != '\0')
        {
            if(strcmp(dataNascimento, regd->dataNascimento)) 
                continue;
        }
        if(sexoBebe != '3')
        {
            if(sexoBebe != regd->sexoBebe) 
                continue;
        }
        if(estadoMae[0] != '\0')
        {
            if(strcmp(estadoMae, regd->estadoMae)) 
                continue;
        }
        if(estadoBebe[0] != '\0')
        {
            if(strcmp(estadoBebe, regd->estadoBebe))
                continue;
        }

        if (regd->campo1 != -1)
        {
            imprime_registro(regd);
            cnt_rem++;
        }
    }

    // Se nenhum registro foi lido
    if(cnt_rem == 0)
    {
        printf("Registro inexistente.");
        return;
    }
    
    free(regd);
}


/**
 *  Funcionalidade 4
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
    if(regd == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return;
    }
    
    // Recebe valor do RRN
    int RRN;
    scanf("%d", &RRN);

    fseek(fpG, 128 * (RRN + 1), SEEK_SET);

    recupera_registro(fpG, regd);
    
    if (regd->campo1 == -1)
        printf("Registro inexistente.");
    else
        imprime_registro(regd);

    free(regd);
}

int main()
{
    // Nomes dos arquivos
    char arquivoEntrada[100];
    char arquivoGerado[100];

    // Opção de funcionalidade a executar
    int opc;
    scanf("%d", &opc);

    // Ponteiros para arquivos
    FILE *fpE;
    FILE *fpG;

    // Aloca dinamicamente uma struct dos registros do cabeçalho
    registro_cabecalho *regc = (registro_cabecalho *)malloc(sizeof(registro_cabecalho));
    
    if(regc == NULL)
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
        if(fpE == NULL || fpG == NULL)
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
    else if (opc >= 2 && opc <= 4)
    {
        // Leitura dos nomes do arquivo
        scanf("%s", arquivoGerado);

        // Abertura do arquivo
        fpG = fopen(arquivoGerado, "rb");
        if(fpG == NULL)
        {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }

        // Checagem de consistência dos dados pelo status
        char status;
        fread(&status, sizeof(char), 1, fpG);

        // Caso arquivo esteja inconsistente
        if(status == '0')
        {
            printf("Falha no processamento do arquivo.\n");
            return 0;
        }

        fseek(fpG, 0, SEEK_SET);

        // Chama funcionalidade
        switch (opc)
        {
            case 2:
                recupera_dados(fpG);
                break;
            case 3:
                busca(fpG);
                break;
            case 4:
                buscaRRN(fpG);
                break;
        }
        
        // Fecha arquivo
        fclose(fpG);
    }


    // Libera espaço alocado dinâmicamente na memória
    free(regc);

    return 0;
}
