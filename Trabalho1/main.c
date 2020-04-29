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
    char status;         // Consistência do arquivo de dados
    int RRNproxRegistro; // RRN do próximo registro a ser inserido
    int numeroRegistrosInseridos;
    int numeroRegistrosRemovidos;
    int numeroRegistrosAtualizados;
} registro_cabecalho;

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


void atualiza_cabecalho(FILE *fpG, registro_cabecalho *regc, int insereLixo)
{
    //printf("status: %c, rrnprox: %d, rrnin: %d , rrnrem: %d , regatualiza: %d", regc->status, regc->RRNproxRegistro, regc->numeroRegistrosInseridos, regc->numeroRegistrosInseridos, regc->numeroRegistrosAtualizados);
    fseek(fpG, 0, SEEK_SET); // Vai para o começo do arquivo
    fwrite(&regc->status, sizeof(char), 1, fpG);
    fwrite(&regc->RRNproxRegistro, sizeof(int), 1, fpG);
    fwrite(&regc->numeroRegistrosInseridos, sizeof(int), 1, fpG);
    fwrite(&regc->numeroRegistrosRemovidos, sizeof(int), 1, fpG);
    fwrite(&regc->numeroRegistrosAtualizados, sizeof(int), 1, fpG);
     if (insereLixo) //caso esteja criando o cabeçalho
     {
         char lixo[111];
         for (int i = 0; i < 111; i++)
             lixo[i] = '$';
         fwrite(&lixo, sizeof(char), 111, fpG);
     }

}

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
    registro_dados *regd = (registro_dados *)malloc(sizeof(registro_dados));

    fseek(fpG, (regc->RRNproxRegistro + 1) * 128, SEEK_SET);
    char testa;
    while (fread(&testa, 1, 1, fpE) != 0)
    {
        //i++;
        //printf("a\n\n");
        fseek(fpE, -1, SEEK_CUR);
        //printf("b %d\n\n", regc->RRNproxRegistro);
        // Leitura de dados

        // DUVIDAS DO MAL
        // 1 arquivoEntrada.csv arquivoGerado.bin
        // tem variavel lixo? no cabeçalho -> não , é só escrever $ mesmo
        // fread em dados variaveis -> usar fscanf com mascara %[^,]
        // o que fazer com o espaço que sobra -> vai sobrar antes dos dados fixos e deve ser preenchido com $
        // status mudar quando? no final da funcionalidade ou do programa? -> final do programa
        // checar status quando?
        // vai ter registros pra adicionar dps? n importa -> usa o rrn que ta top
        // inicializa os dados do cabeçalho apenas qnd o registro esta sendo criado pela primeira vez?
        // registro inexistente só quando nao tem nenhum no arquivo?
        // como saber se algum erro foi encontrado no processamento?
        //char cidadeBebe[17];
        fscanf(fpE, "%[^,],",regd->cidadeMae);
        //printf("%s\n\n",regd->cidadeMae);
        fscanf(fpE, "%[^,],", regd->cidadeBebe);
        fscanf(fpE, "%d,", &regd->idNascimento);
        fscanf(fpE, "%d,", &regd->idadeMae);
        fscanf(fpE, "%[^,],", regd->dataNascimento);
        fscanf(fpE, "%c,", &regd->sexoBebe);
        fscanf(fpE, "%[^,],", regd->estadoMae);
        fscanf(fpE, "%[^\n]\n", regd->estadoBebe);
        printf("%s, %s, %d, %d, %s, %c ,%s, %s\n", regd->cidadeMae,regd->cidadeBebe,regd->idNascimento,regd->idadeMae,regd->dataNascimento,regd->sexoBebe,regd->estadoMae,regd->estadoBebe);

        // Escrita de dados

        //tratando a parte de lixo
        int tamanholixo = 105 - strlen(regd->cidadeMae) - strlen(regd->cidadeBebe) - 8;
        char *lixo = (char *)malloc(sizeof(char) * tamanholixo);
        for (int i = 0; i < tamanholixo; i++)
            lixo[i] = '$';

        int tamanho1 = strlen(regd->cidadeMae);
        int tamanho2 = strlen(regd->cidadeBebe);
        fwrite(&tamanho1, sizeof(int), 1, fpG);
        fwrite(&tamanho2, sizeof(int), 1, fpG);
        fwrite(regd->cidadeMae, sizeof(char), strlen(regd->cidadeMae), fpG);
        fwrite(regd->cidadeBebe, sizeof(char), strlen(regd->cidadeBebe), fpG);
        fwrite(&lixo, sizeof(char), tamanholixo, fpG);
        fwrite(&regd->idNascimento, sizeof(int), 1, fpG);
        fwrite(&regd->idadeMae, sizeof(int), 1, fpG);
        fwrite(&regd->dataNascimento, sizeof(char), 10, fpG);
        fwrite(&regd->sexoBebe, sizeof(char), 1, fpG);
        fwrite(&regd->estadoMae, sizeof(char), 2, fpG);
        fwrite(&regd->estadoBebe, sizeof(char), 2, fpG);

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
    char testa;
    while (fread(&testa, 1, 1, fpG) != 0)
    {
        fseek(fpG, -1, SEEK_CUR);
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
            printf("Nasceu em %s/%s, em %s, um bebe de sexo %s.", regd->cidadeBebe, regd->estadoBebe, regd->dataNascimento, sexoBebe);
        }
    }
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

    // Funcionalidades
    if (opc == 1)
    {
        // Leitura dos nomes dos arquivos
        scanf("%s", arquivoEntrada);
        scanf("%s", arquivoGerado);

        // Abertura dos arquivos
        fpE = fopen(arquivoEntrada, "r");
        fpG = fopen(arquivoGerado, "w+b");
    }
    else if (opc == 2)
    {
        // Leitura dos nomes do arquivo
        scanf("%s", arquivoGerado);

        // Abertura do arquivo
        fpG = fopen(arquivoGerado, "w+b");
    }

    // Aloca dinamicamente ua struct dos registros do cabeçalho
    registro_cabecalho *regc = (registro_cabecalho *)malloc(sizeof(registro_cabecalho));

    printf("o arquivo chama: %s\n", arquivoEntrada);

    // Inicializa os dados do cabeçalho
    // deveria inicializar se não existe o arquivo apenas?
    regc->status = '0';
    regc->RRNproxRegistro = 10;
    regc->numeroRegistrosInseridos = 0;
    regc->numeroRegistrosRemovidos = 0;
    //printf("status: %c, rrnprox: %d, rrnin: %d , rrnrem: %d , regatualiza: %d", regc->status, regc->RRNproxRegistro, regc->numeroRegistrosInseridos, regc->numeroRegistrosInseridos, regc->numeroRegistrosAtualizados);

    atualiza_cabecalho(fpG, regc, 1);

    //4444
    if (opc == 1)
    {
        leitura_registros(fpE, fpG, regc);
        printf("\n\nBOTA NA TELAAA\n\n");
        binarioNaTela(arquivoGerado);
    }
    // else if (opc = 2)
    //     recupera_dados(fpG, regc);

    // regc->status = '1';
    //4444

    // escrever status no arquivo fpG

    // Fechar arquivos utilizados
    fclose(fpE);
    fclose(fpG);

    // Libera espaço alocado dinâmicamente na memória
    free(regc);

    return 0;
}
