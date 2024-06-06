#include "funcoes.h"
#include "funcoes1.h"
#include "funcoes_aux.h"
#include "funcoes_fornecidas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Função para escrita no cabecalho do arquivo de índices
void escrita_cabecalho_index(CABECALHO_INDEX* cabecalho_index, FILE* nomeArquivoBinarioDeIndices){
    fwrite(&cabecalho_index->status,           sizeof(cabecalho_index->status),         1, nomeArquivoBinarioDeIndices);
}

// Função para escrita do registro no arquivo de índices
void escrita_registro_index(REGISTRO_INDEX* registro_index, FILE* nomeArquivoBinarioDeIndices) {
    fwrite(&registro_index->id, sizeof(registro_index->id), 1, nomeArquivoBinarioDeIndices);
    fwrite(&registro_index->byteOffset, sizeof(registro_index->byteOffset), 1, nomeArquivoBinarioDeIndices);
}

void leitura_cabecalho_index(CABECALHO_INDEX* cabecalho_index, FILE* arquivoIndex) {
    fread(&cabecalho_index->status,         sizeof(cabecalho_index->status),         1, arquivoIndex);
}

// Função para verificar se um registro corresponde a todos os campos de busca
int todosCamposMenosIdCorrespondem(DADOS registro, CAMPO_BUSCA camposBusca[], int numCamposBusca) {
    for (int i = 0; i < numCamposBusca; i++) {
        if (strcmp(camposBusca[i].nomeCampo, "idade") == 0) { // Campo idade
            if (camposBusca[i].valorInt != registro.idade) {
                return(0);
            }
        } else if (strcmp(camposBusca[i].nomeCampo, "nome") == 0) { // Campo nome
            if (strcmp(camposBusca[i].valorString, registro.nomeJogador) != 0) {
                return(0);
            }
        } else if (strcmp(camposBusca[i].nomeCampo, "nacionalidade") == 0) { // Campo nacionalidade
            if (strcmp(camposBusca[i].valorString, registro.nacionalidade) != 0) {
                return(0);
            }
        } else if (strcmp(camposBusca[i].nomeCampo, "nomeClube") == 0) { // Campo nomeClube
            if (strcmp(camposBusca[i].valorString, registro.nomeClube) != 0) {
                return(0);
            }
        } else {
            return(0);
        }
    }
    return(1);
}

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
    return;
}

