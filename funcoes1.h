#ifndef FUNCOES1_H

    #define FUNCOES1_H

    #include <stdint.h>
    #include <stdbool.h>

    #define TAM_INICIAL_BYTEOFFSET 25
    
    // Registro de index do cabeçalho
    typedef struct {
        char status;
    } CABECALHO_INDEX;

    // Registro de index do registro
    typedef struct {
        int id;
        int64_t byteOffset;
    } REGISTRO_INDEX;

    typedef struct ListaByteOffSet {
        long int byteOffset;
        int tamRegistro;
        struct ListaByteOffSet *prox;
    } LISTABYTE;

    // Funcionalidade 4
    int criarIndex(char* nomeArquivoBinario, char* nomeArquivoBinDeIndices, int opcao);
    // Funcionalidade 5
    bool remover(FILE* nomeArquivoBinario, FILE* nomeArquivoBinDeIndices, int numBuscas);
    // Funcionalidade 6
    void inserir(char* nomeArquivoBinario, char* nomeArquivoIndex, int numBuscas);

#endif
