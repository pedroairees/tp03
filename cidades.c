#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "cidades.h"


int compararCidades(const void *a, const void *b) {
    return ((Cidade *)a)->Posicao - ((Cidade *)b)->Posicao;
}

Estrada *getEstrada(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro: não foi possível abrir o arquivo %s\n", nomeArquivo);
        return NULL;
    }

    Estrada *estrada = (Estrada *)malloc(sizeof(Estrada));
    if (estrada == NULL) {
        printf("Erro: não foi possível alocar memória para estrada\n");
        fclose(arquivo);
        return NULL;
    }

    if (fscanf(arquivo, "%d", &(estrada->T)) != 1) {
        printf("Erro: não foi possível ler o comprimento da estrada\n");
        free(estrada);
        fclose(arquivo);
        return NULL;
    }

    if (fscanf(arquivo, "%d", &(estrada->N)) != 1) {
        printf("Erro: não foi possível ler o número de cidades\n");
        free(estrada);
        fclose(arquivo);
        return NULL;
    }

    if (estrada->T < 3 || estrada->T > 1000000 || estrada->N < 2 || estrada->N > 10000) {
        printf("Erro: valores de T ou N fora dos limites permitidos\n");
        free(estrada);
        fclose(arquivo);
        return NULL;
    }

    estrada->C = (Cidade *)malloc(estrada->N * sizeof(Cidade));
    if (estrada->C == NULL) {
        printf("Erro: não foi possível alocar memória para cidades\n");
        free(estrada);
        fclose(arquivo);
        return NULL;
    }

    // Pular a nova linha após ler N
    fgetc(arquivo);

    for (int i = 0; i < estrada->N; i++) {
        char linha[512];
        if (fgets(linha, sizeof(linha), arquivo) != NULL) {
            if (sscanf(linha, "%d %[^\n]", &(estrada->C[i].Posicao), estrada->C[i].Nome) != 2) {
                printf("Erro: formato inválido na linha %d\n", i + 1);
                free(estrada->C);
                free(estrada);
                fclose(arquivo);
                return NULL;
            }
        } else {
            printf("Erro: não foi possível ler a linha %d\n", i + 1);
            free(estrada->C);
            free(estrada);
            fclose(arquivo);
            return NULL;
        }
    }

    fclose(arquivo);
    return estrada;
}

double calcularMenorVizinhanca(const char *nomeArquivo) {
    Estrada *estrada = getEstrada(nomeArquivo);
    if (!estrada)
        return -1.0;


    qsort(estrada->C, estrada->N, sizeof(Cidade), compararCidades);

    double menorVizinhanca = estrada->T;

    for (int i = 0; i < estrada->N; i++) {
        double inicio = (i == 0) ? 0 : (estrada->C[i].Posicao + estrada->C[i-1].Posicao) / 2.0;
        double fim = (i == estrada->N - 1) ? estrada->T : (estrada->C[i].Posicao + estrada->C[i+1].Posicao) / 2.0;
        double vizinhanca = fim - inicio;

        if (vizinhanca < menorVizinhanca)
            menorVizinhanca = vizinhanca;
    }

    free(estrada->C);
    free(estrada);

    return menorVizinhanca;
}

char *cidadeMenorVizinhanca(const char *nomeArquivo) {
    Estrada *estrada = getEstrada(nomeArquivo);
    if (estrada == NULL) {
        printf("Erro: não foi possível obter os dados da estrada\n");
        return NULL;
    }

    double menorVizinhanca = (double) estrada->T;
    int cidadeIndex = -1;
    for (int i = 0; i < estrada->N - 1; i++) {
        double vizinhanca = (estrada->C[i+1].Posicao - estrada->C[i].Posicao) / 2.0;
        if (vizinhanca < menorVizinhanca) {
            menorVizinhanca = vizinhanca;
            cidadeIndex = i;
        }
    }

    // Verificar as extremidades
    if (estrada->C[0].Posicao < menorVizinhanca) {
        menorVizinhanca = estrada->C[0].Posicao;
        cidadeIndex = 0;
    }
    if (estrada->T - estrada->C[estrada->N - 1].Posicao < menorVizinhanca) {
        menorVizinhanca = estrada->T - estrada->C[estrada->N - 1].Posicao;
        cidadeIndex = estrada->N - 1;
    }

    char *nomeCidade = (char *)malloc(256 * sizeof(char));
    if (nomeCidade == NULL) {
        printf("Erro: não foi possível alocar memória para nome da cidade\n");
        free(estrada->C);
        free(estrada);
        return NULL;
    }
    strcpy(nomeCidade, estrada->C[cidadeIndex].Nome);

    free(estrada->C);
    free(estrada);
    return nomeCidade;
}

int main() {
    setlocale(LC_ALL, "");

    char nomeArquivo[256];
    int opcao;

    printf("Digite o nome do arquivo de entrada: ");
    scanf("%s", nomeArquivo);

    printf("Escolha uma opção:\n");
    printf("1. Calcular menor vizinhança\n");
    printf("2. Encontrar cidade com a menor vizinhança\n");
    scanf("%d", &opcao);

    switch(opcao) {
        case 1: {
            double menorVizinhanca = calcularMenorVizinhanca(nomeArquivo);
            if (menorVizinhanca < 0) {
                printf("Erro ao processar o arquivo.\n");
            } else {
                printf("Menor vizinhança: %.2f\n", menorVizinhanca);
            }
            break;
        }
        case 2: {
            char *cidade = cidadeMenorVizinhanca(nomeArquivo);
            if (cidade == NULL) {
                printf("Erro ao processar o arquivo.\n");
            } else {
                printf("Cidade com a menor vizinhança: %s\n", cidade);
                free(cidade);  // Lembre-se de liberar a memória alocada
            }
            break;
        }
        default:
            printf("Opção inválida.\n");
    }

    return 0;
}
