#include "funcoes.h"
#include "funcoes1.h"
#include "funcoes_aux.h"
#include "funcoes_fornecidas.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

// Função para criar index a partir do arquivo binário
void criarIndex(char* nomeArquivoBinario, char* nomeArquivoBinDeIndices){
    // Abertura dos arquivos
    FILE* arquivoBinario = fopen(nomeArquivoBinario, "rb");
    if (arquivoBinario == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Abre para escrita
    FILE* arquivoBinarioDeIndices = fopen(nomeArquivoBinDeIndices, "wb");
    if (arquivoBinarioDeIndices == NULL){
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    // Inicialização do cabeçalho
    CABECALHO_INDEX cabecalho_index;

    cabecalho_index.status = '0';

    // Escrita do cabeçalho no arquivo binário de índices
    escrita_cabecalho_index(&cabecalho_index, arquivoBinarioDeIndices);

    // Variáveis para armazenar dados do registro
    REGISTRO_INDEX registro_index;

    char statusCabecalhoArquivoBinario;

    fread(&statusCabecalhoArquivoBinario, sizeof(char), 1, arquivoBinario);

    // Verifica a consistência do arquivo
    if(statusCabecalhoArquivoBinario == '0'){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Pula o cabeçalho do arquivo de dados
    fseek(arquivoBinario, 25, SEEK_SET);

    // Variável para calcular o byteOffset
    int64_t byteOffset = 25; // Começa após o cabeçalho

    // Só para o loop quando encontrar o fim do arquivo
    while (1) {
        char removido;
        int tamanhoRegistroInteiro;

        // Tente ler o campo removido
        if (fread(&removido, sizeof(char), 1, arquivoBinario) != 1) {
            break; // Sai do loop se não conseguir ler
        }

        // Tente ler o tamanho do registro
        if (fread(&tamanhoRegistroInteiro, sizeof(int), 1, arquivoBinario) != 1) {
            break; // Sai do loop se não conseguir ler
        }

        // Pula o campo 'prox' (8 bytes) e lê 'id' (4 bytes)
        fseek(arquivoBinario, 8, SEEK_CUR);
        if (fread(&registro_index.id, sizeof(int), 1, arquivoBinario) != 1) {
            break; // Sai do loop se não conseguir ler
        }

        // Verifica se o registro não foi removido
        if (removido != '1') {
            // Define o byteOffset do registro atual
            registro_index.byteOffset = byteOffset;

            // Escreve o registro de índice no arquivo binário de índices
            escrita_registro_index(&registro_index, arquivoBinarioDeIndices);
        }

        // Atualiza o byteOffset para o próximo registro
        byteOffset += tamanhoRegistroInteiro;

        // Move o ponteiro do arquivo para o próximo registro
        fseek(arquivoBinario, byteOffset, SEEK_SET);

    }
    // Deu tudo certo
    cabecalho_index.status = '1';

    // Reescrita do cabeçalho com as informações corretas
    fseek(arquivoBinarioDeIndices, 0, SEEK_SET);
    
    // escrita do cabeçalho no arquivo binário de índices
    escrita_cabecalho_index(&cabecalho_index, arquivoBinarioDeIndices);

    // Fechamento dos arquivos
    fclose(arquivoBinario);
    fclose(arquivoBinarioDeIndices);
    binarioNaTela(nomeArquivoBinDeIndices);

    return;
}

// Função para remover registro no arquivo binário
void remover(char *nomeArquivoDados, char *nomeArquivoIndice, int numRemocoes) {
    FILE *arquivoDados = fopen(nomeArquivoDados, "r+b");
    FILE *arquivoIndice = fopen(nomeArquivoIndice, "r+b");
    if (arquivoDados == NULL || arquivoIndice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        if (arquivoDados != NULL) fclose(arquivoDados);
        if (arquivoIndice != NULL) fclose(arquivoIndice);
        return;
    }

    CABECALHO cabecalhoDados, cabecalhoIndice;
    leitura_cabecalho(&cabecalhoDados, arquivoDados);
    leitura_cabecalho(&cabecalhoIndice, arquivoIndice);

    if (cabecalhoDados.status == '0' || cabecalhoIndice.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arquivoDados);
        fclose(arquivoIndice);
        return;
    }

    for (int i = 0; i < numRemocoes; i++) {
        int numCamposBusca;
        scanf("%d", &numCamposBusca);

        CAMPO_BUSCA camposBusca[numCamposBusca];
        for (int j = 0; j < numCamposBusca; j++) {
            scanf("%s", camposBusca[j].nomeCampo);
            if (validarNomeCampo(camposBusca[j].nomeCampo) == 0) {
                printf("Falha no processamento do arquivo.\n");
                fclose(arquivoDados);
                fclose(arquivoIndice);
                return;
            }

            if (strcmp(camposBusca[j].nomeCampo, "id") == 0 || strcmp(camposBusca[j].nomeCampo, "idade") == 0) {
                scanf("%d", &camposBusca[j].valorInt);
            } else {
                scan_quote_string(camposBusca[j].valorString);
            }
        }

        DADOS registro;
        fseek(arquivoDados, 25, SEEK_SET); // Vai para o início dos registros de dados
        int64_t offsetRemovido = -1;
        while (ftell(arquivoDados) < cabecalhoDados.proxByteOffset) {
            int64_t byteOffset = ftell(arquivoDados);
            leitura_registro(&registro, arquivoDados);
            if (registro.removido == '1' && todosCamposCorrespondem(registro, camposBusca, numCamposBusca)) {
                registro.removido = '0'; // Marca como removido
                fseek(arquivoDados, byteOffset, SEEK_SET);
                escrita_registro(&registro, arquivoDados);

                offsetRemovido = byteOffset;
                cabecalhoDados.nroRegArq--;
                cabecalhoDados.nroRegRem++;
                break;
            }
        }

        if (offsetRemovido != -1) {
            atualizarIndiceRemocao(arquivoIndice, &cabecalhoIndice, registro.id);
            inserirNoListaRemovidos(arquivoDados, &cabecalhoDados, offsetRemovido, registro.tamanhoRegistro + 5);
        }
    }

    fseek(arquivoDados, 0, SEEK_SET);
    escrita_cabecalho(&cabecalhoDados, arquivoDados);

    fseek(arquivoIndice, 0, SEEK_SET);
    escrita_cabecalho(&cabecalhoIndice, arquivoIndice);

    fclose(arquivoDados);
    fclose(arquivoIndice);

    binarioNaTela(nomeArquivoDados);
    binarioNaTela(nomeArquivoIndice);
}

// Função para inserir novo registro no arquivo binário
void inserir(char* nomeArquivoBinario, char* nomeArquivoIndex, int numBuscas) {
    // Abertura dos arquivos binário e de índice
    FILE* arquivoBinario = fopen(nomeArquivoBinario, "rb+");
    FILE* arquivoIndice = fopen(nomeArquivoIndex, "rb+");
    if (!arquivoBinario || !arquivoIndice) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Leitura do cabeçalho do arquivo de dados
    CABECALHO cabecalho;
    leitura_cabecalho(&cabecalho, arquivoBinario);

    // Criação do novo registro
    DADOS novoRegistro;
    novoRegistro.removido = '0';
    novoRegistro.prox = -1;

    for(int i=0; i < numBuscas; i++){
        scanf("%d", &novoRegistro.id); // Leitura do campo 'id'
        scanf("%d", &novoRegistro.idade); // Leitura do campo 'idade'
        scan_quote_string(novoRegistro.nomeJogador); // Leitura do campo 'nomeJogador'
        novoRegistro.tamNomeJog = strlen(novoRegistro.nomeJogador);
        scan_quote_string(novoRegistro.nacionalidade); // Leitura do campo 'nacionalidade'
        novoRegistro.tamNacionalidade = strlen(novoRegistro.nacionalidade);
        scan_quote_string(novoRegistro.nomeClube); // Leitura do campo 'nomeClube'
        novoRegistro.tamNomeClube = strlen(novoRegistro.nomeClube);
    }
    // Cálculo do tamanho do registro
    novoRegistro.tamanhoRegistro = 33 + novoRegistro.tamNomeJog + novoRegistro.tamNacionalidade + novoRegistro.tamNomeClube;

    int64_t offset;
    if (cabecalho.topo == -1) {  // Não há registros removidos
        offset = cabecalho.proxByteOffset;
        cabecalho.proxByteOffset += novoRegistro.tamanhoRegistro + 5;
    } else {  // Há registros removidos
        offset = cabecalho.topo;
        fseek(arquivoBinario, offset, SEEK_SET);
        fread(&novoRegistro.removido, sizeof(char), 1, arquivoBinario);
        fread(&novoRegistro.tamanhoRegistro, sizeof(int), 1, arquivoBinario);
        fread(&novoRegistro.prox, sizeof(int64_t), 1, arquivoBinario);
        cabecalho.topo = novoRegistro.prox;
    }

    // Escrita do novo registro no arquivo de dados
    fseek(arquivoBinario, offset, SEEK_SET);
    escrita_registro(&novoRegistro, arquivoBinario);

    // Atualização do cabeçalho
    cabecalho.nroRegArq++;
    if (novoRegistro.removido == '1') cabecalho.nroRegRem--;
    escrita_cabecalho(&cabecalho, arquivoBinario);

    // Atualização do índice
    fseek(arquivoIndice, 0, SEEK_END);
    REGISTRO_INDEX novoIndice;
    novoIndice.id = novoRegistro.id;
    novoIndice.byteOffset = offset;
    escrita_registro_index(&novoIndice, arquivoIndice);

    // Fechamento dos arquivos
    fclose(arquivoBinario);
    fclose(arquivoIndice);
    
    // Exibir o conteúdo do arquivo binário com a função binarioNaTela
    binarioNaTela(nomeArquivoBinario);
    binarioNaTela(nomeArquivoIndex);
}