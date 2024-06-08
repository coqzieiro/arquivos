/*
INTEGRANTES DO GRUPO:
                    Felipe da Costa Coqueiro,   NºUSP: 11781361
                    Fernando Alee Suaiden,      NºUSP: 12680836
*/

#include "funcoes_aux.h"
#include "funcoes_fornecidas.h"
#include "funcoes.h"
#include "funcoes1.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>

// Função para leitura do cabeçalho
void LeituraCabecalho(CABECALHO* cabecalho, FILE* arquivoBinario) {
    fseek(arquivoBinario, 0, SEEK_SET);
    fread(&cabecalho->status,         sizeof(cabecalho->status),         1, arquivoBinario);
    fread(&cabecalho->topo,           sizeof(cabecalho->topo),           1, arquivoBinario);    
    fread(&cabecalho->proxByteOffset, sizeof(cabecalho->proxByteOffset), 1, arquivoBinario);  
    fread(&cabecalho->nroRegArq,      sizeof(cabecalho->nroRegArq),      1, arquivoBinario);  
    fread(&cabecalho->nroRegRem,      sizeof(cabecalho->nroRegRem),      1, arquivoBinario);
}

// Função para leitura do registro
void LeituraRegistro(DADOS* registro, FILE* arquivoBinario){
    fread(&registro->removido,        sizeof(registro->removido),         1,                          arquivoBinario);
    fread(&registro->tamanhoRegistro, sizeof(registro->tamanhoRegistro),  1,                          arquivoBinario);
    fread(&registro->prox,            sizeof(registro->prox),             1,                          arquivoBinario);   
    fread(&registro->id,              sizeof(registro->id),               1,                          arquivoBinario);   
    fread(&registro->idade,           sizeof(registro->idade),            1,                          arquivoBinario);
    fread(&registro->tamNomeJog,      sizeof(registro->tamNomeJog),       1,                          arquivoBinario);
    fread(&registro->nomeJogador,     sizeof(char),                      registro->tamNomeJog,        arquivoBinario);
    registro->nomeJogador[registro->tamNomeJog] = '\0';     
    fread(&registro->tamNacionalidade,sizeof(registro->tamNacionalidade), 1,                          arquivoBinario);
    fread(&registro->nacionalidade,   sizeof(char),                      registro->tamNacionalidade,  arquivoBinario);
    registro->nacionalidade[registro->tamNacionalidade] = '\0';
    fread(&registro->tamNomeClube,    sizeof(registro->tamNomeClube),     1,                          arquivoBinario);
    fread(&registro->nomeClube,       sizeof(char),                      registro->tamNomeClube,      arquivoBinario);
    registro->nomeClube[registro->tamNomeClube] = '\0';
}

// Função para escrita no cabecalho
void EscritaCabecalho(CABECALHO* cabecalho, FILE* arquivoBinario){
    fseek(arquivoBinario, 0, SEEK_SET);
    fwrite(&cabecalho->status,           sizeof(cabecalho->status),         1, arquivoBinario);
    fwrite(&cabecalho->topo,             sizeof(cabecalho->topo),           1, arquivoBinario);
    fwrite(&cabecalho->proxByteOffset,   sizeof(cabecalho->proxByteOffset), 1, arquivoBinario);
    fwrite(&cabecalho->nroRegArq,        sizeof(cabecalho->nroRegArq),      1, arquivoBinario);
    fwrite(&cabecalho->nroRegRem,        sizeof(cabecalho->nroRegRem),      1, arquivoBinario);
}

// Função para escrita no registro
void EscritaRegistro(DADOS* registro, FILE* arquivoBinario){
    fwrite(&registro->removido,          sizeof(registro->removido),         1,                         arquivoBinario);
    fwrite(&registro->tamanhoRegistro,   sizeof(registro->tamanhoRegistro),  1,                         arquivoBinario);
    fwrite(&registro->prox,              sizeof(registro->prox),             1,                         arquivoBinario);
    fwrite(&registro->id,                sizeof(registro->id),               1,                         arquivoBinario);
    fwrite(&registro->idade,             sizeof(registro->idade),            1,                         arquivoBinario);
    fwrite(&registro->tamNomeJog,        sizeof(registro->tamNomeJog),       1,                         arquivoBinario);
    fwrite(&registro->nomeJogador,       sizeof(char),                      registro->tamNomeJog,       arquivoBinario);
    fwrite(&registro->tamNacionalidade,  sizeof(registro->tamNacionalidade), 1,                         arquivoBinario);
    fwrite(&registro->nacionalidade,     sizeof(char),                      registro->tamNacionalidade, arquivoBinario);
    fwrite(&registro->tamNomeClube,      sizeof(registro->tamNomeClube),     1,                         arquivoBinario);
    fwrite(&registro->nomeClube,         sizeof(char),                      registro->tamNomeClube,     arquivoBinario);
}

