#include <stdint.h>

#define LINHAS (28)
#define COLUNAS (28)
#define NUM_DIGITOS (10)
#define NUM_CLASSES (50)
#define TAMANHO_IMG (LINHAS * COLUNAS)
#define QTD_ENTRADAS (NUM_DIGITOS * NUM_CLASSES)
#define NEURONIOS_CAMADA_INTERNA      (15)
#define NEURONIOS_CAMADA_SAIDA        (10)
#define X_MAX (255.0)
#define X_MIN (0)
#define Y_MAX (1.0)
#define Y_MIN (-1.0)

uint8_t entradas[QTD_ENTRADAS][TAMANHO_IMG];
double entradas_normalizadas[QTD_ENTRADAS][TAMANHO_IMG];
int8_t saidas[NUM_DIGITOS][NUM_DIGITOS];

uint8_t entradas_teste[NUM_DIGITOS][TAMANHO_IMG];
double entradas_teste_normalizada[NUM_DIGITOS][TAMANHO_IMG];

/* Inicialização dos pesos dos neurônios da camada de saída */
double w[NEURONIOS_CAMADA_SAIDA][NEURONIOS_CAMADA_INTERNA];
double b[NEURONIOS_CAMADA_SAIDA];

/* Inicialização dos pesos dos neurônios da camada interna */
double w_int[NEURONIOS_CAMADA_INTERNA][TAMANHO_IMG];
double b_int[NEURONIOS_CAMADA_INTERNA];

/* Saída obtida do teste da rede neural treinada */
double saida_obtida[NUM_DIGITOS][NEURONIOS_CAMADA_SAIDA];

int gera_entradas(void);
int gera_saidas(void);

#define SIGMOIDE(x) ((2.0 / (1 + exp(-x))) - 1)
#define NORMALIZA(x) ( ((x - X_MIN) / (X_MAX - X_MIN)) * (Y_MAX - Y_MIN) + Y_MIN )