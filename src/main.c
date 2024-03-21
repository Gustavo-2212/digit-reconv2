#include "data.h"
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define CICLOS_MAX              (10000)
#define TAXA_DE_APRENDIZAGEM    (0.001)
#define ERRO_MINIMO             (0.002)

struct erro_s {
    double *Erros;
    int ciclos;
};

struct erro_s treina_MPL(void);
void testa_MPL(void);
void salvar_pesos(const char nome_arq[20], double w[NEURONIOS_CAMADA_SAIDA][NEURONIOS_CAMADA_INTERNA], double b[NEURONIOS_CAMADA_SAIDA],
                    double w_int[NEURONIOS_CAMADA_INTERNA][TAMANHO_IMG], double b_int[NEURONIOS_CAMADA_INTERNA]);
void salvar_erros(const char nome_arq[], double erros[], int ciclos);
double calcula_erro(double Y[], int saida);
double calcula_erro_inicial(void);
double *calcula_saida(uint8_t entrada[]);

int main(void) {
    gera_entradas();
    gera_saidas();

    struct erro_s rs = treina_MPL();
    
    testa_MPL();
    for(int i = 0; i < NUM_DIGITOS; i++) {
        printf("\n\n[+] Para a entrada teste [%d], obtemos:\n", i);
        for(int j = 0; j < NEURONIOS_CAMADA_SAIDA; j++) {
            printf("\tDígito [%d]: %f\n", j, saida_obtida[i][j]);
        }
    }

    salvar_erros("./tmp/erros.txt", rs.Erros, rs.ciclos);

    return EXIT_SUCCESS;
}


struct erro_s treina_MPL(void) {
    srand(time(NULL));
    int ciclos = 0;
  
    double *Erros = (double*)calloc(CICLOS_MAX, sizeof(double));

    /* Inicialização dos pesos dos neurônios da camada interna */
    for(int i = 0; i < NEURONIOS_CAMADA_INTERNA; i++) {
        for(int j = 0; j < TAMANHO_IMG; j++) {
            w_int[i][j] = (double)(rand() % 1000) / 1000 - 0.5;
        }
        b_int[i]  = (double)(rand() % 1000) / 1000 - 0.5;
    }
    /* delta_int = [0, 0, 0, ..., 0], length=NEURONIOS_CAMADA_INTERNA */
    double *delta_int = (double*)calloc(NEURONIOS_CAMADA_INTERNA, sizeof(double));


    /* Inicialização dos pesos dos neurônios da camada de saída */
    for(int i = 0; i < NEURONIOS_CAMADA_SAIDA; i++) {
        for(int j = 0; j < NEURONIOS_CAMADA_INTERNA; j++) {
            w[i][j] = (double)(rand() % 1000) / 1000 - 0.5;
        }
        b[i] = (double)(rand() % 1000) / 1000 - 0.5;
    }

    salvar_pesos("./tmp/pesos_iniciais.txt" ,w, b, w_int, b_int);

