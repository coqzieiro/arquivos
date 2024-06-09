/*
INTEGRANTES DO GRUPO:
                    Felipe da Costa Coqueiro,   NºUSP: 11781361
                    Fernando Alee Suaiden,      NºUSP: 12680836
*/

#include "funcoesAuxiliares.h"
#include "funcoes_fornecidas.h"
#include "funcionalidades.h"
#include "definicoesTipos.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Função para inserir ordenado na lista
void InserirOrdenado(LISTA** head, LISTA* novoNo) {
    LISTA* atual;

    // Caso especial para o cabeçalho da lista
    if (*head == NULL || (*head)->tamRegistro > novoNo->tamRegistro) {
        novoNo->prox = *head;
        *head = novoNo;
    } else {
        // Localiza o nó anterior ao ponto de inserção
        atual = *head;
        LISTA* anterior = NULL;

        while (atual != NULL && atual->tamRegistro <= novoNo->tamRegistro) {
            anterior = atual;
            atual = atual->prox;
        }

        novoNo->prox = anterior->prox;
        anterior->prox = novoNo;
    }
}

// Função para reescrever os registros removidos no arquivo binário
bool ReescreveRemovidos(FILE* arquivoBinario, LISTA* removidos) {
    if (arquivoBinario == NULL) {
        fprintf(stderr, "Erro: ponteiro do arquivo é NULL\n");
        return false;
    }

    if (removidos == NULL) {
        // Se a lista está vazia, marca o topo como -1
        fseek(arquivoBinario, 1, SEEK_SET);
        int64_t topo = -1;
        fwrite(&topo, sizeof(int64_t), 1, arquivoBinario);
        return true;
    }

    // Atualiza o topo da lista de removidos
    fseek(arquivoBinario, 1, SEEK_SET);
    fwrite(&removidos->byteOffset, sizeof(int64_t), 1, arquivoBinario);

    LISTA* atual = removidos;
    while (atual != NULL) {
        fseek(arquivoBinario, atual->byteOffset, SEEK_SET);

        char removido = '1';  // Marca como removido
        fwrite(&removido, sizeof(char), 1, arquivoBinario);
        fwrite(&atual->tamRegistro, sizeof(int), 1, arquivoBinario);

        if (atual->prox != NULL) {
            fwrite(&atual->prox->byteOffset, sizeof(int64_t), 1, arquivoBinario);
        } else {
            int64_t end_of_list = -1;
            fwrite(&end_of_list, sizeof(int64_t), 1, arquivoBinario);
        }

        atual = atual->prox;
    }
    return true;
}

// Função que libera memória alocada para a lista encadeada
void DesalocaLista(LISTA* cabeca) {
    LISTA* atual = cabeca;
    LISTA* next;

    while (atual != NULL) {
        next = atual->prox;
        free(atual);
        atual = next;
    }
}

// Função retorna o byte offset do registro a ser substituido no binario
int64_t MetodoBestFit(LISTA** removidos, int tamRegistro, FILE* arquivoBinario) {
    LISTA* atual = *removidos;
    LISTA* anterior = NULL;

    // percorre a lista e encontra o primeiro registro suficientemente grande
    while (atual != NULL) {
        if (atual->tamRegistro >= tamRegistro) {
            int64_t offset = atual->byteOffset;

            // remove o registro da lista
            if (anterior == NULL) {
                // o registro a ser removido é o primeiro da lista
                *removidos = atual->prox;

            } else {
                // o registro a ser removido está no meio ou no final da lista
                anterior->prox = atual->prox;
            }

            free(atual);
            return offset;
        }
        anterior = atual;
        atual = atual->prox;
    }

    // caso não encontre um registro suficientemente grande
    return -1;
}

// Função que retorna uma lista dos registros removidos em ordem crescente 
LISTA* OrdenaRegistrosRem(FILE* arquivoBinario){

    // Pula o status
    fseek(arquivoBinario, 1, SEEK_SET);

    // Guarda a posição atual do ponteiro
    int64_t posicaoAtual = ftell(arquivoBinario);

    int64_t topo;
    LISTA* listaOrdenada = NULL;

    // Leitura do topo da lista de removidos
    fread(&topo, sizeof(int64_t), 1, arquivoBinario);

    if(topo == -1){
        fseek(arquivoBinario, posicaoAtual, SEEK_SET);
        return(listaOrdenada);
    }

    fseek(arquivoBinario, topo, SEEK_SET);
    DADOS registro_dados;

    // Aloca memória para o registro
    AlocaMemoriaRegistro(&registro_dados);

    while(topo != -1){
        if (fread(&registro_dados.removido, sizeof(char), 1, arquivoBinario) != 1 ||
            fread(&registro_dados.tamanhoRegistro, sizeof(int), 1, arquivoBinario) != 1 ||
            fread(&registro_dados.prox, sizeof(int64_t), 1, arquivoBinario) != 1) {

            printf("Erro: falha em ler o próximo registro\n");

            break;
        }

        LISTA* no = (LISTA*)malloc(sizeof(LISTA));
        no->byteOffset = topo;
        no->tamRegistro = registro_dados.tamanhoRegistro;
        no->prox = NULL;

        // Insere ordenado
        InserirOrdenado(&listaOrdenada, no);

        topo = registro_dados.prox;

        if(topo != -1){
            // Restaura a posição original do ponteiro
            fseek(arquivoBinario, topo, SEEK_SET);
        }
    }

    free(registro_dados.nacionalidade);
    free(registro_dados.nomeClube);
    free(registro_dados.nomeJogador);
    
    // Restaura a posição original do ponteiro
    fseek(arquivoBinario, posicaoAtual, SEEK_SET);

    return(listaOrdenada);
}

