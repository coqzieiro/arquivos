/*
INTEGRANTES DO GRUPO:
                    Felipe da Costa Coqueiro,   NºUSP: 11781361
                    Fernando Alee Suaiden,      NºUSP: 12680836
*/

#include "funcoes.h"
#include "funcoes_aux.h"
#include "funcoes_fornecidas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Função para criar o arquivo binário
void criarArquivoBinario(char* nomeArquivoCSV, char* nomeArquivoBinario){

    // Abertura dos arquivos
    FILE* arquivoCSV = fopen(nomeArquivoCSV, "r");
    if (arquivoCSV == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Abre para escrita
    FILE* arquivoBinario = fopen(nomeArquivoBinario, "wb");
    if (arquivoBinario == NULL){
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoCSV);
        return;
    }

    // Inicialização do cabeçalho
    CABECALHO cabecalho;

    cabecalho.status = '0';
    cabecalho.topo = -1;
    cabecalho.proxByteOffset = 25; // Tamanho do cabeçalho
    cabecalho.nroRegArq = 0;
    cabecalho.nroRegRem = 0;

    // Escrita de cada campo do cabeçalho no arquivo binário (sequencialmente)
    EscritaCabecalho(&cabecalho, arquivoBinario);

    // Variáveis para armazenar dados do registro
    DADOS registro;

    // Pula a primeira linha do CSV
    char linha[45];
    fgets(linha, 45, arquivoCSV);

    // Só para o loop quando encontrar o fim do arquivo
    while(!feof(arquivoCSV)){
        char string_aux[40];
        char c;
        int i;

        registro.tamNomeJog = 0;
        registro.tamNacionalidade = 0;
        registro.tamNomeClube = 0;

        // Id
        int linha_vazia = 0;
        i=0;
        while((c = getc(arquivoCSV)) != ','){
            if(c == EOF){ 
                linha_vazia = 1;
                break;
            }
            string_aux[i] = c;
            i++;
        }
        if (linha_vazia) break;
        string_aux[i] = '\0';
        registro.id = atoi(string_aux);
        if (registro.id == 0) registro.id = -1;

        // Idade
        i=0;
        while((c = getc(arquivoCSV)) != ','){
            string_aux[i] = c;
            i++;
        }
        string_aux[i] = '\0';
        registro.idade = atoi(string_aux);
        if (registro.idade == 0) registro.idade = -1;


        // Nome jogador
        i=0;
        while((c = getc(arquivoCSV)) != ','){
            registro.nomeJogador[i] = c;
            registro.tamNomeJog++;
            i++;
        }

        // Nacionalidade
        i=0;
        while((c = getc(arquivoCSV)) != ','){
            registro.nacionalidade[i] = c;
            registro.tamNacionalidade++;
            i++;
        }        

        // Nome do clube
        i=0;
        while((c = getc(arquivoCSV)) != '\n' && c != EOF){
            registro.nomeClube[i] = c;
            registro.tamNomeClube++;
            i++;
        }

        // Cálculo e atribuição do tamanho do registro
        registro.tamanhoRegistro = 33 + registro.tamNomeJog + registro.tamNacionalidade + registro.tamNomeClube;
        
        // Definir registro como não removido logicamente
        registro.removido = '0';

        // Não há próximo registro que foi removido
        registro.prox = -1;

        // Escrita de cada campo do registro no arquivo binário (sequencialmente)
        EscritaRegistro(&registro, arquivoBinario);

        // Atualização do offset do próximo byte
        cabecalho.proxByteOffset += registro.tamanhoRegistro;

        // Incrementação do número de registros
        cabecalho.nroRegArq++;

    }
    // Deu tudo certo
    cabecalho.status = '1';

    // Reescrita do cabeçalho com as informações corretas
    fseek(arquivoBinario, 0, SEEK_SET);
    
    // escrita do cabeçalho no arquivo binário
    EscritaCabecalho(&cabecalho, arquivoBinario);

    // Fechamento dos arquivos
    fclose(arquivoCSV);
    fclose(arquivoBinario);

    return;
}

