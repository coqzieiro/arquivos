#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcoes.h"
#include "funcoes1.h"
#include "funcoes_fornecidas.h"

int main(void){
    int opcao, numBuscas, numRemocoes, numInsercoes;
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
            if (criarIndex(nomeArquivoBinario, nomeArquivoIndex, opcao)) {
                binarioNaTela(nomeArquivoIndex); // Só imprime se criarIndex retornar sucesso
            }
            break;
        case 5: { // funcionalidade 5 (remoção lógica do arquivo de dados)
            scanf("%s %s %d", nomeArquivoBinario, nomeArquivoIndex, &numRemocoes);
            // abertura do arquivo
            FILE* arquivoBinario = fopen(nomeArquivoBinario, "rb+");
            FILE* arquivoIndex = fopen(nomeArquivoIndex, "rb+");

            if (arquivoBinario == NULL) {
                printf("Falha no processamento do arquivo.\n");
                return 0;
            }

            if(arquivoIndex == NULL) {
                // caso o arquivo de índice não exista, cria um novo
                arquivoIndex = fopen(nomeArquivoIndex, "wb");
            }

            int qtdParametros;

            for(int i = 0; i < numRemocoes; i++) {
                scanf("%d", &qtdParametros);
                remover(arquivoBinario, arquivoIndex, qtdParametros);
            }

            criarIndex(nomeArquivoBinario, nomeArquivoIndex, opcao);

                    // Fechar os arquivos
            fclose(arquivoBinario);
            fclose(arquivoIndex);

            binarioNaTela(nomeArquivoBinario);
            binarioNaTela(nomeArquivoIndex);

            break;
        }
        case 6: // funcionalidade 6 (inserção de novos registros)
            scanf("%s %s %d", nomeArquivoBinario, nomeArquivoIndex, &numInsercoes);
            // abertura do arquivo
            FILE* arquivoBinario = fopen(nomeArquivoBinario, "rb+");
            FILE* arquivoIndex = fopen(nomeArquivoIndex, "rb+");

            if (arquivoBinario == NULL) {
                printf("Falha no processamento do arquivo.\n");
                return 0;
            }

            if(arquivoIndex == NULL) {
                // caso o arquivo de índice não exista, cria um novo
                arquivoIndex = fopen(nomeArquivoIndex, "wb");
            }

            int qtdParametros;

            for(int i = 0; i < numInsercoes; i++) {
                scanf("%d", &qtdParametros);
                inserir(arquivoBinario);
            }

            criarIndex(nomeArquivoBinario, nomeArquivoIndex, opcao);

            // Fechar os arquivos
            fclose(arquivoBinario);
            fclose(arquivoIndex);

            binarioNaTela(nomeArquivoBinario);
            binarioNaTela(nomeArquivoIndex);

            break;
        default: 
            printf("Opção inválida.\n");
            return(1);
    }
    return(0);
}