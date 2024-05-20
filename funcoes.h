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
        char nomeCampo[MAX_CAMPO];
        int valorInt;
        char valorString[MAX_CAMPO];
    } CAMPO_BUSCA;

    void criarArquivoBinario(char* nomeArquivoCSV, char* nomeArquivoBinario);
    void listarRegistros(const char* nomeArquivoBinario);
    void buscarRegistros(const char *arquivoEntrada, int numBuscas);
    void leitura_cabecalho(CABECALHO* cabecalho, FILE* arquivoBinario);
    void leitura_registro(DADOS* registro, FILE* arquivoBinario);
    void escrita_cabecalho(CABECALHO* cabecalho, FILE* arquivoBinario);
    void escrita_registro(DADOS* registro, FILE* arquivoBinario);

#endif
