#include "funcoes.h"
#include "funcoes1.h"
#include "funcoes_aux.h"
#include "funcoes_fornecidas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Função para escrita no cabecalho
void escrita_cabecalho_index(CABECALHO_INDEX* cabecalho_index, FILE* nomeArquivoBinarioDeIndices){
    fwrite(&cabecalho_index->status,           sizeof(cabecalho_index->status),         1, nomeArquivoBinarioDeIndices);
}

// Função para escrita do registro no arquivo de índices
void escrita_registro_index(REGISTRO_INDEX* registro_index, FILE* nomeArquivoBinarioDeIndices) {
    fwrite(&registro_index->id, sizeof(registro_index->id), 1, nomeArquivoBinarioDeIndices);
    fwrite(&registro_index->byteOffset, sizeof(registro_index->byteOffset), 1, nomeArquivoBinarioDeIndices);
}

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

void deletar(char* nomeArquivoBinario, char* nomeArquivoIndex, int numBuscas) {
    FILE* arquivoDados = fopen(nomeArquivoBinario, "rb+");
    FILE* arquivoIndice = fopen(nomeArquivoIndex, "rb+");

    if (!arquivoDados || !arquivoIndice) {
        printf("Falha no processamento do arquivo.\n");
        if (arquivoDados) fclose(arquivoDados);
        if (arquivoIndice) fclose(arquivoIndice);
        return;
    }

    CABECALHO cabecalho;
    leitura_cabecalho(&cabecalho, arquivoDados);

    CABECALHO_INDEX cabecalhoIndex;
    fread(&cabecalhoIndex, sizeof(CABECALHO_INDEX), 1, arquivoIndice);

    CAMPO_BUSCA criterios[MAX_CAMPO];
    int registrosRemovidos = 0;

    for (int busca = 0; busca < numBuscas; busca++) {
        int numCamposBusca;
        scanf("%d", &numCamposBusca);
        getchar(); // Limpa o buffer do '\n'

        for (int i = 0; i < numCamposBusca; i++) {
            scanf("%s", criterios[i].nomeCampo);
            getchar(); // Limpa o buffer do '\n'

            if (strcmp(criterios[i].nomeCampo, "id") == 0 || strcmp(criterios[i].nomeCampo, "idade") == 0) {
                scanf("%d", &criterios[i].valorInt);
                getchar(); // Limpa o buffer do '\n'
            } else {
                scan_quote_string(criterios[i].valorString);
            }
        }

        fseek(arquivoDados, 25, SEEK_SET);
        int64_t byteOffset;
        DADOS registro;

        while (fread(&registro.removido, sizeof(char), 1, arquivoDados) == 1) {
            fread(&registro.tamanhoRegistro, sizeof(int), 1, arquivoDados);
            byteOffset = ftell(arquivoDados) - 5;

            fread(&registro.prox, sizeof(int64_t), 1, arquivoDados);
            fread(&registro.id, sizeof(int), 1, arquivoDados);
            fread(&registro.idade, sizeof(int), 1, arquivoDados);
            fread(&registro.tamNomeJog, sizeof(int), 1, arquivoDados);
            fread(registro.nomeJogador, sizeof(char), registro.tamNomeJog, arquivoDados);
            registro.nomeJogador[registro.tamNomeJog] = '\0';
            fread(&registro.tamNacionalidade, sizeof(int), 1, arquivoDados);
            fread(registro.nacionalidade, sizeof(char), registro.tamNacionalidade, arquivoDados);
            registro.nacionalidade[registro.tamNacionalidade] = '\0';
            fread(&registro.tamNomeClube, sizeof(int), 1, arquivoDados);
            fread(registro.nomeClube, sizeof(char), registro.tamNomeClube, arquivoDados);
            registro.nomeClube[registro.tamNomeClube] = '\0';

            if (registro.removido == '0' && todosCamposCorrespondem(registro, criterios, numCamposBusca)) {
                registro.removido = '1';
                fseek(arquivoDados, byteOffset, SEEK_SET);
                fwrite(&registro.removido, sizeof(char), 1, arquivoDados);
                fseek(arquivoDados, byteOffset + 5 + sizeof(int64_t) + sizeof(int) * 2, SEEK_SET);
                fwrite(&cabecalho.topo, sizeof(int64_t), 1, arquivoDados);
                cabecalho.topo = byteOffset;
                cabecalho.nroRegRem++;
                registrosRemovidos++;

                // Atualizar o arquivo de índice
                fseek(arquivoIndice, 1, SEEK_SET); // Pula o status
                int nroIndices;
                fread(&nroIndices, sizeof(int), 1, arquivoIndice);

                REGISTRO_INDEX indice;
                FILE* novoArquivoIndice = fopen("tmp.bin", "wb+");
                fwrite(&cabecalhoIndex.status, sizeof(char), 1, novoArquivoIndice);
                fwrite(&nroIndices, sizeof(int), 1, novoArquivoIndice);

                fseek(arquivoIndice, sizeof(CABECALHO_INDEX), SEEK_SET);
                for (int i = 0; i < nroIndices; i++) {
                    fread(&indice.id, sizeof(int), 1, arquivoIndice);
                    fread(&indice.byteOffset, sizeof(int64_t), 1, arquivoIndice);

                    if (indice.id != registro.id) {
                        fwrite(&indice.id, sizeof(int), 1, novoArquivoIndice);
                        fwrite(&indice.byteOffset, sizeof(int64_t), 1, novoArquivoIndice);
                    }
                }

                fclose(arquivoIndice);
                fclose(novoArquivoIndice);

                remove(nomeArquivoIndex);
                rename("tmp.bin", nomeArquivoIndex);
                arquivoIndice = fopen(nomeArquivoIndex, "rb+");
            }

            fseek(arquivoDados, byteOffset + registro.tamanhoRegistro + 5, SEEK_SET);
        }
    }

    cabecalho.status = '1';
    fseek(arquivoDados, 0, SEEK_SET);
    escrita_cabecalho(&cabecalho, arquivoDados);

    fseek(arquivoIndice, 0, SEEK_SET);
    fwrite(&cabecalhoIndex.status, sizeof(char), 1, arquivoIndice);

    fclose(arquivoDados);
    fclose(arquivoIndice);

    // Exibir conteúdo dos arquivos
    binarioNaTela(nomeArquivoBinario);
    binarioNaTela(nomeArquivoIndex);
}