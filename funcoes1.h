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

    // Escrita index
    void escrita_cabecalho_index(CABECALHO_INDEX* cabecalho_index, FILE* nomeArquivoBinarioDeIndices);
    void escrita_registro_index(REGISTRO_INDEX* registro_index, FILE* nomeArquivoBinarioDeIndices);

    // Funções auxiliar para remoção
    void removerRegistroLogico(FILE *arquivoBinario, int64_t offset, CABECALHO *cabecalho);
    int buscarERemover(FILE *arquivoBinario, CABECALHO *cabecalho, CAMPO_BUSCA *criterio, int x);

    // Atualização cabeçalho e index
    void atualizarCabecalho(CABECALHO *cabecalho, FILE *arquivoBinario);
    void atualizarIndice(const char *arquivoIndice, int id);

    // Funcionalidades 4, 5 e 6
    void criarIndex(char* nomeArquivoBinario, char* nomeArquivoBinDeIndices);
    void deletar(char* nomeArquivoBinario, char* nomeArquivoIndex, int numBuscas, CAMPO_BUSCA criterios[][10], int x[]);

    /*void inserir(nomeArquivoBinario, nomeArquivoIndex, numBuscas);*/

#endif