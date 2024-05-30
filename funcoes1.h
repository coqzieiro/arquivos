#ifndef FUNCOES1_H

    #define FUNCOES1_H
    #include <stdint.h>
    
    // Registro de cabeçalho
    typedef struct {
        char status;
    } CABECALHO_INDEX;

    // Registro de dados
    typedef struct {
        int id;
        int64_t byteOffset;
    } REGISTRO_INDEX;

    void criarIndex();
    void deletar();
    void inserir();

#endif