// Função que campos relacionados ao jogador
void LeituraCampos(DADOS* registro){

    if(registro == NULL){
        return;
    }

    // Leitura id
    scanf("%d", &registro->id);

    char idade[15];
    // Leitura idade
    scanf("%s", idade);
    
    if(strcmp(idade, "NULO") == 0){
        registro->idade = -1;
    } else {
        registro->idade = atoi(idade); 
    }
    // Leitura nomeJogador
    scan_quote_string(registro->nomeJogador);

    // Leitura nacionalidade
    scan_quote_string(registro->nacionalidade);

    // Leitura nomeClube
    scan_quote_string(registro->nomeClube);

    return;
}

// atualiza o tamanho das strings do jogador com base no tamanho real
bool AtualizaTamanhoStrings(DADOS* registro) {
    if (registro == NULL) return false;
    registro->tamNomeJog = strlen(registro->nomeJogador);
    registro->tamNacionalidade = strlen(registro->nacionalidade);
    registro->tamNomeClube = strlen(registro->nomeClube);

    return true;
}

// Função que verifica se deve adicionar ou reutilizar um registro
int* ReutilizarRegistro(FILE* arquivoBinario, CABECALHO* cabecalho, DADOS* registro, int MetodoBestFitOffset, LISTA* removidos) {
    
    // Tamanho do registro removido
    int tamRegRemovido;

    // Registro removido que pode ser usado
    if(MetodoBestFitOffset != -1){

        // Atualiza os registros removidos
        cabecalho->nroRegRem--;

        // Pega o tamanho do registro
        fseek(arquivoBinario, MetodoBestFitOffset, SEEK_SET);
        fread(&registro->removido, sizeof(char), 1, arquivoBinario);
        fread(&tamRegRemovido, sizeof(int), 1, arquivoBinario);

        // Reutiliza o registro removido
        fseek(arquivoBinario, MetodoBestFitOffset, SEEK_SET);
    } else {
        // Adiciona no final do arquivo
        cabecalho->proxByteOffset += registro->tamanhoRegistro;

        // Atualiza o tamanho do registro removido
        tamRegRemovido = registro->tamanhoRegistro;

        // Vai para final do arquivo
        fseek(arquivoBinario, 0, SEEK_END);
    }

    if(removidos && MetodoBestFitOffset == cabecalho->topo) {
        // Atualiza o topo
        cabecalho->topo = removidos->byteOffset;
    }

    int tamRegistroInserido = registro->tamanhoRegistro;
    registro->tamanhoRegistro = tamRegRemovido;

    // Retorna um vetor do registro novo e o removido
    int* tamRegs = (int*)malloc(2*sizeof(int));
    tamRegs[0] = tamRegRemovido;
    tamRegs[1] = tamRegistroInserido;

    return(tamRegs);
}

// Funcao escreve o cabecalho no arquivo binario
bool EscreveDadosJogadorBin(FILE* arquivoBinario, DADOS* registro) {
    // Escreve o registro no arquivo binário
    fwrite(&registro->removido, sizeof(registro->removido), 1, arquivoBinario);
    fwrite(&registro->tamanhoRegistro, sizeof(registro->tamanhoRegistro), 1, arquivoBinario);
    fwrite(&registro->prox, sizeof(registro->prox), 1, arquivoBinario);
    fwrite(&registro->id, sizeof(registro->id), 1, arquivoBinario);
    fwrite(&registro->idade, sizeof(registro->idade), 1, arquivoBinario);
    fwrite(&registro->tamNomeJog, sizeof(registro->tamNomeJog), 1, arquivoBinario);
    fwrite(registro->nomeJogador, sizeof(char), registro->tamNomeJog, arquivoBinario);
    fwrite(&registro->tamNacionalidade, sizeof(registro->tamNacionalidade), 1, arquivoBinario);
    fwrite(registro->nacionalidade, sizeof(char), registro->tamNacionalidade, arquivoBinario);
    fwrite(&registro->tamNomeClube, sizeof(registro->tamNomeClube), 1, arquivoBinario);
    fwrite(registro->nomeClube, sizeof(char), registro->tamNomeClube, arquivoBinario);

    return true;
}

// Função que escreve o lixo no final do registro
void EscreveLixo(FILE* arquivoBinario, int tamRegistroRemovido, int tamRegistroInserido) {
    if (tamRegistroRemovido == tamRegistroInserido){
        return;
    }

    int bytesRestantes = tamRegistroRemovido - tamRegistroInserido;

    for(int i = 0; i < bytesRestantes; i++){
        char lixo = '$';
        fwrite(&lixo, sizeof(char), 1, arquivoBinario);
    }
}

// Função que inicializa o cabecalho
void IniCabecalho(CABECALHO* cabecalho) {
    cabecalho->status = '0';
    cabecalho->topo = -1;
    cabecalho->proxByteOffset = 25;
    cabecalho->nroRegArq = 0;
    cabecalho->nroRegRem = 0;
}