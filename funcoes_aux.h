#ifndef FUNCOES_AUX_H
    #define FUNCOES_AUX_H

    #include "funcoes.h"
    #include "funcoes1.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>

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
    void InicializaRegistroJogador(DADOS* registro);
    void AlocaMemoriaJogador(DADOS* registro);
    bool LeDadosJogadorBin(FILE* arquivoBinario, DADOS* registro);
    long int RetornaByteOffSetUltimoRemovido(FILE* arquivoBinario);
    void LiberaMemoriaChar(char** nomeCampo, char** valorCampo, int numCamposBusca);
    void DesalocaMemoriaJogador(DADOS* registro);
    void LeituraParametrosBusca(int numCamposBusca, char** nomeCampo, char** valorCampo);

    // Funções auxiliares de inserção
    void AtualizaCampos(DADOS* registro);
    LISTABYTE* OrdenaRegistrosRemovidos(FILE* arquivoBinario);
    void sortedInsert(LISTABYTE** cabeca, LISTABYTE* novoNo);
    long int BestFitRegister(LISTABYTE* removidos, int tamanhoRegistro);
    bool ReescreveRegistrosRemovidosBIN(FILE* arquivoBinario, LISTABYTE* removidos);
    void LiberaLista(LISTABYTE* cabeca);
    long int BestFitRegister(LISTABYTE* removidos, int tamRegistro);
    LISTABYTE* OrdenaRegistrosRemovidos(FILE* nomeArquivoBinario);
    void AdicionaRegistroRemovidoOrdenado(LISTABYTE** lista, int tamanhoRegistro, long int offset);
    
#endif
