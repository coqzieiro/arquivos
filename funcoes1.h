#ifndef FUNCOES1_H

    #define FUNCOES1_H
    #include <stdint.h>
    
    // Registro de index do cabeçalho
    typedef struct {
        char status;
    } CABECALHO_INDEX;

    // Registro de index do registro
    typedef struct {
        int id;
        int64_t byteOffset;
    } REGISTRO_INDEX;

    // Funcionalidade 4
    void criarIndex(char* nomeArquivoBinario, char* nomeArquivoBinDeIndices);
    // Funcionalidade 5
    void remover(char *nomeArquivoDados, char *nomeArquivoIndice, int numRemocoes);
    // Funcionalidade 6
    void inserir(char* nomeArquivoBinario, char* nomeArquivoIndex, int numBuscas);

#endif