// remove um ou mais registros do arquivo binario BIN IndiceBIN numBuscas
bool remover(FILE* nomeArquivoBinario, FILE* nomeArquivoBinDeIndices, int numBuscas) {
    // Variáveis auxiliares para leitura dos parâmetros de busca
    char** nomeCampo = (char**)malloc(numBuscas * sizeof(char*));
    for (int i = 0; i < numBuscas; i++) {
        nomeCampo[i] = (char*)malloc(14 * sizeof(char));
    }

    char** valorCampo = (char**)malloc(numBuscas * sizeof(char*));
    for (int i = 0; i < numBuscas; i++) {
        valorCampo[i] = (char*)malloc(40 * sizeof(char));
    }

    // Lendo os parâmetros de busca
    LeituraParametrosBusca(numBuscas, nomeCampo, valorCampo);

    // Variáveis auxiliares para leitura dos registros
    DADOS registro_dados;
    InicializaRegistroJogador(&registro_dados);
    AlocaMemoriaJogador(&registro_dados);

    int registrosEncontrados = 0;
    long long prox;

    // Saltar o cabeçalho
    fseek(nomeArquivoBinario, TAM_INICIAL_BYTEOFFSET, SEEK_SET);

    while (fread(&prox, sizeof(long long), 1, nomeArquivoBinario) == 1) {
        fseek(nomeArquivoBinario, -sizeof(long long), SEEK_CUR);  // Retorna para ler o registro completo

        // Lê o byte offset atual
        long int byteOffset = ftell(nomeArquivoBinario);

        if (!LeDadosJogadorBin(nomeArquivoBinario, &registro_dados)) {
            continue;
        }

        int contadorCampo = 0;

        // Verificar os critérios de busca
        for (int j = 0; j < numBuscas; j++) {
            if (strcmp(nomeCampo[j], "id") == 0 && registro_dados.id == atoi(valorCampo[j])) {
                contadorCampo++;

                // Vai no arquivo de índice e procura o byteoffset do registro do ID
                fseek(nomeArquivoBinDeIndices, 0, SEEK_SET);
                int id;
                long int byteOffsetIndice;
                while (fread(&id, sizeof(int), 1, nomeArquivoBinDeIndices) == 1) {
                    fread(&byteOffsetIndice, sizeof(long int), 1, nomeArquivoBinDeIndices);
                    if (id == registro_dados.id) {

                        // Remoção do registro no arquivo BIN
                        int registrosRemovidos = RetornaByteOffSetUltimoRemovido(nomeArquivoBinario);

                        if (registrosRemovidos != -1) {
                            fseek(nomeArquivoBinario, registrosRemovidos + 5, SEEK_SET);
                            fwrite(&byteOffsetIndice, sizeof(long int), 1, nomeArquivoBinario);

                            fseek(nomeArquivoBinario, byteOffsetIndice, SEEK_SET);
                            char removido = '1';
                            fwrite(&removido, sizeof(char), 1, nomeArquivoBinario);
                        } else {
                            // Vai pro byteoffset do registro que quer remover
                            fseek(nomeArquivoBinario, byteOffsetIndice, SEEK_SET);

                            // Escreve o campo removido
                            char removido = '1';
                            fwrite(&removido, sizeof(char), 1, nomeArquivoBinario);

                            // Volta pro começo do registro pulando status e lê o topo
                            fseek(nomeArquivoBinario, 1, SEEK_SET);
                            long int topo;
                            fread(&topo, sizeof(long int), 1, nomeArquivoBinario);

                            // Volta para o registro que quer remover mais 5 bytes
                            fseek(nomeArquivoBinario, byteOffsetIndice + 5, SEEK_SET);

                            // Escreve o topo no campo prox
                            fwrite(&topo, sizeof(long int), 1, nomeArquivoBinario);

                            // Volta pro começo do registro
                            fseek(nomeArquivoBinario, byteOffsetIndice, SEEK_SET);

                            // Escreve o byteoffset no topo
                            fseek(nomeArquivoBinario, 1, SEEK_SET);
                            fwrite(&byteOffsetIndice, sizeof(long int), 1, nomeArquivoBinario);
                        }
                        break;
                    }
                }

            } else if (strcmp(nomeCampo[j], "idade") == 0 && registro_dados.idade == atoi(valorCampo[j])) {
                contadorCampo++;
            } else if (strcmp(nomeCampo[j], "nomeJogador") == 0 && registro_dados.tamNomeJog != 0 && strcmp(registro_dados.nomeJogador, valorCampo[j]) == 0) {
                contadorCampo++;
            } else if (strcmp(nomeCampo[j], "nacionalidade") == 0 && registro_dados.tamNacionalidade != 0 && strcmp(registro_dados.nacionalidade, valorCampo[j]) == 0) {
                contadorCampo++;
            } else if (strcmp(nomeCampo[j], "nomeClube") == 0 && registro_dados.tamNomeClube != 0 && strcmp(registro_dados.nomeClube, valorCampo[j]) == 0) {
                contadorCampo++;
            }
        }

        // Se todos os critérios forem atendidos
        if (contadorCampo == numBuscas) {
            long int MantemComoTava = ftell(nomeArquivoBinario);

            int registrosRemovidos = RetornaByteOffSetUltimoRemovido(nomeArquivoBinario);

            if (registrosRemovidos != -1) {
                fseek(nomeArquivoBinario, registrosRemovidos + 5, SEEK_SET);
                fwrite(&byteOffset, sizeof(long int), 1, nomeArquivoBinario);

                fseek(nomeArquivoBinario, byteOffset, SEEK_SET);
                char removido = '1';
                fwrite(&removido, sizeof(char), 1, nomeArquivoBinario);
            } else {
                // Vai pro byteoffset do registro que quer remover
                fseek(nomeArquivoBinario, byteOffset, SEEK_SET);

                // Escreve o campo removido
                char removido = '1';
                fwrite(&removido, sizeof(char), 1, nomeArquivoBinario);

                // Volta pro começo do registro pulando status e lê o topo
                fseek(nomeArquivoBinario, 1, SEEK_SET);
                long int topo;
                fread(&topo, sizeof(long int), 1, nomeArquivoBinario);

                // Volta para o registro que quer remover mais 5 bytes
                fseek(nomeArquivoBinario, byteOffset + 5, SEEK_SET);

                // Escreve o topo no campo prox
                fwrite(&topo, sizeof(long int), 1, nomeArquivoBinario);

                // Volta pro começo do registro
                fseek(nomeArquivoBinario, byteOffset, SEEK_SET);

                // Escreve o byteoffset no topo
                fseek(nomeArquivoBinario, 1, SEEK_SET);
                fwrite(&byteOffset, sizeof(long int), 1, nomeArquivoBinario);
            }

            RetornaByteOffSetUltimoRemovido(nomeArquivoBinario);

            fseek(nomeArquivoBinario, MantemComoTava, SEEK_SET);

            registrosEncontrados++;
        }
    }

    // Caso o registro não seja encontrado
    if (registrosEncontrados == 0) {
        LiberaMemoriaChar(nomeCampo, valorCampo, numBuscas);
        DesalocaMemoriaJogador(&registro_dados);
        return false;
    }

    // Libera memória alocada
    LiberaMemoriaChar(nomeCampo, valorCampo, numBuscas);
    DesalocaMemoriaJogador(&registro_dados);

    return true;
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
