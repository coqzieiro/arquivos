/*
INTEGRANTES DO GRUPO:
                    Felipe da Costa Coqueiro,   NºUSP: 11781361
                    Fernando Alee Suaiden,      NºUSP: 12680836
*/

#include "funcoes_aux.h"
#include "funcoes.h"
#include "funcoes1.h"
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

// Função para atualizar o índice após a remoção de um registro
void atualizarIndiceRemocao(FILE *arquivoIndice, CABECALHO *cabecalhoIndice, int idRemovido) {
    fseek(arquivoIndice, 25, SEEK_SET); // Vai para o início dos registros de índice
    int id, byteOffset;
    while (ftell(arquivoIndice) < cabecalhoIndice->proxByteOffset) {
        fread(&id, sizeof(int), 1, arquivoIndice);
        fread(&byteOffset, sizeof(int64_t), 1, arquivoIndice);
        if (id == idRemovido) {
            byteOffset = -1;
            fseek(arquivoIndice, -sizeof(int64_t), SEEK_CUR);
            fwrite(&byteOffset, sizeof(int64_t), 1, arquivoIndice);
            break;
        }
    }
}

// Função para inserir um novo registro na lista de removidos, utilizando a estratégia Best Fit
void inserirNoListaRemovidos(FILE *arquivoDados, CABECALHO *cabecalhoDados, int64_t offsetRemovido, int tamanhoRemovido) {
    fseek(arquivoDados, cabecalhoDados->topo, SEEK_SET);
    DADOS registro;
    int64_t melhorOffset = -1;
    int melhorTamanho = 0;

    while (ftell(arquivoDados) < cabecalhoDados->proxByteOffset) {
        int64_t byteOffset = ftell(arquivoDados);
        leitura_registro(&registro, arquivoDados);
        if (registro.removido == '0' && registro.tamanhoRegistro == tamanhoRemovido) {
            melhorOffset = byteOffset;
            melhorTamanho = registro.tamanhoRegistro;
            break;
        }
        if (registro.removido == '0' && registro.tamanhoRegistro > melhorTamanho) {
            melhorOffset = byteOffset;
            melhorTamanho = registro.tamanhoRegistro;
        }
    }

    if (melhorOffset != -1) {
        fseek(arquivoDados, melhorOffset, SEEK_SET);
        leitura_registro(&registro, arquivoDados);
        registro.removido = '0';
        registro.prox = cabecalhoDados->topo;
        cabecalhoDados->topo = melhorOffset;
        fseek(arquivoDados, melhorOffset, SEEK_SET);
        escrita_registro(&registro, arquivoDados);
    } else {
        cabecalhoDados->topo = offsetRemovido;
    }
}

// Função para escrita no cabecalho
void escrita_cabecalho_index(CABECALHO_INDEX* cabecalho_index, FILE* nomeArquivoBinarioDeIndices){
    fwrite(&cabecalho_index->status,           sizeof(cabecalho_index->status),         1, nomeArquivoBinarioDeIndices);
}

// Função para escrita do registro no arquivo de índices
void escrita_registro_index(REGISTRO_INDEX* registro_index, FILE* nomeArquivoBinarioDeIndices) {
    fwrite(&registro_index->id, sizeof(registro_index->id), 1, nomeArquivoBinarioDeIndices);
    fwrite(&registro_index->byteOffset, sizeof(registro_index->byteOffset), 1, nomeArquivoBinarioDeIndices);
}