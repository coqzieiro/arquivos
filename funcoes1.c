#include "funcoes.h"
#include "funcoes1.h"
#include "funcoes_aux.h"
#include "funcoes_fornecidas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Função para criar index a partir do arquivo binário
int criarIndex(char* nomeArquivoBinario, char* nomeArquivoBinDeIndices, int opcao){
    // Abertura dos arquivos
    FILE* arquivoBinario = fopen(nomeArquivoBinario, "rb");
    if (arquivoBinario == NULL){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // Abre para escrita
    FILE* arquivoBinarioDeIndices = fopen(nomeArquivoBinDeIndices, "wb");
    if (arquivoBinarioDeIndices == NULL){
        printf("Falha no processamento do arquivo.\n");
        return 0;
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
        return 0;
    }

    // Pula o cabeçalho do arquivo de dados
    fseek(arquivoBinario, 25, SEEK_SET);

    // Variável para calcular o byteOffset
    long int byteOffset = 25; // Começa após o cabeçalho

    // Só para o loop quando encontrar o fim do arquivo
    while(1){
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
    
    return 1;
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

// Função para inserir um novo registro no arquivo binário
bool inserir(FILE* nomeArquivoBinario) {
    
    if (nomeArquivoBinario == NULL) {
        fprintf(stderr, "Erro: ponteiro do arquivo é NULL\n");
        return false;
    }
    
    // Leitura do cabeçalho
    CABECALHO cabecalho;
    InicializaCabecalho(&cabecalho);
    leitura_cabecalho(&cabecalho, nomeArquivoBinario);

    // Inicializa e aloca memória para o registro do jogador
    DADOS registro_dados;
    InicializaRegistroJogador(&registro_dados);
    AlocaMemoriaJogador(&registro_dados);

    // Lê os dados do jogador pelo stdin
    LerInputDadosJogador(&registro_dados);

    // Atualiza o tamanho das strings do jogador
    AtualizaTamanhoStringsJogador(&registro_dados);

    // Atualiza os campos secundários do registro do jogador
    AtualizaCampos(&registro_dados);

    // Lê o topo do binário
    long int topoAtualBin;
    fseek(nomeArquivoBinario, 1, SEEK_SET);
    fread(&topoAtualBin, sizeof(long int), 1, nomeArquivoBinario);

    // Variável para armazenar o offset do melhor registro removido
    long int bestFitOffset = -1;
    LISTABYTE* removidos = NULL;

    // Verifica se existe algum registro removido
    if (topoAtualBin != -1) {
        // Captura e ordena os registros removidos
        removidos = OrdenaRegistrosRemovidos(nomeArquivoBinario);
        if (removidos != NULL) {
            // Encontra o melhor registro removido para reutilizar
            bestFitOffset = BestFitRegister(&removidos, registro_dados.tamanhoRegistro, nomeArquivoBinario);
        }
    }

    // Reutiliza ou adiciona o registro
    int* tamRegs = ReutilizarOuAdicionarRegistro(nomeArquivoBinario, &cabecalho, &registro_dados, bestFitOffset, removidos);

    // Incrementa o número de registros no arquivo
    cabecalho.nroRegArq++;

    // Define os novos valores para o registro não removido
    registro_dados.removido = '0';
    registro_dados.prox = -1;

    // Escreve os dados do jogador no arquivo binário
    EscreveDadosJogadorBin(nomeArquivoBinario, &registro_dados);

    // Escreve o lixo restante de acordo com tamRegs
    EscreveLixoRestante(nomeArquivoBinario, tamRegs[0], tamRegs[1]);

    // Libera a memória alocada para tamRegs
    free(tamRegs);

    // Atualiza a lista de removidos no arquivo
    ReescreveRegistrosRemovidosBIN(nomeArquivoBinario, removidos);

    // Libera a memória alocada para o registro e a lista de removidos
    DesalocaMemoriaJogador(&registro_dados);
    LiberaLista(removidos);

    // Atualiza e escreve o cabeçalho no arquivo
    escrita_cabecalho(&cabecalho, nomeArquivoBinario);

    return true;
}