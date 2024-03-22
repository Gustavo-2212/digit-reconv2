#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data.h"

/* ======================================================================= */
/*               Definição de valores constantes                           */
// const char *imagens[] = {
//     "0_1.bmp\0", "0_2.bmp\0", "0_3.bmp\0", "0_4.bmp\0", "0_5.bmp\0",
//     "1_1.bmp\0", "1_2.bmp\0", "1_3.bmp\0", "1_4.bmp\0", "1_5.bmp\0",
//     "2_1.bmp\0", "2_2.bmp\0", "2_3.bmp\0", "2_4.bmp\0", "2_5.bmp\0",
//     "3_1.bmp\0", "3_2.bmp\0", "3_3.bmp\0", "3_4.bmp\0", "3_5.bmp\0",
//     "4_1.bmp\0", "4_2.bmp\0", "4_3.bmp\0", "4_4.bmp\0", "4_5.bmp\0",
//     "5_1.bmp\0", "5_2.bmp\0", "5_3.bmp\0", "5_4.bmp\0", "5_5.bmp\0",
//     "6_1.bmp\0", "6_2.bmp\0", "6_3.bmp\0", "6_4.bmp\0", "6_5.bmp\0",
//     "7_1.bmp\0", "7_2.bmp\0", "7_3.bmp\0", "7_4.bmp\0", "7_5.bmp\0",
//     "8_1.bmp\0", "8_2.bmp\0", "8_3.bmp\0", "8_4.bmp\0", "8_5.bmp\0",
//     "9_1.bmp\0", "9_2.bmp\0", "9_3.bmp\0", "9_4.bmp\0", "9_5.bmp\0"
// };

const char *imagens_teste[] = {
    "0_teste.bmp\0", "1_teste.bmp\0", "2_teste.bmp\0", "3_teste.bmp\0", "4_teste.bmp\0",
    "5_teste.bmp\0", "6_teste.bmp\0", "7_teste.bmp\0", "8_teste.bmp\0", "9_teste.bmp\0"
};

const char *arquivos[] = {
    "data0.bin\0", "data1.bin\0", "data2.bin\0", "data3.bin\0",
    "data4.bin\0", "data5.bin\0", "data6.bin\0", "data7.bin\0",
    "data8.bin\0", "data9.bin\0"
};


/* ======================================================================= */
/*               Protótipo das funções                                     */
void criaBMP(const char *nome_arq, uint8_t conteudo[TAMANHO_IMG]);
void adiciona_valores(const char *nome_arq, int linha, int digito);


/* ======================================================================= */
/*     Função para construir a variável de entradas e gerar as imagens     */
int gera_entradas(void) {
    /* Constrói os dados de treinamento */
    int linha = 0;
    for(int i = 0; i < NUM_DIGITOS; i++) {
        char nome_arq[20] = "data_bin/";
        strcat(nome_arq, arquivos[i]);

        adiciona_valores(nome_arq, linha, i);
        linha += NUM_CLASSES;
    }
    
    for(int i = 0; i < QTD_ENTRADAS; i++) {
        char nome_arq[20] = "data/";
        
        //strcat(nome_arq, imagens[i]);
        
        char digito[20];
        sprintf(digito, "%d", (i/NUM_CLASSES));
        char amostra[20];
        sprintf(amostra, "%d", (i%NUM_CLASSES)+1);


        strcat(amostra, ".bmp");
        strcat(digito, "_");
        strcat(digito, amostra);
        strcat(nome_arq, digito);

        criaBMP(nome_arq, entradas[i]);
    }

    for(int i = 0; i < NUM_DIGITOS; i++) {
        char nome_arq[20] = "data/";
        strcat(nome_arq, imagens_teste[i]);

        criaBMP(nome_arq, entradas_teste[i]);
    }

    return EXIT_SUCCESS;
}

/* ======================================================================= */
/*     Função para construir a variável de saídas                          */
int gera_saidas(void) {
    for(int i = 0; i < NUM_DIGITOS; i++) {
        for(int j = 0; j < NUM_DIGITOS; j++) {
            if(i == j) saidas[i][j] = 1;
            else       saidas[i][j] = -1;
        }
    }

    return EXIT_SUCCESS;
}


