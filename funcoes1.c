#include "funcoes.h"
#include "funcoes1.h"
#include "funcoes_aux.h"
#include "funcoes_fornecidas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Função para criar index a partir do arquivo binário
int criarIndex(char* nomeArquivoBinario, char* nomeArquivoBinDeIndices, int opcao){
    // Abertura dos arquivos
    FILE* arquivoBinario = fopen(nomeArquivoBinario, "rb");
    if (arquivoBinario == NULL){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // Abre para escrita
    FILE* arquivoBinarioDeIndices = fopen(nomeArquivoBinDeIndices, "wb");
    if (arquivoBinarioDeIndices == NULL){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }
    // Inicialização do cabeçalho
    CABECALHO_INDEX cabecalho_index;

    cabecalho_index.status = '0';

    // Escrita do cabeçalho no arquivo binário de índices
    escrita_cabecalho_index(&cabecalho_index, arquivoBinarioDeIndices);

    // Variáveis para armazenar dados do registro
    REGISTRO_INDEX registro_index;

    char statusCabecalhoArquivoBinario;

    fread(&statusCabecalhoArquivoBinario, sizeof(char), 1, arquivoBinario);

    // Verifica a consistência do arquivo
    if(statusCabecalhoArquivoBinario == '0'){
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    // Pula o cabeçalho do arquivo de dados
    fseek(arquivoBinario, 25, SEEK_SET);

    // Variável para calcular o byteOffset
    int64_t byteOffset = 25; // Começa após o cabeçalho

    // Só para o loop quando encontrar o fim do arquivo
    while (1) {
        char removido;
        int tamanhoRegistroInteiro;

        // Tente ler o campo removido
        if (fread(&removido, sizeof(char), 1, arquivoBinario) != 1) {
            break; // Sai do loop se não conseguir ler
        }

        // Tente ler o tamanho do registro
        if (fread(&tamanhoRegistroInteiro, sizeof(int), 1, arquivoBinario) != 1) {
            break; // Sai do loop se não conseguir ler
        }

        // Pula o campo 'prox' (8 bytes) e lê 'id' (4 bytes)
        fseek(arquivoBinario, 8, SEEK_CUR);
        if (fread(&registro_index.id, sizeof(int), 1, arquivoBinario) != 1) {
            break; // Sai do loop se não conseguir ler
        }

        // Verifica se o registro não foi removido
        if (removido != '1') {
            // Define o byteOffset do registro atual
            registro_index.byteOffset = byteOffset;

            // Escreve o registro de índice no arquivo binário de índices
            escrita_registro_index(&registro_index, arquivoBinarioDeIndices);
        }

        // Atualiza o byteOffset para o próximo registro
        byteOffset += tamanhoRegistroInteiro;

        // Move o ponteiro do arquivo para o próximo registro
        fseek(arquivoBinario, byteOffset, SEEK_SET);

    }
    // Deu tudo certo
    cabecalho_index.status = '1';

    // Reescrita do cabeçalho com as informações corretas
    fseek(arquivoBinarioDeIndices, 0, SEEK_SET);
    
    // escrita do cabeçalho no arquivo binário de índices
    escrita_cabecalho_index(&cabecalho_index, arquivoBinarioDeIndices);

    // Fechamento dos arquivos
    fclose(arquivoBinario);
    fclose(arquivoBinarioDeIndices);
    
    return 1;
}

// remove um ou mais registros do arquivo binario BIN IndiceBIN numBuscas
bool remover(FILE* nomeArquivoBinario, FILE* nomeArquivoBinDeIndices, int numBuscas) {
    // Variáveis auxiliares para leitura dos parâmetros de busca
    char** nomeCampo = (char**)malloc(numBuscas * sizeof(char*));
    for (int i = 0; i < numBuscas; i++) {
        nomeCampo[i] = (char*)malloc(14 * sizeof(char));
    }

    char** valorCampo = (char**)malloc(numBuscas * sizeof(char*));
    for (int i = 0; i < numBuscas; i++) {
        valorCampo[i] = (char*)malloc(40 * sizeof(char));
    }

    // Lendo os parâmetros de busca
    LeituraParametrosBusca(numBuscas, nomeCampo, valorCampo);

    // Variáveis auxiliares para leitura dos registros
    DADOS registro_dados;
    InicializaRegistroJogador(&registro_dados);
    AlocaMemoriaJogador(&registro_dados);

    int registrosEncontrados = 0;
    long long prox;

    // Saltar o cabeçalho
    fseek(nomeArquivoBinario, TAM_INICIAL_BYTEOFFSET, SEEK_SET);

    while (fread(&prox, sizeof(long long), 1, nomeArquivoBinario) == 1) {
        fseek(nomeArquivoBinario, -sizeof(long long), SEEK_CUR);  // Retorna para ler o registro completo

        // Lê o byte offset atual
        long int byteOffset = ftell(nomeArquivoBinario);

        if (!LeDadosJogadorBin(nomeArquivoBinario, &registro_dados)) {
            continue;
        }

        int contadorCampo = 0;

        // Verificar os critérios de busca
        for (int j = 0; j < numBuscas; j++) {
            if (strcmp(nomeCampo[j], "id") == 0 && registro_dados.id == atoi(valorCampo[j])) {
                contadorCampo++;

                // Vai no arquivo de índice e procura o byteoffset do registro do ID
                fseek(nomeArquivoBinDeIndices, 0, SEEK_SET);
                int id;
                long int byteOffsetIndice;
                while (fread(&id, sizeof(int), 1, nomeArquivoBinDeIndices) == 1) {
                    fread(&byteOffsetIndice, sizeof(long int), 1, nomeArquivoBinDeIndices);
                    if (id == registro_dados.id) {

                        // Remoção do registro no arquivo BIN
                        int registrosRemovidos = RetornaByteOffSetUltimoRemovido(nomeArquivoBinario);

                        if (registrosRemovidos != -1) {
                            fseek(nomeArquivoBinario, registrosRemovidos + 5, SEEK_SET);
                            fwrite(&byteOffsetIndice, sizeof(long int), 1, nomeArquivoBinario);

                            fseek(nomeArquivoBinario, byteOffsetIndice, SEEK_SET);
                            char removido = '1';
                            fwrite(&removido, sizeof(char), 1, nomeArquivoBinario);
                        } else {
                            // Vai pro byteoffset do registro que quer remover
                            fseek(nomeArquivoBinario, byteOffsetIndice, SEEK_SET);

                            // Escreve o campo removido
                            char removido = '1';
                            fwrite(&removido, sizeof(char), 1, nomeArquivoBinario);

                            // Volta pro começo do registro pulando status e lê o topo
                            fseek(nomeArquivoBinario, 1, SEEK_SET);
                            long int topo;
                            fread(&topo, sizeof(long int), 1, nomeArquivoBinario);

                            // Volta para o registro que quer remover mais 5 bytes
                            fseek(nomeArquivoBinario, byteOffsetIndice + 5, SEEK_SET);

                            // Escreve o topo no campo prox
                            fwrite(&topo, sizeof(long int), 1, nomeArquivoBinario);

                            // Volta pro começo do registro
                            fseek(nomeArquivoBinario, byteOffsetIndice, SEEK_SET);

                            // Escreve o byteoffset no topo
                            fseek(nomeArquivoBinario, 1, SEEK_SET);
                            fwrite(&byteOffsetIndice, sizeof(long int), 1, nomeArquivoBinario);
                        }
                        break;
                    }
                }

            } else if (strcmp(nomeCampo[j], "idade") == 0 && registro_dados.idade == atoi(valorCampo[j])) {
                contadorCampo++;
            } else if (strcmp(nomeCampo[j], "nomeJogador") == 0 && registro_dados.tamNomeJog != 0 && strcmp(registro_dados.nomeJogador, valorCampo[j]) == 0) {
                contadorCampo++;
            } else if (strcmp(nomeCampo[j], "nacionalidade") == 0 && registro_dados.tamNacionalidade != 0 && strcmp(registro_dados.nacionalidade, valorCampo[j]) == 0) {
                contadorCampo++;
            } else if (strcmp(nomeCampo[j], "nomeClube") == 0 && registro_dados.tamNomeClube != 0 && strcmp(registro_dados.nomeClube, valorCampo[j]) == 0) {
                contadorCampo++;
            }
        }

        // Se todos os critérios forem atendidos
        if (contadorCampo == numBuscas) {
            long int MantemComoTava = ftell(nomeArquivoBinario);

            int registrosRemovidos = RetornaByteOffSetUltimoRemovido(nomeArquivoBinario);

            if (registrosRemovidos != -1) {
                fseek(nomeArquivoBinario, registrosRemovidos + 5, SEEK_SET);
                fwrite(&byteOffset, sizeof(long int), 1, nomeArquivoBinario);

                fseek(nomeArquivoBinario, byteOffset, SEEK_SET);
                char removido = '1';
                fwrite(&removido, sizeof(char), 1, nomeArquivoBinario);
            } else {
                // Vai pro byteoffset do registro que quer remover
                fseek(nomeArquivoBinario, byteOffset, SEEK_SET);

                // Escreve o campo removido
                char removido = '1';
                fwrite(&removido, sizeof(char), 1, nomeArquivoBinario);

                // Volta pro começo do registro pulando status e lê o topo
                fseek(nomeArquivoBinario, 1, SEEK_SET);
                long int topo;
                fread(&topo, sizeof(long int), 1, nomeArquivoBinario);

                // Volta para o registro que quer remover mais 5 bytes
                fseek(nomeArquivoBinario, byteOffset + 5, SEEK_SET);

                // Escreve o topo no campo prox
                fwrite(&topo, sizeof(long int), 1, nomeArquivoBinario);

                // Volta pro começo do registro
                fseek(nomeArquivoBinario, byteOffset, SEEK_SET);

                // Escreve o byteoffset no topo
                fseek(nomeArquivoBinario, 1, SEEK_SET);
                fwrite(&byteOffset, sizeof(long int), 1, nomeArquivoBinario);
            }

            RetornaByteOffSetUltimoRemovido(nomeArquivoBinario);

            fseek(nomeArquivoBinario, MantemComoTava, SEEK_SET);

            registrosEncontrados++;
        }
    }

    // Caso o registro não seja encontrado
    if (registrosEncontrados == 0) {
        LiberaMemoriaChar(nomeCampo, valorCampo, numBuscas);
        DesalocaMemoriaJogador(&registro_dados);
        return false;
    }

    // Libera memória alocada
    LiberaMemoriaChar(nomeCampo, valorCampo, numBuscas);
    DesalocaMemoriaJogador(&registro_dados);

    return true;
}

// Função para inserir novo registro no arquivo binário
bool inserir(FILE* nomeArquivoBinario) {
    if (nomeArquivoBinario == NULL) {
        fprintf(stderr, "Erro: ponteiro do arquivo é NULL\n");
        return false;
    }

    DADOS registro_dados;
    InicializaRegistroJogador(&registro_dados);
    AlocaMemoriaJogador(&registro_dados);

    // solicita os dados do jogador
    scanf("%d", &registro_dados.id);

    char idadeStr[10];
    scanf("%s", idadeStr);
    if (strcmp(idadeStr, "NULO") == 0) {
        registro_dados.idade = -1;  // representando idade nula com -1
    } else {
        registro_dados.idade = atoi(idadeStr);
    }

    scan_quote_string(registro_dados.nomeJogador);
    if (strcmp(registro_dados.nomeJogador, "") == 0) {
        free(registro_dados.nomeJogador);
        registro_dados.nomeJogador = NULL;
    }

    scan_quote_string(registro_dados.nacionalidade);
    if (strcmp(registro_dados.nacionalidade, "") == 0) {
        free(registro_dados.nacionalidade);
        registro_dados.nacionalidade = NULL;
    }

    scan_quote_string(registro_dados.nomeClube);
    if (strcmp(registro_dados.nomeClube, "") == 0) {
        free(registro_dados.nomeClube);
        registro_dados.nomeClube = NULL;
    }

    // adiciona os valores correspondentes para os campos de tamanho variável
    registro_dados.tamNomeJog = (registro_dados.nomeJogador != NULL) ? strlen(registro_dados.nomeJogador) : 0;
    registro_dados.tamNacionalidade = (registro_dados.nacionalidade != NULL) ? strlen(registro_dados.nacionalidade) : 0;
    registro_dados.tamNomeClube = (registro_dados.nomeClube != NULL) ? strlen(registro_dados.nomeClube) : 0;

    // atualiza os campos secundários do registro do jogador
    AtualizaCampos(&registro_dados);

    // Debug: Printar os dados do jogador
    /*printf("Dados do jogador:\n");
    printf("ID: %d\n", registro_dados.id);
    printf("Idade: %d\n", registro_dados.idade);
    printf("Nome do jogador: %s\n", registro_dados.nomeJogador ? registro_dados.nomeJogador : "NULL");
    printf("Nacionalidade: %s\n", registro_dados.nacionalidade ? registro_dados.nacionalidade : "NULL");
    printf("Nome do clube: %s\n", registro_dados.nomeClube ? registro_dados.nomeClube : "NULL");
    printf("Tamanho do registro: %d\n", registro_dados.tamanhoRegistro);
    printf("\n\n");*/

    // ordena registros removidos e armazena em uma lista
    LISTABYTE* removidos = OrdenaRegistrosRemovidos(nomeArquivoBinario);

    // encontra o melhor registro removido para reutilizar
    long int bestFitOffset = BestFitRegister(removidos, registro_dados.tamanhoRegistro);

    if (bestFitOffset != -1) {
        // reutiliza o registro removido
        fseek(nomeArquivoBinario, bestFitOffset, SEEK_SET);

        // Debug: Registro reutilizado
        /*printf("Reutilizando registro removido no byte offset %ld\n", bestFitOffset);

        // printa os dados do registro removido que será sobrescrito
        char removido;
        fread(&removido, sizeof(char), 1, nomeArquivoBinario);
        printf("Removido: %c\n", removido);

        int tamRegistro;
        fread(&tamRegistro, sizeof(int), 1, nomeArquivoBinario);
        printf("Tamanho do registro: %d\n", tamRegistro);

        long int Prox;
        fread(&Prox, sizeof(long int), 1, nomeArquivoBinario);
        printf("Próximo: %ld\n", Prox);

        int ID;
        fread(&ID, sizeof(int), 1, nomeArquivoBinario);
        printf("ID: %d\n", ID);

        int idade;
        fread(&idade, sizeof(int), 1, nomeArquivoBinario);
        printf("Idade: %d\n", idade);

        int tamNomeJog;
        fread(&tamNomeJog, sizeof(int), 1, nomeArquivoBinario);
        char NomeJogador[100];
        fread(NomeJogador, sizeof(char), tamNomeJog, nomeArquivoBinario);
        NomeJogador[tamNomeJog] = '\0';
        printf("Nome do jogador: %s\n", NomeJogador);

        int tamNacionalidade;   
        fread(&tamNacionalidade, sizeof(int), 1, nomeArquivoBinario);
        char NomeNacionalidade[100];
        fread(NomeNacionalidade, sizeof(char), tamNacionalidade, nomeArquivoBinario);
        NomeNacionalidade[tamNacionalidade] = '\0';
        printf("Nacionalidade: %s\n", NomeNacionalidade);

        int tamNomeClube;
        fread(&tamNomeClube, sizeof(int), 1, nomeArquivoBinario);
        char NomeClube[100];    
        fread(NomeClube, sizeof(char), tamNomeClube, nomeArquivoBinario);
        NomeClube[tamNomeClube] = '\0';
        printf("Nome do clube: %s\n", NomeClube);

        printf("\n\n");*/
    } else {
        // adiciona no final do arquivo
        fseek(nomeArquivoBinario, 0, SEEK_END);

        // Debug: Registro adicionado no final
        /*printf("Adicionando registro no final do arquivo\n");*/
    }

    // escrevendo os dados do registro no arquivo BIN
    registro_dados.removido = '0';
    registro_dados.prox = -1;

    long int posicaoRegistro = ftell(nomeArquivoBinario); // salva a posição do novo registro

    fwrite(&registro_dados.removido, sizeof(char), 1, nomeArquivoBinario);
    fwrite(&registro_dados.tamanhoRegistro, sizeof(int), 1, nomeArquivoBinario);
    fwrite(&registro_dados.prox, sizeof(long int), 1, nomeArquivoBinario);
    fwrite(&registro_dados.id, sizeof(int), 1, nomeArquivoBinario);
    fwrite(&registro_dados.idade, sizeof(int), 1, nomeArquivoBinario);
    

    // escreve o nome do jogador ou valor nulo
    if (registro_dados.tamNomeJog > 0) {
        fwrite(&registro_dados.tamNomeJog, sizeof(int), 1, nomeArquivoBinario);
        fwrite(registro_dados.nomeJogador, sizeof(char), registro_dados.tamNomeJog, nomeArquivoBinario);
    } else {
        int tamNulo = 0;
        fwrite(&tamNulo, sizeof(int), 1, nomeArquivoBinario);
    }

    // escreve a nacionalidade do jogador ou valor nulo
    if (registro_dados.tamNacionalidade > 0) {
        fwrite(&registro_dados.tamNacionalidade, sizeof(int), 1, nomeArquivoBinario);
        fwrite(registro_dados.nacionalidade, sizeof(char), registro_dados.tamNacionalidade, nomeArquivoBinario);
    } else {
        int tamNulo = 0;
        fwrite(&tamNulo, sizeof(int), 1, nomeArquivoBinario);
    }

    // escreve o nome do clube do jogador ou valor nulo
    if (registro_dados.tamNomeClube > 0) {
        fwrite(&registro_dados.tamNomeClube, sizeof(int), 1, nomeArquivoBinario);
        fwrite(registro_dados.nomeClube, sizeof(char), registro_dados.tamNomeClube, nomeArquivoBinario);
    } else {
        int tamNulo = 0;
        fwrite(&tamNulo, sizeof(int), 1, nomeArquivoBinario);
    }

    // preenche o lixo com '$' o restante do registro (se tiver espaço sobrando)
    int totalBytes = sizeof(registro_dados.removido) + sizeof(registro_dados.tamanhoRegistro) +
                     sizeof(registro_dados.id) + sizeof(registro_dados.idade) +
                     sizeof(registro_dados.tamNomeJog) + registro_dados.tamNomeJog +
                     sizeof(registro_dados.tamNacionalidade) +
                     registro_dados.tamNacionalidade + sizeof(registro_dados.tamNomeClube) +
                     registro_dados.tamNomeClube + sizeof(registro_dados.prox);

    int bytesRestantes = registro_dados.tamanhoRegistro - totalBytes;

    for (int i = 0; i < bytesRestantes; i++) {
        char lixo = '$';
        fwrite(&lixo, sizeof(char), 1, nomeArquivoBinario);
    }

    // Debug: Registro inserido com sucesso
    /*printf("Registro inserido com sucesso!\n");*/

    // lê o registro recém-escrito para verificar consistência
    fseek(nomeArquivoBinario, posicaoRegistro, SEEK_SET);

    /*fread(&registro_dados.removido, sizeof(char), 1, nomeArquivoBinario);
    printf("Removido: %c\n", registro_dados.removido);

    fread(&registro_dados.tamanhoRegistro, sizeof(int), 1, nomeArquivoBinario);
    printf("Tamanho do registro: %d\n", registro_dados.tamanhoRegistro);

    fread(&registro_dados.prox, sizeof(long int), 1, nomeArquivoBinario);
    printf("Próximo: %ld\n", registro_dados.prox);

    fread(&registro_dados.id, sizeof(int), 1, nomeArquivoBinario);
    printf("ID: %d\n", registro_dados.id);

    fread(&registro_dados.idade, sizeof(int), 1, nomeArquivoBinario);
    printf("Idade: %d\n", registro_dados.idade);

    fread(&registro_dados.tamNomeJog, sizeof(int), 1, nomeArquivoBinario);
    printf("Tamanho do nome do jogador: %d\n", registro_dados.tamNomeJog);

    char NomeJogador[100];
    fread(NomeJogador, sizeof(char), registro_dados.tamNomeJog, nomeArquivoBinario);
    NomeJogador[registro_dados.tamNomeJog] = '\0';
    printf("Nome do jogador: %s\n", NomeJogador);

    fread(&registro_dados.tamNacionalidade, sizeof(int), 1, nomeArquivoBinario);
    printf("Tamanho da nacionalidade: %d\n", registro_dados.tamNacionalidade);

    char NomeNacionalidade[100];
    fread(NomeNacionalidade, sizeof(char), registro_dados.tamNacionalidade, nomeArquivoBinario);
    NomeNacionalidade[registro_dados.tamNacionalidade] = '\0';
    printf("Nacionalidade: %s\n", NomeNacionalidade);

    fread(&registro_dados.tamNomeClube, sizeof(int), 1, nomeArquivoBinario);
    printf("Tamanho do nome do clube: %d\n", registro_dados.tamNomeClube);

    char NomeClube[100];    
    fread(NomeClube, sizeof(char), registro_dados.tamNomeClube, nomeArquivoBinario);
    NomeClube[registro_dados.tamNomeClube] = '\0';
    printf("Nome do clube: %s\n", NomeClube);

    printf("\n\n");*/

    // atualiza a lista de removidos no arquivo
    ReescreveRegistrosRemovidosBIN(nomeArquivoBinario, removidos);

    DesalocaMemoriaJogador(&registro_dados);
    LiberaLista(removidos);

    return true;
}