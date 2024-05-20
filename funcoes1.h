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
        int64_t proxByteOffset;
    } REGISTRO_INDEX;

    void createIndex();
    void deleteFrom();
    void insertInto();

#endif