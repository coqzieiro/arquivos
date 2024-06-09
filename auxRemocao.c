#include "funcoesAuxiliares.h"
#include "funcoes_fornecidas.h"
#include "funcionalidades.h"
#include "definicoesTipos.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>

// Função que inicializa o registro do jogador
void InicializaRegistroJogador(DADOS* registro) {
    registro->removido = '0'; 
    registro->tamanhoRegistro = 0;
    registro->prox = -1;
    registro->id = -1;
    registro->idade = -1;
    registro->tamNomeJog = 0;
    registro->nomeJogador = NULL;
    registro->tamNacionalidade = 0;
    registro->nacionalidade = NULL;
    registro->tamNomeClube = 0;
    registro->nomeClube = NULL;
}

// Função que aloca memoria para struct jogador
void AlocaMemoriaRegistro(DADOS* registro) {
    registro->nomeClube = (char*)malloc(50*sizeof(char));
    registro->nacionalidade = (char*)malloc(50*sizeof(char));
    registro->nomeJogador = (char*)malloc(50*sizeof(char));
}

// Função que lê os inputs do jogador
int LeituraRemocao(FILE* arquivoBinario, DADOS* registro) {
    // Leitura dos jogador
    fread(&(registro->tamanhoRegistro), 4, 1, arquivoBinario);
    fread(&(registro->removido), 1, 1, arquivoBinario);

    // Verifica se o registro foi removido
    if (registro->removido == '1') {
        fseek(arquivoBinario, registro->tamanhoRegistro - 5, SEEK_CUR);
        return false;
    }

    // Leitura dos jogador
    fread(&(registro->prox), sizeof(int64_t), 1, arquivoBinario);
    fread(&(registro->id), sizeof(int), 1, arquivoBinario);
    fread(&(registro->idade), sizeof(int), 1, arquivoBinario);
    fread(&(registro->tamNomeJog), sizeof(int), 1, arquivoBinario);
    fread(registro->nomeJogador, registro->tamNomeJog, 1, arquivoBinario);
    (registro->nomeJogador)[registro->tamNomeJog] = '\0';
    fread(&(registro->tamNacionalidade), sizeof(int), 1, arquivoBinario);
    fread(registro->nacionalidade, registro->tamNacionalidade, 1, arquivoBinario);
    (registro->nacionalidade)[registro->tamNacionalidade] = '\0';
    fread(&(registro->tamNomeClube), sizeof(int), 1, arquivoBinario);
    fread(registro->nomeClube, registro->tamNomeClube, 1, arquivoBinario);
    (registro->nomeClube)[registro->tamNomeClube] = '\0';

    return 1;
}

