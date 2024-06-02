#ifndef FUNCOES_AUX_H
    #define FUNCOES_AUX_H

    #include "funcoes.h"
    #include "funcoes1.h"
    #include <stdio.h>

    // Leitura de cabeçalho e registros
    void leitura_cabecalho(CABECALHO* cabecalho, FILE* arquivoBinario);
    void leitura_registro(DADOS* registro, FILE* arquivoBinario);

    // Escrita de cabeçalho e registros
    void escrita_cabecalho(CABECALHO* cabecalho, FILE* arquivoBinario);
    void escrita_registro(DADOS* registro, FILE* arquivoBinario);
    
    // Correspondecia de campos
    int todosCamposCorrespondem(DADOS registro, CAMPO_BUSCA camposBusca[], int numCamposBusca);
    int validarNomeCampo(const char *nomeCampo);
    
    // Escrita no index
    void escrita_cabecalho_index(CABECALHO_INDEX* cabecalho_index, FILE* nomeArquivoBinarioDeIndices);
    void escrita_registro_index(REGISTRO_INDEX* registro_index, FILE* nomeArquivoBinarioDeIndices);

    // Funções auxiliares de remoção
    void inserirNoListaRemovidos(FILE *arquivoDados, CABECALHO *cabecalhoDados, int64_t offsetRemovido, int tamanhoRemovido);
    void atualizarIndiceRemocao(FILE *arquivoIndice, CABECALHO *cabecalhoIndice, int idRemovido);

#endif