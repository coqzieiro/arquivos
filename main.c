/*
INTEGRANTES DO GRUPO:
                    Felipe da Costa Coqueiro,   NºUSP: 11781361
                    Fernando Alee Suaiden,      NºUSP: 12680836
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
        case 5:  // funcionalidade 5 (remoção lógica do arquivo de dados)
            scanf("%s %s %d", nomeArquivoBinario, nomeArquivoIndex, &numBuscas);
            CAMPO_BUSCA criterios[10][10]; // Supondo no máximo 10 critérios com no máximo 10 campos cada
            int x[10]; // Número de campos para cada critério

            for (int i = 0; i < numBuscas; i++) {
                scanf("%d", &x[i]); // Lê o número de campos para o critério atual
                for (int j = 0; j < x[i]; j++) {
                    char campo[20];
                    scanf("%s", campo); // Lê o nome do campo

                    if (strcmp(campo, "id") == 0 || strcmp(campo, "idade") == 0) {
                        scanf("%d", &criterios[i][j].valorInt); // Lê o valor inteiro do campo
                    } else {
                        scan_quote_string(criterios[i][j].valorString); // Lê o valor da string do campo
                    }

                    strcpy(criterios[i][j].nomeCampo, campo); // Copia o nome do campo
                }
            }
            deletar(nomeArquivoBinario, nomeArquivoIndex, numBuscas, criterios, x);
            break;
        /*case 6: // funcionalidade 6 (inserção de novos registros)
            scanf("%s %s %d", nomeArquivoBinario, nomeArquivoIndex, &numBuscas);
            inserir(nomeArquivoBinario, nomeArquivoIndex, numBuscas);
            break;*/
        default: 
            printf("Opção inválida.\n");
            return(1);
    }
    return(0);
}