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

// Função para remover logicamente um registro com base em um critério de busca
void removerRegistroLogico(FILE *arquivoBinario, int64_t offset, CABECALHO *cabecalho) {
    DADOS registro;
    fseek(arquivoBinario, offset, SEEK_SET);
    fread(&registro, sizeof(DADOS), 1, arquivoBinario);
    
    registro.removido = '1';
    registro.prox = cabecalho->topo;
    cabecalho->topo = offset;
    cabecalho->nroRegRem++;
    
    fseek(arquivoBinario, offset, SEEK_SET);
    fwrite(&registro, sizeof(DADOS), 1, arquivoBinario);
}

// Função para atualizar o cabeçalho do arquivo binário
void atualizarCabecalho(CABECALHO *cabecalho, FILE *arquivoBinario) {
    fseek(arquivoBinario, 0, SEEK_SET);
    fwrite(cabecalho, sizeof(CABECALHO), 1, arquivoBinario);
}

// Função para buscar registros e remover conforme o critério
int buscarERemover(FILE *arquivoBinario, CABECALHO *cabecalho, CAMPO_BUSCA *criterio, int x) {
    DADOS registro;
    int64_t offset = sizeof(CABECALHO);
    int encontrou = 0;
    
    while (fread(&registro, sizeof(DADOS), 1, arquivoBinario) == 1) {
        if (registro.removido == '0') {
            int match = 1;
            for (int i = 0; i < x; i++) {
                if (strcmp(criterio[i].nomeCampo, "id") == 0 && registro.id != criterio[i].valorInt) {
                    match = 0;
                } else if (strcmp(criterio[i].nomeCampo, "idade") == 0 && registro.idade != criterio[i].valorInt) {
                    match = 0;
                } else if (strcmp(criterio[i].nomeCampo, "nomeJogador") == 0 && strcmp(registro.nomeJogador, criterio[i].valorString) != 0) {
                    match = 0;
                } else if (strcmp(criterio[i].nomeCampo, "nacionalidade") == 0 && strcmp(registro.nacionalidade, criterio[i].valorString) != 0) {
                    match = 0;
                } else if (strcmp(criterio[i].nomeCampo, "nomeClube") == 0 && strcmp(registro.nomeClube, criterio[i].valorString) != 0) {
                    match = 0;
                }
            }
            if (match) {
                removerRegistroLogico(arquivoBinario, offset, cabecalho);
                encontrou = 1;
                break;
            }
        }
        offset += sizeof(DADOS);
    }
    return encontrou;
}

// Função para atualizar o índice após a remoção de um registro
void atualizarIndice(const char *arquivoIndice, int id) {
    FILE *arquivoIdx = fopen(arquivoIndice, "rb+");
    if (!arquivoIdx) {
        printf("Falha no processamento do arquivo de índice.\n");
        return;
    }

    REGISTRO_INDEX indice;
    while (fread(&indice, sizeof(REGISTRO_INDEX), 1, arquivoIdx) == 1) {
        if (indice.id == id) {
            indice.byteOffset = -1; // Marca o registro como removido
            fseek(arquivoIdx, -sizeof(REGISTRO_INDEX), SEEK_CUR);
            fwrite(&indice, sizeof(REGISTRO_INDEX), 1, arquivoIdx);
            break;
        }
    }

    fclose(arquivoIdx);
}

// Função para processar os critérios de busca e realizar as remoções
void deletar(char* nomeArquivoBinario, char* nomeArquivoIndex, int numBuscas, CAMPO_BUSCA criterios[][10], int x[]) {
    FILE *arquivoBinario = fopen(nomeArquivoBinario, "rb+");
    if (!arquivoBinario) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    CABECALHO cabecalho;
    leitura_cabecalho(&cabecalho, arquivoBinario);

    for (int i = 0; i < numBuscas; i++) {
        int encontrou = buscarERemover(arquivoBinario, &cabecalho, criterios[i], x[i]);
        if (encontrou) {
            // Atualiza o índice
            for (int j = 0; j < x[i]; j++) {
                if (strcmp(criterios[i][j].nomeCampo, "id") == 0) {
                    atualizarIndice(nomeArquivoIndex, criterios[i][j].valorInt);
                }
            }
        } else {
            printf("Registro não encontrado para o critério %d.\n", i + 1);
        }
    }

    atualizarCabecalho(&cabecalho, arquivoBinario);
    fclose(arquivoBinario);

    // Exibir o conteúdo do arquivo binário com a função binarioNaTela
    binarioNaTela(nomeArquivoBinario);
    binarioNaTela(nomeArquivoIndex);
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