#ifndef FUNCOES_H

    #define FUNCOES_H

    #define MAX_CAMPO 50

    #include <stdint.h>
    #include <stdio.h>
    
    // Registro de cabeçalho
    typedef struct {
        char status;
        long int topo; // Garante 8 bytes
        long int proxByteOffset; // Garante 8 bytes
        int nroRegArq;
        int nroRegRem;
    } CABECALHO;

    // Registro de dados
    typedef struct {
        char removido;
        int tamanhoRegistro;
        long int prox; // Garante que é 8 bytes
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