    double erro = calcula_erro_inicial();
    Erros[0] = erro;
    while (ciclos < CICLOS_MAX) {
        double E = 0;
        ciclos++;

        for(int digito = 0; digito < QTD_ENTRADAS; digito++) {

            uint8_t *entrada = entradas[digito];
            int8_t *saida = saidas[digito / NUM_CLASSES];
            

            double Z[NEURONIOS_CAMADA_INTERNA];
            double Y[NEURONIOS_CAMADA_SAIDA];
            double soma;

            /* Fase de Feedfoward */
            /* Calcula a saída dos neurônios da camada interna */
            for(int neuronio = 0; neuronio < NEURONIOS_CAMADA_INTERNA; neuronio++) {
                soma = b_int[neuronio];
                for(int byte = 0; byte < TAMANHO_IMG; byte++) {
                    soma += entrada[byte] * w_int[neuronio][byte];
                }
                Z[neuronio] = SIGMOIDE(soma);
            }

            /* Calcula a saída dos neurônios da camada de saída */
            for(int neuronio = 0; neuronio < NEURONIOS_CAMADA_SAIDA; neuronio++) {
                soma = b[neuronio];
                for(int saida_internos = 0; saida_internos < NEURONIOS_CAMADA_INTERNA; saida_internos++) {
                    soma += Z[saida_internos] * w[neuronio][saida_internos];
                }
                Y[neuronio] = SIGMOIDE(soma);
            }
            /*
                Aqui foi gerado um vetor com as saídas dos 10 neurônios de saída.
                Y[neurônio], 0 <= neurônio < 10
            */

           /* Cálculo da média do erro quadrático */
            E = calcula_erro(Y, (digito/NUM_CLASSES));   

            /* Fase de Retropropagação */
            
            /* Cálculo da atualização dos pesos da camada dos neurônios de saída */
            double delta_w[NEURONIOS_CAMADA_SAIDA];
            for(int neuronio = 0; neuronio < NEURONIOS_CAMADA_SAIDA; neuronio++) {
                delta_w[neuronio] = (saida[neuronio] - Y[neuronio]) * 0.5 * (1 + Y[neuronio]) * (1 - Y[neuronio]);
            }

            double Delta_w[NEURONIOS_CAMADA_SAIDA][NEURONIOS_CAMADA_INTERNA];
            double Delta_b[NEURONIOS_CAMADA_SAIDA];
            for(int neuronio_out = 0; neuronio_out < NEURONIOS_CAMADA_SAIDA; neuronio_out++) {
                for(int neuronio_int = 0; neuronio_int < NEURONIOS_CAMADA_INTERNA; neuronio_int++) {
                    Delta_w[neuronio_out][neuronio_int] = TAXA_DE_APRENDIZAGEM * delta_w[neuronio_out] * Z[neuronio_int];
                }
            }

            for(int neuronio_out = 0; neuronio_out < NEURONIOS_CAMADA_SAIDA; neuronio_out++) {
                Delta_b[neuronio_out] = TAXA_DE_APRENDIZAGEM * delta_w[neuronio_out];
            }

            /* Cálculo para a atualização dos pesos dos neurônios da camada interna */
            for(int neuronio_int = 0; neuronio_int < NEURONIOS_CAMADA_INTERNA; neuronio_int++) {
                double soma_delta_w = 0;
                double soma_w = 0;
                for(int neuronio_out = 0; neuronio_out < NEURONIOS_CAMADA_SAIDA; neuronio_out++) {
                    soma_delta_w += delta_w[neuronio_out];
                    soma_w += w[neuronio_out][neuronio_int];
                }
                delta_int[neuronio_int] = soma_delta_w * soma_w * 0.5 * (1 + Z[neuronio_int]) * (1 - Z[neuronio_int]);
            }

            double Delta_w_int[NEURONIOS_CAMADA_INTERNA][TAMANHO_IMG];
            double Delta_b_int[NEURONIOS_CAMADA_INTERNA];
            for(int neuronio_int = 0; neuronio_int < NEURONIOS_CAMADA_INTERNA; neuronio_int++) {
                for(int amostra = 0; amostra < TAMANHO_IMG; amostra++) {
                    Delta_w_int[neuronio_int][amostra] = TAXA_DE_APRENDIZAGEM * delta_int[neuronio_int] * entrada[amostra];
                }
            }

            for(int neuronio_int = 0; neuronio_int < NEURONIOS_CAMADA_INTERNA; neuronio_int++) {
                Delta_b_int[neuronio_int] = TAXA_DE_APRENDIZAGEM * delta_int[neuronio_int];
            }

            /* Atualização dos pesos da camada de saída */
            for(int neuronio_out = 0; neuronio_out < NEURONIOS_CAMADA_SAIDA; neuronio_out++) {
                for(int neuronio_int = 0; neuronio_int < NEURONIOS_CAMADA_INTERNA; neuronio_int++) {
                    w[neuronio_out][neuronio_int] += Delta_w[neuronio_out][neuronio_int];
                }
            }

            for(int neuronio_out = 0; neuronio_out < NEURONIOS_CAMADA_SAIDA; neuronio_out++) {
                b[neuronio_out] += Delta_b[neuronio_out];
            }

            /* Atualização dos pesos da camada interna */
            for(int neuronio_int = 0; neuronio_int < NEURONIOS_CAMADA_INTERNA; neuronio_int++) {
                for(int amostra = 0; amostra < TAMANHO_IMG; amostra++) {
                    w_int[neuronio_int][amostra] += Delta_w_int[neuronio_int][amostra];
                }
            }

            for(int neuronio_int = 0; neuronio_int < NEURONIOS_CAMADA_INTERNA; neuronio_int++) {
                b_int[neuronio_int] += Delta_b_int[neuronio_int];
            }   
        }

        erro = E / QTD_ENTRADAS;
        Erros[ciclos] = erro;
    }

    printf("Fim do treinamento!\n");
    printf("Ciclos: %d\n", ciclos);
    salvar_pesos("./tmp/pesos_finais.txt", w, b, w_int, b_int);

    /* Salvando os resultados obtidos em uma estrutura para retorno da função */
    struct erro_s rs = {
        .Erros = Erros,
        .ciclos = ciclos,
    };

    return rs;
}

