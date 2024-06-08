#ifndef FUNCOES_H

    #define FUNCOES_H

    #define MAX_CAMPO 50

    #include <stdint.h>
    #include <stdio.h>
    
    // Registro de cabeçalho
    typedef struct {
        char status;
        int64_t topo;
        int64_t proxByteOffset;
        int nroRegArq;
        int nroRegRem;
    } CABECALHO;

    // Registro de dados
    typedef struct {
        char removido;
        int tamanhoRegistro;
        int64_t prox;
        int id;
        int idade;
        int tamNomeJog;
        char* nomeJogador;
        int tamNacionalidade;
        char* nacionalidade;
        int tamNomeClube;
        char* nomeClube;
    } DADOS;

    // Armazena informações sobre os campos de busca
    typedef struct {
        char nomeCampo[MAX_CAMPO];
        int valorInt;
        char valorString[MAX_CAMPO];
    } CAMPO_BUSCA;

    // Funcionalidade 1
    void criarArquivoBinario(char* nomeArquivoCSV, char* nomeArquivoBinario);

    // Funcionalidade 2
    void listarRegistros(const char* nomeArquivoBinario);

    // Funcionalidade 3
    void buscarRegistros(const char *arquivoEntrada, int numBuscas);

#endif
