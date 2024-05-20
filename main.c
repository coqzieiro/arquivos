/*
INTEGRANTES DO GRUPO:
                    Felipe da Costa Coqueiro,   NºUSP: 11781361
                    Fernando Alee Suaiden,      NºUSP: 12680836
*/

#include <stdio.h>
#include <stdlib.h>
#include "funcoes.h"
#include "funcoes_fornecidas.h"

int main(void){

    int opcao, numBuscas;
    char nomeArquivoCSV[20];
    char nomeArquivoBinario[20];

    scanf("%d", &opcao); // Determina qual funcionalidade irá executar

    switch(opcao){
        case 1: // funcionalidade 1 (cria arquivo binário)
            scanf("%s %s", nomeArquivoCSV, nomeArquivoBinario);
            criarArquivoBinario(nomeArquivoCSV, nomeArquivoBinario);
            binarioNaTela(nomeArquivoBinario);
            break;
        case 2: // funcionalidade 2 (lista os registros)
            scanf("%s", nomeArquivoBinario);
            listarRegistros(nomeArquivoBinario);
            break;
        case 3: // funcionalidade 3 (faz a busca por campos)
            scanf("%s %d", nomeArquivoBinario, &numBuscas);
            buscarRegistros(nomeArquivoBinario, numBuscas);
            break;
        default: 
            printf("Opção inválida.\n");
            return(1);
    }
    return(0);
}