double *calcula_saida(uint8_t entrada[]) {
    double Z[NEURONIOS_CAMADA_INTERNA];
    double *Y = (double*) malloc(sizeof(double) * NEURONIOS_CAMADA_SAIDA);
    double soma;

    for(int neuronio = 0; neuronio < NEURONIOS_CAMADA_INTERNA; neuronio++) {
        soma = b_int[neuronio];
        for(int byte = 0; byte < TAMANHO_IMG; byte++) {
            soma += entrada[byte] * w_int[neuronio][byte];
        }
        Z[neuronio] = SIGMOIDE(soma);
    }

    /* Calcula a saída dos neurônios da camada de saída */
    for(int neuronio = 0; neuronio < NEURONIOS_CAMADA_SAIDA; neuronio++) {
        soma = b[neuronio];
        for(int saida_internos = 0; saida_internos < NEURONIOS_CAMADA_INTERNA; saida_internos++) {
            soma += Z[saida_internos] * w[neuronio][saida_internos];
        }
        Y[neuronio] = SIGMOIDE(soma);
    }

    return Y;
}

double calcula_erro(double Y[], int saida) {
    double erro;
    for(int i = 0; i < NEURONIOS_CAMADA_SAIDA; i++) {
        erro += pow(saidas[saida][i] - Y[i], 2);
    }

    return sqrt(erro / NEURONIOS_CAMADA_SAIDA);
}

double calcula_erro_inicial() {
    double erro;
    for(int i = 0; i < QTD_ENTRADAS; i++) {
        double *Y = calcula_saida(entradas[i]);
        erro += calcula_erro(Y, (i/NUM_CLASSES));
    }

    return erro / QTD_ENTRADAS;
}

void salvar_pesos(const char nome_arq[20], double w[NEURONIOS_CAMADA_SAIDA][NEURONIOS_CAMADA_INTERNA], double b[NEURONIOS_CAMADA_SAIDA],
                    double w_int[NEURONIOS_CAMADA_INTERNA][TAMANHO_IMG], double b_int[NEURONIOS_CAMADA_INTERNA]) {
    FILE *arquivo = fopen(nome_arq, "a"); // Abre o arquivo para adicionar conteúdo (append)
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo pesos.txt.\n");
        return;
    }

    // Escreve a matriz1 no arquivo
    fprintf(arquivo, "W:\n");
    for (int i = 0; i < NEURONIOS_CAMADA_SAIDA; i++) {
        for (int j = 0; j < NEURONIOS_CAMADA_INTERNA; j++) {
            fprintf(arquivo, "%f ", w[i][j]);
        }
        fprintf(arquivo, "\n");
    }

    // Escreve o vetor1 no arquivo
    fprintf(arquivo, "\nB:\n");
    for (int i = 0; i < NEURONIOS_CAMADA_SAIDA; i++) {
        fprintf(arquivo, "%f ", b[i]);
    }
    fprintf(arquivo, "\n");

    // Escreve a matriz2 no arquivo
    fprintf(arquivo, "\nW_int:\n");
    for (int i = 0; i < NEURONIOS_CAMADA_INTERNA; i++) {
        for (int j = 0; j < TAMANHO_IMG; j++) {
            fprintf(arquivo, "%f ", w_int[i][j]);
        }
        fprintf(arquivo, "\n");
    }

    // Escreve o vetor2 no arquivo
    fprintf(arquivo, "\nB_int:\n");
    for (int i = 0; i < NEURONIOS_CAMADA_INTERNA; i++) {
        fprintf(arquivo, "%f ", b_int[i]);
    }
    fprintf(arquivo, "\n\n---------------------------------------------------------------------------------------------\n");

    fclose(arquivo); // Fecha o arquivo
}

void testa_MPL(void) {
    for(int digito = 0; digito < NUM_DIGITOS; digito++) {
        uint8_t *entrada_teste = entradas_teste[digito];
        double *Y = calcula_saida(entrada_teste);

        /* Calcula a saída dos neurônios da camada de saída */
        for(int neuronio = 0; neuronio < NEURONIOS_CAMADA_SAIDA; neuronio++) {
            saida_obtida[digito][neuronio] = Y[neuronio];
        }
    }
}

void salvar_erros(const char nome_arq[], double erros[], int ciclos) {
    FILE *arquivo = fopen(nome_arq, "w");
    if(arquivo == NULL) {
        printf("Falha ao abrir o arquivo %s.\n", nome_arq);
        return;
    }

    fprintf(arquivo, "\t[+] Erros:\n");
    for(int i = 0; i < ciclos; i++) {
        fprintf(arquivo, "%f ", erros[i]);
    }

    fclose(arquivo);
}