/* ======================================================================= */
/*               Implementação das Funções prototipadas                    */

/*
        Cria a imagem do tipo bit map (.bmp) com base no nome do arquivo e na sequência de bytes de "conteudo"
*/
void criaBMP(const char *nome_arq, uint8_t conteudo[TAMANHO_IMG]) {
    FILE *bmpFile = fopen(nome_arq, "wb");
    if (!bmpFile) {
        printf("Erro ao abrir o arquivo %s para escrita.\n", nome_arq);
        return;
    }

    // Cabeçalho do arquivo BMP
    uint8_t bmpCabecalho[54] = {
        0x42, 0x4D,             // "BM" - Assinatura do BMP
        0x36, 0x00, 0x0C, 0x00, // Tamanho do arquivo em bytes (largura x altura x 3 + cabeçalho)
        0x00, 0x00,             // Reservado
        0x00, 0x00,             // Reservado
        0x36, 0x00, 0x00, 0x00, // Offset para os dados da imagem
        0x28, 0x00, 0x00, 0x00, // Tamanho do cabeçalho do BMP
        0x1C, 0x00, 0x00, 0x00, // Largura em pixels (28)
        0x1C, 0x00, 0x00, 0x00, // Altura em pixels (28)
        0x01, 0x00,             // Planes (1)
        0x18, 0x00,             // Bits por pixel (24 bits, 3 bytes)
        0x00, 0x00, 0x00, 0x00, // Compression (0 = sem compressão)
        0x00, 0x00, 0x0C, 0x00, // Tamanho da imagem em bytes (largura x altura x 3)
        0x00, 0x00, 0x00, 0x00, // Resolução horizontal em pixels por metro
        0x00, 0x00, 0x00, 0x00, // Resolução vertical em pixels por metro
        0x00, 0x00, 0x00, 0x00, // Número de cores na paleta (usando paleta padrão)
        0x00, 0x00, 0x00, 0x00  // Número de cores importantes (0 = todas são importantes)
    };

    // Escrever o cabeçalho do BMP
    fwrite(bmpCabecalho, sizeof(uint8_t), 54, bmpFile);

    //Escrever os dados da imagem
    for (int i = LINHAS - 1; i >= 0; i--) {
        for (int j = 0; j < COLUNAS; j++) {
            uint8_t pixel[3] = {conteudo[i * LINHAS + j], conteudo[i * LINHAS + j], conteudo[i * LINHAS + j]}; // RGB (mesma cor em cada canal)
            fwrite(pixel, sizeof(uint8_t), 3, bmpFile);
        }
        // Preencher linha com bytes extras para garantir que seja múltiplo de 4
        uint8_t padding = 0;
        fwrite(&padding, sizeof(uint8_t), (4 - (COLUNAS * 3) % 4) % 4, bmpFile);
    }

    fclose(bmpFile);
}

/* 
        (1) Adiciona a sequencia de bytes na variável entradas, ou seja, os primeiros 784 bytes na primeira linha de entradas.
            Isso se repete cinco vezes, quantidade de classes de um mesmo número adotado.
            Depois, as próximas 5 entradas recebe dados de outro arquivo, no caso seria o arquivo contendo as classes do próximo número, pegando mais 5 * 784 bytes e armazenando em 5 linhas da matriz entradas.
        (2) Adiciona a sequencia de bytes na variável entradas para teste, ou seja, os primeiros 784 bytes na primeira linha de entradas para teste.
        Isso se repete uma vez, serão testados a 6ª amostra de cada arquivo contendo os dígitos.
 */
void adiciona_valores(const char *nome_arq, int linha, int digito) {
    FILE *arq;
    arq = fopen(nome_arq, "rb");

    if(!arq) {
        printf("Erro ao tentar abrir o arquivo %s para leitura.\n", nome_arq);
        return;
    }

    for(int i = linha; i < linha + NUM_CLASSES; i++) {
        for(int j = 0; j < TAMANHO_IMG; j++) {
            fread(&entradas[i][j], sizeof(uint8_t), 1, arq);
        }
    }

    for(int j = 0; j < TAMANHO_IMG; j++) {
        fread(&entradas_teste[digito][j], sizeof(uint8_t), 1, arq);
    }

    fclose(arq);
}
