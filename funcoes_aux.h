#ifndef FUNCOES_AUX_H
    #define FUNCOES_AUX_H

    #include "funcoes.h"
    #include "funcoes1.h"
    #include <stdio.h>

    // Leitura e escrita de cabeçalho
    void leitura_cabecalho(CABECALHO* cabecalho, FILE* arquivoBinario);
    void escrita_cabecalho(CABECALHO* cabecalho, FILE* arquivoBinario);

    // Leitura e escrita de registros
    void leitura_registro(DADOS* registro, FILE* arquivoBinario);
    void escrita_registro(DADOS* registro, FILE* arquivoBinario);

    // Leitura e escrita de cabeçalho index
    void leitura_cabecalho_index(CABECALHO_INDEX* cabecalho, FILE* arquivoBinario);
    void escrita_cabecalho_index(CABECALHO_INDEX* cabecalho, FILE* arquivoBinario);

    // Escrita no index
    void escrita_registro_index(REGISTRO_INDEX* registroIndex, FILE* arquivoBinarioIndex);

    // Correspondecia de campos
    int todosCamposCorrespondem(DADOS registro, CAMPO_BUSCA camposBusca[], int numCamposBusca);
    int validarNomeCampo(const char *nomeCampo);

    // Funções auxiliares de remoção
    void inserirNoListaRemovidos(FILE *arquivoDados, CABECALHO *cabecalhoDados, int64_t offset, int tamanho);
    void reescreverIndiceAposRemocao(FILE *arquivoIndice, CABECALHO_INDEX *cabecalhoIndice, int idRemovido);
    int64_t buscarNoIndice(FILE *arquivoIndice, int id);

#endif
