#include "funcoes.h"
#include "funcoes1.h"
#include "funcoes_aux.h"
#include "funcoes_fornecidas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Função para criar index a partir do arquivo binário
int criarIndex(char* nomeArquivoBinario, char* nomeArquivoIndices, int opcao){
    // Abertura dos arquivos
    FILE* arquivoBinario = fopen(nomeArquivoBinario, "rb");
    if (arquivoBinario == NULL){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // Abre para escrita
    FILE* arquivoBinarioDeIndices = fopen(nomeArquivoIndices, "wb");
    if (arquivoBinarioDeIndices == NULL){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    // Inicialização do cabeçalho
    CABECALHO_INDEX cabecalho_index;

    cabecalho_index.status = '0';

    // Escrita do cabeçalho no arquivo binário de índices
    EscritaCabecalho_Index(&cabecalho_index, arquivoBinarioDeIndices);

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
    int64_t byteOffset = 25; // Começa após o cabeçalho

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
            EscritaRegistro_Index(&registro_index, arquivoBinarioDeIndices);
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
    EscritaCabecalho_Index(&cabecalho_index, arquivoBinarioDeIndices);

    // Fechamento dos arquivos
    fclose(arquivoBinario);
    fclose(arquivoBinarioDeIndices);
    
    return 1;
}

// Função para remover os registros especificados
bool remover(FILE* nomeArquivoBinario, FILE* nomeArquivoIndices, int numeroDeBuscas) {
    // Alocação de memória para 'valorCampo'
    char** valorCampo = (char**)malloc(numeroDeBuscas * sizeof(char*));
    for (int i = 0; i < numeroDeBuscas; i++) {
        valorCampo[i] = (char*)malloc(40 * sizeof(char));
    }
    
    // Alocação de memória para 'campo'
    char** campo = (char**)malloc(numeroDeBuscas * sizeof(char*));
    for (int i = 0; i < numeroDeBuscas; i++) {
        campo[i] = (char*)malloc(14 * sizeof(char));
    }

    // Lê os campos da busca
    LeituraParametrosBusca(numeroDeBuscas, campo, valorCampo);

    // Inicializa registro de dados
    DADOS registro_dados;
    InicializaRegistroJogador(&registro_dados);

    // Aloca memória para variáveis de tamanho variável
    AlocaMemoriaRegistro(&registro_dados);

    int numRegistrosCorrespondentes = 0;
    int64_t prox;

    // Pula o cabeçalho
    fseek(nomeArquivoBinario, TAM_INICIAL_BYTEOFFSET, SEEK_SET);

    // Enquanto fread funcionar (retonando 1), continuamos o loop
    while (fread(&prox, sizeof(int64_t), 1, nomeArquivoBinario) == 1) {
        // Volta o 8bytes para ler o registro inteiro
        fseek(nomeArquivoBinario, -sizeof(int64_t), SEEK_CUR);

        int64_t byteOffset = ftell(nomeArquivoBinario);

        if (!LeDadosJogadorBin(nomeArquivoBinario, &registro_dados)) {
            continue;
        }

        // Contador para a quantidade de campos da busca
        int counterCampo = 0;

        // Verificar os critérios de busca
        for (int j = 0; j < numeroDeBuscas; j++) {
            if (strcmp(campo[j], "id") == 0 && registro_dados.id == atoi(valorCampo[j])) {
                counterCampo++;

                // Vai no arquivo de índice e procura o byteoffset do registro do ID
                fseek(nomeArquivoIndices, 0, SEEK_SET);
                int idRegistro;
                int64_t byteOffsetIndice;
                while (fread(&idRegistro, sizeof(int), 1, nomeArquivoIndices) == 1) {
                    fread(&byteOffsetIndice, sizeof(int64_t), 1, nomeArquivoIndices);
                    if (idRegistro == registro_dados.id) {

                        // ByteOffSet do último registro removido
                        int byteOffSetUltimoRegistroRemovido = RetornaByteOffSetUltimoRemovido(nomeArquivoBinario);

                        //  Se o arquivo binário tiver registros removidos inicialmente
                        if (byteOffSetUltimoRegistroRemovido != -1) {
                            //pula para o campo próx do registro
                            fseek(nomeArquivoBinario, byteOffSetUltimoRegistroRemovido + 5, SEEK_SET);
                            fwrite(&byteOffsetIndice, sizeof(int64_t), 1, nomeArquivoBinario);

                            fseek(nomeArquivoBinario, byteOffsetIndice, SEEK_SET);
                            char removido = '1';
                            fwrite(&removido, sizeof(char), 1, nomeArquivoBinario);

                        // Senão, o arquivo binário não tem registros removidos inicialmente e vem para cá
                        } else {
                            // fseek para o byteoffset do registro que será removido
                            fseek(nomeArquivoBinario, byteOffsetIndice, SEEK_SET);

                            // Atualiza o removido
                            char removido = '1';
                            fwrite(&removido, sizeof(char), 1, nomeArquivoBinario);

                            // Volta pro começo do registro pulando status e lê o topo
                            fseek(nomeArquivoBinario, 1, SEEK_SET);
                            int64_t topo;
                            fread(&topo, sizeof(int64_t), 1, nomeArquivoBinario);

                            // Volta para o registro que quer remover mais 5 bytes
                            fseek(nomeArquivoBinario, byteOffsetIndice + 5, SEEK_SET);

                            // Escreve o topo no campo prox
                            fwrite(&topo, sizeof(int64_t), 1, nomeArquivoBinario);

                            // Volta pro começo do registro
                            fseek(nomeArquivoBinario, byteOffsetIndice, SEEK_SET);

                            // Escreve o byteoffset no topo
                            fseek(nomeArquivoBinario, 1, SEEK_SET);
                            fwrite(&byteOffsetIndice, sizeof(int64_t), 1, nomeArquivoBinario);
                        }
                        break;
                    }
                }

            } else if (strcmp(campo[j], "idade") == 0 && registro_dados.idade == atoi(valorCampo[j])) {
                counterCampo++;
            } else if (strcmp(campo[j], "nomeJogador") == 0 && registro_dados.tamNomeJog != 0 && strcmp(registro_dados.nomeJogador, valorCampo[j]) == 0) {
                counterCampo++;
            } else if (strcmp(campo[j], "nacionalidade") == 0 && registro_dados.tamNacionalidade != 0 && strcmp(registro_dados.nacionalidade, valorCampo[j]) == 0) {
                counterCampo++;
            } else if (strcmp(campo[j], "nomeClube") == 0 && registro_dados.tamNomeClube != 0 && strcmp(registro_dados.nomeClube, valorCampo[j]) == 0) {
                counterCampo++;
            }
        }

        // Se todos os critérios forem atendidos
        if (counterCampo == numeroDeBuscas) {

            // Salvamos o byteoffset atual pois os freads e fwrites vão mudar a posição do arquivo
            int64_t byteOffsetAtual = ftell(nomeArquivoBinario);

            int byteOffSetUltimoRegistroRemovido = RetornaByteOffSetUltimoRemovido(nomeArquivoBinario);

            if (byteOffSetUltimoRegistroRemovido != -1) {
                // pulamos para o campo prox do registro
                fseek(nomeArquivoBinario, byteOffSetUltimoRegistroRemovido + 5, SEEK_SET);
                fwrite(&byteOffset, sizeof(int64_t), 1, nomeArquivoBinario);

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
                int64_t topo;
                fread(&topo, sizeof(int64_t), 1, nomeArquivoBinario);

                // Volta para o registro que quer remover mais 5 bytes
                fseek(nomeArquivoBinario, byteOffset + 5, SEEK_SET);

                // Escreve o topo no campo prox
                fwrite(&topo, sizeof(int64_t), 1, nomeArquivoBinario);

                // Volta pro começo do registro
                fseek(nomeArquivoBinario, byteOffset, SEEK_SET);

                // Escreve o byteoffset no topo
                fseek(nomeArquivoBinario, 1, SEEK_SET);
                fwrite(&byteOffset, sizeof(int64_t), 1, nomeArquivoBinario);
            }

            RetornaByteOffSetUltimoRemovido(nomeArquivoBinario);

            //volta para o byteoffset que estava antes de mudarmos a posição no arquivo binário com freads e fwrites
            fseek(nomeArquivoBinario, byteOffsetAtual, SEEK_SET);

            numRegistrosCorrespondentes++;
        }
    }

    // Caso o registro não seja encontrado
    if (numRegistrosCorrespondentes == 0) {
        LiberaMemoriaChar(campo, valorCampo, numeroDeBuscas);
        DesalocaMemoriaReg(&registro_dados);
        return false;
    }

    // Libera memória alocada
    LiberaMemoriaChar(campo, valorCampo, numeroDeBuscas);
    DesalocaMemoriaReg(&registro_dados);

    return true;
}

// Função que insere um novo registro no arquivo
void inserir(FILE* nomeArquivoBinario){
    
    if(nomeArquivoBinario == NULL){
        printf("Erro: ponteiro do arquivo é NULL\n");
        return;
    }
    
    CABECALHO cabecalho;
    
    // Inicializa o cabeçalho
    IniCabecalho(&cabecalho);
    
    // Leitura do cabeçalho
    LeituraCabecalho(&cabecalho, nomeArquivoBinario);

    DADOS registro_dados;

    // Aloca memória para o registro
    AlocaMemoriaRegistro(&registro_dados);

    // Lê os campos relacionados ao registro
    LeituraCampos(&registro_dados);

    // Atualiza o tamanho das strings
    AtualizaTamanhoStrings(&registro_dados);

    // Atualiza os outros campos
    AtualizaCampos(&registro_dados);

    // Armazenar o offset registro removido
    int64_t bestFitOffset = -1;
    LISTA* removidos = NULL;

    // Leitura do topo
    int64_t topo;
    fseek(nomeArquivoBinario, 1, SEEK_SET);
    fread(&topo, sizeof(int64_t), 1, nomeArquivoBinario);

    // Se tem registro removido
    if(topo != -1){

        // Ordena os registros removidos
        removidos = OrdenaRegistrosRem(nomeArquivoBinario);

        if(removidos != NULL){
            // Encontra o byteoffset do melhor registro removido para reutilizar
            bestFitOffset = BestFit(&removidos, registro_dados.tamanhoRegistro, nomeArquivoBinario);
        }
    }

    // Adiciona o registro
    int* tamanhoRegistro = ReutilizarRegistro(nomeArquivoBinario, &cabecalho, &registro_dados, bestFitOffset, removidos);

    // Define os novos valores para o registro não removido
    registro_dados.prox = -1;
    registro_dados.removido = '0';

    // Aumenta o número de registros
    cabecalho.nroRegArq++;

    // Escreve os dados inseridos no arquivo
    EscreveDadosJogadorBin(nomeArquivoBinario, &registro_dados);

    // Escreve o lixo '$' de acordo com o tamanho do registro
    EscreveLixo(nomeArquivoBinario, tamanhoRegistro[0], tamanhoRegistro[1]);

    // Libera a memória alocada
    free(tamanhoRegistro);

    // Atualiza a lista de removidos no arquivo
    ReescreveRemovidos(nomeArquivoBinario, removidos);

    // Libera a memória alocada
    DesalocaMemoriaReg(&registro_dados);
    DesalocaLista(removidos);

    // Atualiza e escreve o cabeçalho no arquivo
    EscritaCabecalho(&cabecalho, nomeArquivoBinario);

    return;
}