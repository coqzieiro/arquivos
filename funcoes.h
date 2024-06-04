#ifndef FUNCOES_H

    #define FUNCOES_H

    #define MAX_CAMPO 50

    #include <stdint.h>
    #include <stdio.h>
    
    // Registro de cabeçalho
    typedef struct {
        char status;
        int64_t topo; // Garante 8 bytes
        int64_t proxByteOffset; // Garante 8 bytes
        int nroRegArq;
        int nroRegRem;
    } CABECALHO;

    // Registro de dados
    typedef struct {
        char removido;
        int tamanhoRegistro;
        int64_t prox; // Garante que é 8 bytes
        int id;
        int idade;
        int tamNomeJog;
        char nomeJogador[40];
        int tamNacionalidade;
        char nacionalidade[40];
        int tamNomeClube;
        char nomeClube[40];
    } DADOS;

    // Armazena informações sobre os campos de busca
    typedef struct {
        char* nomeCampo;
        int valorInt;
        char* valorString;
    } CAMPO_BUSCA;

    // Funcionalidade 1
    void criarArquivoBinario(char* nomeArquivoCSV, char* nomeArquivoBinario);
    // Funcionalidade 2
    void listarRegistros(const char* nomeArquivoBinario);
    // Funcionalidade 3
    void buscarRegistros(const char *arquivoEntrada, int numBuscas);

#endif