// Função para obter o byte offset do último registro removido
int64_t RetornaUltimoRemovido(FILE* arquivoBinario) {
    if (arquivoBinario == NULL) {
        printf("Erro: ponteiro do arquivo é NULL\n");
        return -1;
    }

    int64_t ultimoRemovido = -1;

    // Pula o status
    if (fseek(arquivoBinario, 1, SEEK_SET) != 0) {
        printf("Erro: fseek falhou ao pular o status\n");
        return -1;
    }

    // Armazena o topo
    int64_t topo;
    if (fread(&topo, sizeof(int64_t), 1, arquivoBinario) != 1) {
        printf("Erro: fread falhou ao ler o topo\n");
        return -1;
    }

    // Verifica se o topo é -1
    if(topo == -1){
        return ultimoRemovido;
    } else {
        ultimoRemovido = topo;
    }

    // Pula para o byte offset do topo
    if (fseek(arquivoBinario, topo, SEEK_SET) != 0) {
        printf("Erro: fseek falhou ao pular para o topo\n");
        return -1;
    }

    DADOS registro_dados;
    AlocaMemoriaRegistro(&registro_dados);
    int64_t byteoffsetVDD = 0;

    // Percorre os registros até encontrar o último removido
    while(registro_dados.prox != -1){
        byteoffsetVDD = ftell(arquivoBinario);

        if (fread(&registro_dados.removido, sizeof(char), 1, arquivoBinario) != 1 || fread(&registro_dados.tamanhoRegistro, sizeof(int), 1, arquivoBinario) != 1 || fread(&registro_dados.prox, sizeof(int64_t), 1, arquivoBinario) != 1) {
            printf("Erro: fread falhou ao ler o próximo registro\n");
            return -1;
        }

        // pula 8 bytes
        fseek(arquivoBinario, 8, SEEK_CUR);

        if (fread(&registro_dados.tamNomeJog, sizeof(int), 1, arquivoBinario) != 1) {
            printf("Erro: fread falhou ao ler o tamanho do nome do jogador\n");
            return -1;
        }

        if (registro_dados.tamNomeJog > 0) {
            if (fread(registro_dados.nomeJogador, sizeof(char), registro_dados.tamNomeJog, arquivoBinario) != registro_dados.tamNomeJog) {
                printf("Erro: fread falhou ao ler o nome do jogador\n");
                return -1;
            }
            registro_dados.nomeJogador[registro_dados.tamNomeJog] = '\0';
        }

        if (fread(&registro_dados.tamNacionalidade, sizeof(int), 1, arquivoBinario) != 1){
            printf("Erro: fread falhou ao ler o tamanho da nacionalidade\n");
            return -1;
        }

        if (registro_dados.tamNacionalidade > 0){
            if (fread(registro_dados.nacionalidade, sizeof(char), registro_dados.tamNacionalidade, arquivoBinario) != registro_dados.tamNacionalidade) {
                printf("Erro: fread falhou ao ler a nacionalidade\n");
                return -1;
            }
            registro_dados.nacionalidade[registro_dados.tamNacionalidade] = '\0';
        }

        if (fread(&registro_dados.tamNomeClube, sizeof(int), 1, arquivoBinario) != 1) {
            printf("Erro: fread falhou ao ler o tamanho do nome do clube\n");
            return -1;
        }

        if (registro_dados.tamNomeClube > 0){
            if (fread(registro_dados.nomeClube, sizeof(char), registro_dados.tamNomeClube, arquivoBinario) != registro_dados.tamNomeClube) {
                printf("Erro: fread falhou ao ler o nome do clube\n");
                return -1;
            }
            registro_dados.nomeClube[registro_dados.tamNomeClube] = '\0';
        }

        topo = registro_dados.prox;
        fseek(arquivoBinario, registro_dados.prox, SEEK_SET);
    }

    // Libera a memória alocada para os campos variáveis
    free(registro_dados.nacionalidade);
    free(registro_dados.nomeClube);
    free(registro_dados.nomeJogador);

    return byteoffsetVDD;
}

// Função liberar memoria alocada de dois vetores de strings
void LiberaMemoriaChar(char** nomeCampo, char** valorCampo, int numCamposBusca){
    for (int i = 0; i < numCamposBusca; i++) {
        free(nomeCampo[i]);
        free(valorCampo[i]);
    }
    free(nomeCampo);
    free(valorCampo);
}

// Função desaloca memoria para struct jogador
void DesalocaMemoriaReg(DADOS* registro){
    free(registro->nomeJogador);
    free(registro->nacionalidade);
    free(registro->nomeClube);
}

// essa funcao le os parametros de busca da funcionalidade 3
void LeituraParametrosBusca(int numCamposBusca, char** nomeCampo, char** valorCampo){
    // lendo os parâmetros de busca
    for (int i = 0; i < numCamposBusca; i++) {
        scanf("%s", nomeCampo[i]);
        if (strcmp(nomeCampo[i], "id") == 0 ||
            strcmp(nomeCampo[i], "idade") == 0) {
            scanf("%s", valorCampo[i]);
        } else {
            scan_quote_string(valorCampo[i]);
        }
    }
}