// Função listar os registros 
void listarRegistros(const char* nomeArquivoBinario) {

    // Abertura do arquivo binário
    FILE* arquivoBinario = fopen(nomeArquivoBinario, "rb");
    if (arquivoBinario == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CABECALHO cabecalho;

    // Leitura do cabeçalho
    LeituraCabecalho(&cabecalho, arquivoBinario);

    // Se o arquivo estiver inconsistente
    if (cabecalho.status == '0') {
        printf("Arquivo inconsistente.\n");
        fclose(arquivoBinario);
        return;
    }

    // Variável para armazenar o registro
    DADOS registro;

    if (cabecalho.nroRegArq == 0){
        printf("Registro inexistente.\n\n");
    }

    // Loop para ler e exibir cada registro
    int i;
    for (i = 0; i < cabecalho.nroRegArq + cabecalho.nroRegRem; i++) {

        // Leitura do registro campo a campo
        LeituraRegistro(&registro, arquivoBinario);

        // Se o registro não estiver removido
        if (registro.removido == '0') {
            
            // Exibição dos dados do registro
            printf("Nome do Jogador: %s\n", registro.nomeJogador[0] ? registro.nomeJogador : "SEM DADO");
            printf("Nacionalidade do Jogador: %s\n", registro.nacionalidade[0] ? registro.nacionalidade : "SEM DADO");
            printf("Clube do Jogador: %s\n", registro.nomeClube[0] ? registro.nomeClube : "SEM DADO");
            printf("\n");

        }

    }

    // Fechamento do arquivo
    fclose(arquivoBinario);
}

// Função para buscar registros em um arquivo de dados
void buscarRegistros(const char *arquivoEntrada, int numBuscas) {
   
    FILE* entrada = fopen(arquivoEntrada, "rb");
    if(entrada == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    CABECALHO cabecalho;

    // Lê o cabeçalho campo a campo
    LeituraCabecalho(&cabecalho, entrada);

    // Verifica a consistência do arquivo
    if(cabecalho.status == '0'){
        printf("Arquivo inconsistente.\n");
        fclose(entrada);
        return;
    }

    // variáveis para armazenar os campos de busca
    CAMPO_BUSCA camposBusca[MAX_CAMPO];
    int numCamposBusca;

    DADOS registro;
    int encontrado;

    // Loop para cada busca
    for (int busca = 0; busca < numBuscas; busca++) {
        printf("Busca %d\n\n", busca + 1);
        encontrado = 0;

        // Leitura do número de campos 
        scanf("%d", &numCamposBusca);
        getchar(); // limpa o buffer do '\n'

        // valida do número de campos
        if (numCamposBusca > MAX_CAMPO) {
            printf("Número máximo de campos excedido.\n");
            continue;
        }

        // Loop para cada campo
        for (int i = 0; i < numCamposBusca; i++) {
            // leitura do nome do campo
            scanf("%s", camposBusca[i].nomeCampo);
            getchar(); 

            // Validação do nome do campo
            if (!validarNomeCampo(camposBusca[i].nomeCampo)) { 
                printf("Nome de campo inválido.\n");
                continue;
            }

            // Lê o valor do campo
            if (strcmp(camposBusca[i].nomeCampo, "id") == 0 || strcmp(camposBusca[i].nomeCampo, "idade") == 0) {
                // Campo de inteiro
                scanf("%d", &camposBusca[i].valorInt); // lê do inteiro
            } else {
                // Campo de string
                scan_quote_string(camposBusca[i].valorString); // lê da string
            }
        }

        // Voltar ao início do arquivo para iniciar a busca
        fseek(entrada, 25, SEEK_SET);

        // Busca os registros correspondentes
        while (!feof(entrada)) {
            
            // Leitura do registro
            LeituraRegistro(&registro, entrada);

            if (registro.removido == '0') {
                if (todosCamposCorrespondem(registro, camposBusca, numCamposBusca)) { // Verifica se um registro corresponde a todos os campos
                    encontrado = 1;
                    printf("Nome do Jogador: %s\n", registro.nomeJogador[0] ? registro.nomeJogador : "SEM DADO");
                    printf("Nacionalidade do Jogador: %s\n", registro.nacionalidade[0] ? registro.nacionalidade : "SEM DADO");
                    printf("Clube do Jogador: %s\n", registro.nomeClube[0] ? registro.nomeClube : "SEM DADO");
                    printf("\n");
                }
            }
        }

        // Mensagem caso nenhum registro seja encontrado
        if (!encontrado) {
            printf("Registro inexistente.\n\n");
        }
    }

    // Fechamento do arquivo
    fclose(entrada);
}