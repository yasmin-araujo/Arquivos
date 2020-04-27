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

typedef struct _registro_cabecalho
{
    char status;            // Consistência do arquivo de dados
    int RRNproxRegistro;    // RRN do próximo registro a ser inserido
    int numeroRegistrosInseridos;
    int numeroRegistrosRemovidos;
    int numeroRegistrosAtualizados;
    // char lixo[111];
} registro_cabecalho;

typedef struct _registro_dados
{
    int campo1;
    int campo2;
    char *cidadeMae;
    char *cidadeBebe;
    int idNascimento;
    int idadeMae;
    char dataNascimento[10];
    char sexoBebe;
    char estadoMae[2];
    char estadoBebe[2];
} registro_dados;

/**
 *  Funcionalidade 1 
 *  
 *  Leitura de vários registros obtidos a partir de um arquivo de entrada -> fpE
 *  e gravação em um arquivo de dados de saída -> fpG
 */
void leitura_registros(FILE *fpE, FILE *fpG, registro_cabecalho *regc)
{
    // fread(onde armazenar, sizeof , vezes quatos, fp)
    // fwrite(de onde ler , sizeof, vezes quantos, fp)
    int size_campo1;
    registro_dados *regd = (registro_dados *)malloc(sizeof(registro_dados));
    while (fpE != EOF)
    {
        // Leitura de dados

        // DUVIDAS DO MAL
        // tem variavel lixo? no cabeçalho -> não , é só escrever $ mesmo
        // fread em dados variaveis -> usar fscanf com mascara %[^,]
        // o que fazer com o espaço que sobra -> vai sobrar antes dos dados fixos e deve ser preenchido com $
        // status mudar quando? no final da funcionalidade ou do programa? -> final do programa
        // checar status quando?
        // vai ter registros pra adicionar dps? n importa -> usa o rrn que ta top
        // inicializa os dados do cabeçalho apenas qnd o registro esta sendo criado pela primeira vez?
        // registro inexistente só quando nao tem nenhum no arquivo?
        // como saber se algum erro foi encontrado no processamento?

        fscanf(fpE, "%[^,],", regd->cidadeMae);
        fscanf(fpE, "%[^,],", regd->cidadeBebe);
        fscanf(fpE, "%d,", &regd->idNascimento);
        fscanf(fpE, "%d,", &regd->idadeMae);
        fscanf(fpE, "%[^,],", regd->dataNascimento);
        fscanf(fpE, "%c,", &regd->sexoBebe);
        fscanf(fpE, "%[^,],", regd->estadoMae);
        fscanf(fpE, "%[^,]", regd->estadoBebe);
        //fread(regd->cidadeMae, , sizeof(), fpE);
        // fread(&regd->idNascimento, sizeof(int), 1, fpE);
        // fread(&regd->idadeMae, sizeof(int), 1, fpE);
        // fread(regd->dataNascimento, sizeof(char), 10, fpE);
        // fread(&regd->sexoBebe, sizeof(char), 1, fpE);
        // fread(regd->estadoMae, sizeof(char), 2, fpE);
        // fread(regd->estadoBebe, sizeof(char), 2, fpE);
        // Escrita de dados
        //muitos fwrites...
        size_campo1 = strlen(regd->cidadeMae);
        fwrite(size_campo1, sizeof(int), 1, fpG);
        fwrite(regd->cidadeMae, sizeof(char), size_campo1, fpG);
        fwrite(&regd->idNascimento, sizeof(int), 1, fpG);
        fwrite(&regd->idadeMae, sizeof(int), 1, fpG);
        fwrite(regd->dataNascimento, sizeof(char), 10, fpG);
        fwrite(&regd->sexoBebe, sizeof(char), 1, fpG);
        fwrite(regd->estadoMae, sizeof(char), 2, fpG);
        fwrite(regd->estadoBebe, sizeof(char), 2, fpG);

        regc->RRNproxRegistro++;
        regc->numeroRegistrosInseridos++;
    }

    binarioNaTela(fpG);
}

/**
 *  Funcionalidade 2
 * 
 *  Recuperação dos dados armazenados no arquivo de dados
 *  mostrando os dados de forma organizada na saída padrão para permitir distinção de campos e registros
 *  Tratamento de lixo
 *  Registros marcados como logicamente removidos não deverão ser exibidos
*/
void recupera_dados(FILE *fpG, registro_cabecalho *regc)
{
    // Move ponteiro para o começo do arquivo
    fseek(fpG, 128, SEEK_SET);

    registro_dados *regd = (registro_dados *)malloc(sizeof(registro_dados));
    char sexoBebe[10]; // Variável auxiliar para formatar sexo do bebe

    if (regc->numeroRegistrosInseridos == 0)
    {
        printf("Registro inexistente.");
        return;
    }

    // Ler dados até o final do arquivo
    while (fpG != EOF)
    {
        fread(&regd->campo1, sizeof(int), 1, fpG);
        fread(&regd->campo2, sizeof(int), 1, fpG);
        //dados variaveis aqui
        fread(&regd->idNascimento, sizeof(int), 1, fpG);
        fread(&regd->idadeMae, sizeof(int), 1, fpG);
        fread(regd->dataNascimento, sizeof(char), 10, fpG);
        fread(&regd->sexoBebe, sizeof(char), 1, fpG);
        fread(regd->estadoMae, sizeof(char), 2, fpG);
        fread(regd->estadoBebe, sizeof(char), 2, fpG);

        // Se arquivo não estiver removido
        if (regd->campo1 != -1)
        {
            // Formatação dos dados
            if (regd->cidadeBebe == "")
                strcpy(regd->cidadeBebe, "-");
            if (regd->estadoBebe == "")
                strcpy(regd->estadoBebe, "-");
            if (regd->dataNascimento == "")
                strcpy(regd->dataNascimento, "-");

            switch (regd->sexoBebe)
            {
            case '0':
                strcpy(sexoBebe, "IGNORADO");
                break;
            case '1':
                strcpy(sexoBebe, "MASCULINO");
                break;
            case '2':
                strcpy(sexoBebe, "FEMININO");
                break;
            default:
                strcpy(sexoBebe, "-");
                break;
            }

            // Impressão dos dados formatados
            printf("Nasceu em %s/%s, em %s, um bebe de sexo %s.", regd->cidadeBebe, regd->estadoBebe, regd->dataNascimento, sexoBebe);
        }
    }
}

int main()
{
    char *arquivoEntrada;
    char *arquivoGerado;
    int opc;
    scanf("%d", &opc);
    scanf("%s", arquivoEntrada);
    scanf("%s", arquivoGerado);
    FILE *fpE = fopen(arquivoEntrada, "r");
    FILE *fpG = fopen(arquivoGerado, "w+b");

    registro_cabecalho *regc = (registro_cabecalho *)malloc(sizeof(registro_cabecalho));
    
    // Inicializa os dados do cabeçalho
    // deveria inicializar se não existe o arquivo apenas?
    regc->status = '0';
    regc->RRNproxRegistro = 0;
    regc->numeroRegistrosInseridos = 0;
    regc->numeroRegistrosRemovidos = 0;
    // for (int i = 0; i < 111; i++)
    //     regc->lixo[i] = '$';

    leitura_registros(fpE, fpG, regc);
    recupera_dados(fpG, regc);

    regc->status = '1';
    // escrever status no arquivo fpG

    // Fechar arquivos utilizados
    fclose(fpE);
    fclose(fpG);

    // Libera espaço alocado dinâmicamente na memória
    free(regc);

    return 0;
}