// Função para verificar se um registro corresponde a todos os campos de busca
int todosCamposCorrespondem(DADOS registro, CAMPO_BUSCA camposBusca[], int numCamposBusca) {
    for (int i = 0; i < numCamposBusca; i++) {
        if (strcmp(camposBusca[i].nomeCampo, "id") == 0) { // Campo id
            if (camposBusca[i].valorInt != registro.id) {
                return(0);
            }
        } else if (strcmp(camposBusca[i].nomeCampo, "idade") == 0) { // Campo idade
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

// Função para validar o nome do campo
int validarNomeCampo(const char *nomeCampo) {
    if (strlen(nomeCampo) > MAX_CAMPO) { // Tamanho do campo não pode ser maior MAX_CAMPO
        return(0);
    }
    for (int i = 0; nomeCampo[i] != '\0'; i++) {
        if (!isalnum(nomeCampo[i]) && nomeCampo[i] != '_') {
            return(0);
        }
    }
    return(1);
}

// Função para escrita no cabecalho do arquivo de índices
void EscritaCabecalho_Index(CABECALHO_INDEX* cabecalho_index, FILE* nomeArquivoBinarioDeIndices){
    fwrite(&cabecalho_index->status,           sizeof(cabecalho_index->status),         1, nomeArquivoBinarioDeIndices);
}

// Função para escrita do registro no arquivo de índices
void EscritaRegistro_Index(REGISTRO_INDEX* registro_index, FILE* nomeArquivoBinarioDeIndices) {
    fwrite(&registro_index->id, sizeof(registro_index->id), 1, nomeArquivoBinarioDeIndices);
    fwrite(&registro_index->byteOffset, sizeof(registro_index->byteOffset), 1, nomeArquivoBinarioDeIndices);
}

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
    registro->nomeJogador = (char*)malloc(50*sizeof(char));
    registro->nomeClube = (char*)malloc(50*sizeof(char));
    registro->nacionalidade = (char*)malloc(50*sizeof(char));
}

// Função que lê os inputs do jogador
bool LeDadosJogadorBin(FILE* arquivoBinario, DADOS* registro) {
    // leitura dos jogador
    fread(&(registro->removido), 1, 1, arquivoBinario);     // 1 byte
    fread(&(registro->tamanhoRegistro), 4, 1, arquivoBinario);  // 4 bytes

    // verifica se o registro foi removido
    if (registro->removido == '1') {
        fseek(arquivoBinario, registro->tamanhoRegistro - 5, SEEK_CUR);  // pula os bytes restantes
        return false;
    }

    // leitura dos jogador
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

    return true;
}

// Função para obter o byte offset do último registro removido
int64_t RetornaByteOffSetUltimoRemovido(FILE* arquivoBinario) {
    if (arquivoBinario == NULL) {
        fprintf(stderr, "Erro: ponteiro do arquivo é NULL\n");
        return -1;
    }

    int64_t ultimoRemovido = -1;

    // pula o status
    if (fseek(arquivoBinario, 1, SEEK_SET) != 0) {
        fprintf(stderr, "Erro: fseek falhou ao pular o status\n");
        return -1;
    }

    // armazena o topo
    int64_t topo;
    if (fread(&topo, sizeof(int64_t), 1, arquivoBinario) != 1) {
        fprintf(stderr, "Erro: fread falhou ao ler o topo\n");
        return -1;
    }

    // verifica se o topo é -1
    if (topo == -1) {
        return ultimoRemovido;
    } else {
        ultimoRemovido = topo;
    }

    // pula para o byte offset do topo
    if (fseek(arquivoBinario, topo, SEEK_SET) != 0) {
        fprintf(stderr, "Erro: fseek falhou ao pular para o topo\n");
        return -1;
    }

    DADOS registro_dados;
    AlocaMemoriaRegistro(&registro_dados);
    int64_t byteoffsetVDD = 0;

    // percorre os registros até encontrar o último removido
    while (registro_dados.prox != -1) {
        byteoffsetVDD = ftell(arquivoBinario);

        if (fread(&registro_dados.removido, sizeof(char), 1, arquivoBinario) != 1 ||
            fread(&registro_dados.tamanhoRegistro, sizeof(int), 1, arquivoBinario) != 1 ||
            fread(&registro_dados.prox, sizeof(int64_t), 1, arquivoBinario) != 1) {

            fprintf(stderr, "Erro: fread falhou ao ler o próximo registro\n");
            return -1;
        }

        // pula 8 bytes
        fseek(arquivoBinario, 8, SEEK_CUR);

        if (fread(&registro_dados.tamNomeJog, sizeof(int), 1, arquivoBinario) != 1) {
            fprintf(stderr,
                    "Erro: fread falhou ao ler o tamanho do nome do jogador\n");
            return -1;
        }

        if (registro_dados.tamNomeJog > 0) {
            if (fread(registro_dados.nomeJogador, sizeof(char), registro_dados.tamNomeJog,
                      arquivoBinario) != registro_dados.tamNomeJog) {
                fprintf(stderr,
                        "Erro: fread falhou ao ler o nome do jogador\n");
                return -1;
            }
            registro_dados.nomeJogador[registro_dados.tamNomeJog] =
                '\0';  // garantindo que a string está terminada com nulo
        }

        if (fread(&registro_dados.tamNacionalidade, sizeof(int), 1, arquivoBinario) != 1) {
            fprintf(stderr,
                    "Erro: fread falhou ao ler o tamanho da nacionalidade\n");
            return -1;
        }

        if (registro_dados.tamNacionalidade > 0) {
            if (fread(registro_dados.nacionalidade, sizeof(char),
                      registro_dados.tamNacionalidade,
                      arquivoBinario) != registro_dados.tamNacionalidade) {
                fprintf(stderr, "Erro: fread falhou ao ler a nacionalidade\n");
                return -1;
            }
            registro_dados.nacionalidade[registro_dados.tamNacionalidade] =
                '\0';  // garantindo que a string está terminada com nulo
        }

        if (fread(&registro_dados.tamNomeClube, sizeof(int), 1, arquivoBinario) != 1) {
            fprintf(stderr,
                    "Erro: fread falhou ao ler o tamanho do nome do clube\n");
            return -1;
        }

        if (registro_dados.tamNomeClube > 0) {
            if (fread(registro_dados.nomeClube, sizeof(char), registro_dados.tamNomeClube,
                      arquivoBinario) != registro_dados.tamNomeClube) {
                fprintf(stderr, "Erro: fread falhou ao ler o nome do clube\n");
                return -1;
            }
            registro_dados.nomeClube[registro_dados.tamNomeClube] =
                '\0';  // garantindo que a string está terminada com nulo
        }

        topo = registro_dados.prox;
        fseek(arquivoBinario, registro_dados.prox, SEEK_SET);
    }

    // libera a memória alocada para os campos variáveis
    free(registro_dados.nomeJogador);
    free(registro_dados.nacionalidade);
    free(registro_dados.nomeClube);

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

// essa funcao atualiza os campos secundarios do registro do jogador
void AtualizaCampos(DADOS* registro) {
    registro->tamanhoRegistro = 33 + registro->tamNomeJog + registro->tamNacionalidade + registro->tamNomeClube;
    registro->removido = '0';
    registro->prox = -1;
}

// Usa o algoritmo de ordenar por inserção para ordenar a lista considerando o
// tamanho dos registros e, em caso de empate, o menor byteOffset
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

// libera memória alocada para a lista encadeada
void DesalocaLista(LISTA* cabeca) {
    LISTA* atual = cabeca;
    LISTA* next;

    while (atual != NULL) {
        next = atual->prox;
        free(atual);
        atual = next;
    }
}

// essa função retorna o byte offset do registro a ser substituido no binario
int64_t BestFit(LISTA** removidos, int tamRegistro, FILE* arquivoBinario) {
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
int* ReutilizarRegistro(FILE* arquivoBinario, CABECALHO* cabecalho, DADOS* registro, int bestFitOffset, LISTA* removidos) {
    
    // Tamanho do registro removido
    int tamRegRemovido;

    // Registro removido que pode ser usado
    if(bestFitOffset != -1){

        // Atualiza os registros removidos
        cabecalho->nroRegRem--;

        // Pega o tamanho do registro
        fseek(arquivoBinario, bestFitOffset, SEEK_SET);
        fread(&registro->removido, sizeof(char), 1, arquivoBinario);
        fread(&tamRegRemovido, sizeof(int), 1, arquivoBinario);

        // Reutiliza o registro removido
        fseek(arquivoBinario, bestFitOffset, SEEK_SET);
    } else {
        // Adiciona no final do arquivo
        cabecalho->proxByteOffset += registro->tamanhoRegistro;

        // Atualiza o tamanho do registro removido
        tamRegRemovido = registro->tamanhoRegistro;

        // Vai para final do arquivo
        fseek(arquivoBinario, 0, SEEK_END);
    }

    if(removidos && bestFitOffset == cabecalho->topo) {
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
