/*
INTEGRANTES DO GRUPO:
                    Felipe da Costa Coqueiro,   NºUSP: 11781361
                    Fernando Alee Suaiden,      NºUSP: 12680836
*/

#include <stdio.h>
#include <stdlib.h>
#include "funcoes.h"
#include "funcoes1.h"
#include "funcoes_fornecidas.h"

int main(void){

    int opcao, numBuscas;
    char nomeArquivoCSV[20];
    char nomeArquivoBinario[20];
    char nomeArquivoIndex[20];

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
        case 4: // funcionalidade 4 (cria arquivo de index)
            scanf("%s %s", nomeArquivoBinario, nomeArquivoIndex);
            criarIndex(nomeArquivoBinario, nomeArquivoIndex);
            break;
        
        case 5: // funcionalidade 5 (remoção lógica do arquivo de dados)
            scanf("%s %s %d", nomeArquivoBinario, nomeArquivoIndex, &numBuscas);
            deletar(nomeArquivoBinario, nomeArquivoIndex, numBuscas);
            break;
        /*case 6: // funcionalidade 6 (inserção de novos registros)
            scanf("%s %s %d", nomeArquivoBinario, nomeArquivoIndex, &numBuscas);
            inserir(nomeArquivoBinario, nomeArquivoIndex, numBuscas);
            break;
        */
        default: 
            printf("Opção inválida.\n");
            return(1);
    }
    return(0);
}
