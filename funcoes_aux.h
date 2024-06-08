#ifndef FUNCOES_AUX_H
    #define FUNCOES_AUX_H

    #include "funcoes.h"
    #include "funcoes1.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>

    // Leitura de cabeçalho e registros
    void LeituraCabecalho(CABECALHO* cabecalho, FILE* arquivoBinario);
    void LeituraRegistro(DADOS* registro, FILE* arquivoBinario);

    // Escrita de cabeçalho e registros
    void EscritaCabecalho(CABECALHO* cabecalho, FILE* arquivoBinario);
    void EscritaRegistro(DADOS* registro, FILE* arquivoBinario);
    
    // Correspondecia de campos
    int todosCamposCorrespondem(DADOS registro, CAMPO_BUSCA camposBusca[], int numCamposBusca);
    int validarNomeCampo(const char *nomeCampo);
    
    // Escrita no index
    void EscritaCabecalho_Index(CABECALHO_INDEX* cabecalho_index, FILE* nomeArquivoBinarioDeIndices);
    void EscritaRegistro_Index(REGISTRO_INDEX* registro_index, FILE* nomeArquivoBinarioDeIndices);

    // Funções auxiliares de remoção
    void InicializaRegistroJogador(DADOS* registro);
    void AlocaMemoriaRegistro(DADOS* registro);
    bool LeDadosJogadorBin(FILE* arquivoBinario, DADOS* registro);
    int64_t RetornaByteOffSetUltimoRemovido(FILE* arquivoBinario);
    void LiberaMemoriaChar(char** nomeCampo, char** valorCampo, int numCamposBusca);
    void DesAlocaMemoriaRegistro(DADOS* registro);
    void LeituraParametrosBusca(int numCamposBusca, char** nomeCampo, char** valorCampo);

    // Funções auxiliares de inserção
    void AtualizaCampos(DADOS* registro);
    LISTA* OrdenaRegistrosRemovidos(FILE* arquivoBinario);
    void sortedInsert(LISTA** cabeca, LISTA* novoNo);
    int64_t BestFitRegister(LISTA** removidos, int tamRegistro, FILE* arquivoBinario);
    bool ReescreveRegistrosRemovidosBIN(FILE* arquivoBinario, LISTA* removidos);
    void LiberaLista(LISTA* cabeca);
    void LerInputDadosJogador(DADOS* registro);
    bool AtualizaTamanhoStrings(DADOS* registro);
    int* ReutilizarOuAdicionarRegistro(FILE* arquivoBinario, CABECALHO* cabecalho, DADOS* registro, int bestFitOffset, LISTA* removidos);
    bool EscreveDadosJogadorBin(FILE* arquivoBinario, DADOS* registro);
    bool EscreveNoFinal(FILE* arquivoBinario, int tamRegistroAntigo, int tamRegsInserido);
    void IniCabecalho(CABECALHO* cabecalho);

#endif