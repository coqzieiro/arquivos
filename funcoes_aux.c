/*
INTEGRANTES DO GRUPO:
                    Felipe da Costa Coqueiro,   NºUSP: 11781361
                    Fernando Alee Suaiden,      NºUSP: 12680836
*/

#include "funcoes_aux.h"
#include "funcoes.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Função para verificar se um registro corresponde a todos os campos de busca
int todosCamposCorrespondem(DADOS registro, CAMPO_BUSCA camposBusca[], int numCamposBusca) {
    for (int i = 0; i < numCamposBusca; i++) {
        if (strcmp(camposBusca[i].nomeCampo, "id") == 0) { // Campo id
            if (camposBusca[i].valorInt != registro.id) {
                return(0);
            }
        } else if (strcmp(camposBusca[i].nomeCampo, "idade") == 0) { // Campo idade
            if (camposBusca[i].valorInt != registro.idade) {
                return(0);
            }
        } else if (strcmp(camposBusca[i].nomeCampo, "nome") == 0) { // Campo nome
            if (strcmp(camposBusca[i].valorString, registro.nomeJogador) != 0) {
                return(0);
            }
        } else if (strcmp(camposBusca[i].nomeCampo, "nacionalidade") == 0) { // Campo nacionalidade
            if (strcmp(camposBusca[i].valorString, registro.nacionalidade) != 0) {
                return(0);
            }
        } else if (strcmp(camposBusca[i].nomeCampo, "nomeClube") == 0) { // Campo nomeClube
            if (strcmp(camposBusca[i].valorString, registro.nomeClube) != 0) {
                return(0);
            }
        } else {
            return(0);
        }
    }
    return(1);
}

// Função para validar o nome do campo
int validarNomeCampo(const char *nomeCampo) {
    if (strlen(nomeCampo) > MAX_CAMPO) { // Tamanho do campo não pode ser maior MAX_CAMPO
        return(0);
    }
    for (int i = 0; nomeCampo[i] != '\0'; i++) {
        if (!isalnum(nomeCampo[i]) && nomeCampo[i] != '_') {
            return(0);
        }
    }
    return(1);
}