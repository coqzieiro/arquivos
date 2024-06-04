/*
INTEGRANTES DO GRUPO:
                    Felipe da Costa Coqueiro,   NºUSP: 11781361
                    Fernando Alee Suaiden,      NºUSP: 12680836
*/

#include "funcoes_aux.h"
#include "funcoes.h"
#include "funcoes1.h"
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Função para leitura do cabeçalho
void leitura_cabecalho(CABECALHO* cabecalho, FILE* arquivoBinario) {
    fread(&cabecalho->status,         sizeof(cabecalho->status),         1, arquivoBinario);
    fread(&cabecalho->topo,           sizeof(cabecalho->topo),           1, arquivoBinario);    
    fread(&cabecalho->proxByteOffset, sizeof(cabecalho->proxByteOffset), 1, arquivoBinario);  
    fread(&cabecalho->nroRegArq,      sizeof(cabecalho->nroRegArq),      1, arquivoBinario);  
    fread(&cabecalho->nroRegRem,      sizeof(cabecalho->nroRegRem),      1, arquivoBinario);
}

// Função para leitura do registro
void leitura_registro(DADOS* registro, FILE* arquivoBinario){
    fread(&registro->removido,        sizeof(registro->removido),         1,                          arquivoBinario);
    fread(&registro->tamanhoRegistro, sizeof(registro->tamanhoRegistro),  1,                          arquivoBinario);  
    fread(&registro->prox,            sizeof(registro->prox),             1,                          arquivoBinario);   
    fread(&registro->id,              sizeof(registro->id),               1,                          arquivoBinario);   
    fread(&registro->idade,           sizeof(registro->idade),            1,                          arquivoBinario);
    fread(&registro->tamNomeJog,      sizeof(registro->tamNomeJog),       1,                          arquivoBinario);
    registro->nomeJogador[registro->tamNomeJog] ='\0';     
    fread(&registro->nomeJogador,     sizeof(char),                      registro->tamNomeJog,        arquivoBinario);
    fread(&registro->tamNacionalidade,sizeof(registro->tamNacionalidade), 1,                          arquivoBinario);
    registro->nacionalidade[registro->tamNacionalidade] ='\0';
    fread(&registro->nacionalidade,   sizeof(char),                      registro->tamNacionalidade,  arquivoBinario);
    fread(&registro->tamNomeClube,    sizeof(registro->tamNomeClube),     1,                          arquivoBinario);
    registro->nomeClube[registro->tamNomeClube] ='\0';
    fread(&registro->nomeClube,       sizeof(char),                      registro->tamNomeClube,      arquivoBinario);
}

// Função para escrita no cabecalho
void escrita_cabecalho(CABECALHO* cabecalho, FILE* arquivoBinario){
    fwrite(&cabecalho->status,           sizeof(cabecalho->status),         1, arquivoBinario);
    fwrite(&cabecalho->topo,             sizeof(cabecalho->topo),           1, arquivoBinario);
    fwrite(&cabecalho->proxByteOffset,   sizeof(cabecalho->proxByteOffset), 1, arquivoBinario);
    fwrite(&cabecalho->nroRegArq,        sizeof(cabecalho->nroRegArq),      1, arquivoBinario);
    fwrite(&cabecalho->nroRegRem,        sizeof(cabecalho->nroRegRem),      1, arquivoBinario);
}

// Função para escrita no registro
void escrita_registro(DADOS* registro, FILE* arquivoBinario){
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
    for (int i = 0; nomeCampo[i] != '\0'; i++) {
        if (!isalnum(nomeCampo[i]) && nomeCampo[i] != '_') {
            return(0);
        }
    }
    return(1);
}

// Função para escrita no cabecalho de index
void escrita_cabecalho_index(CABECALHO_INDEX* cabecalhoIndex, FILE* arquivoBinarioIndex){
    fwrite(&cabecalhoIndex->status,           sizeof(cabecalhoIndex->status),         1, arquivoBinarioIndex);
}

// Função para leitura no cabecalho do index
void leitura_cabecalho_index(CABECALHO_INDEX* cabecalhoIndex, FILE* arquivoBinarioIndex){
    fread(&cabecalhoIndex->status,         sizeof(cabecalhoIndex->status),         1, arquivoBinarioIndex);
}

// Função para escrita do registro no arquivo de índices
void escrita_registro_index(REGISTRO_INDEX* registroIndex, FILE* arquivoBinarioIndex) {
    fwrite(&registroIndex->id, sizeof(registroIndex->id), 1, arquivoBinarioIndex);
    fwrite(&registroIndex->byteOffset, sizeof(registroIndex->byteOffset), 1, arquivoBinarioIndex);
}

// Função auxiliar para buscar registros no índice
int64_t buscarNoIndice(FILE *arquivoIndice, int id) {
    fseek(arquivoIndice, sizeof(char), SEEK_SET); // Pular o status do cabeçalho
    REGISTRO_INDEX regIndex;
    while (fread(&regIndex, sizeof(REGISTRO_INDEX), 1, arquivoIndice) == 1) {
        if (regIndex.id == id) {
            return regIndex.byteOffset;
        }
    }
    return -1; // Não encontrado
}

// Função auxiliar para reescrever o índice após remoção
void reescreverIndiceAposRemocao(FILE *arquivoIndice, CABECALHO_INDEX *cabecalhoIndice, int idRemovido) {
    fseek(arquivoIndice, sizeof(char), SEEK_SET); // Pular o status do cabeçalho
    FILE *tempFile = tmpfile(); // Arquivo temporário para armazenar o índice atualizado
    if (!tempFile) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    REGISTRO_INDEX regIndex;
    while (fread(&regIndex, sizeof(REGISTRO_INDEX), 1, arquivoIndice) == 1) {
        if (regIndex.id != idRemovido) {
            fwrite(&regIndex, sizeof(REGISTRO_INDEX), 1, tempFile);
        }
    }
    rewind(arquivoIndice);
    fwrite(&cabecalhoIndice->status, sizeof(char), 1, arquivoIndice); // Reescrever o status
    rewind(tempFile);
    while (fread(&regIndex, sizeof(REGISTRO_INDEX), 1, tempFile) == 1) {
        fwrite(&regIndex, sizeof(REGISTRO_INDEX), 1, arquivoIndice);
    }
    fclose(tempFile);
}

// Função auxiliar para inserir na lista de removidos
void inserirNoListaRemovidos(FILE *arquivoDados, CABECALHO *cabecalhoDados, int64_t offset, int tamanho) {
    // Mover para o offset e marcar como removido
    fseek(arquivoDados, offset, SEEK_SET);
    char removido = '0';
    fwrite(&removido, sizeof(char), 1, arquivoDados);
    fwrite(&tamanho, sizeof(int), 1, arquivoDados);
    fwrite(&cabecalhoDados->topo, sizeof(int64_t), 1, arquivoDados);
    cabecalhoDados->topo = offset;
}
