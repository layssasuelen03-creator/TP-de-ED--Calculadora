
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "calculadora.h"

void removeSpaces(char *s) {
    int j=0;
    for (int i=0; s[i]; i++)
        if (!isspace((unsigned char)s[i]))
            s[j++] = s[i];
    s[j] = '\0';
}

// Remove parênteses externos 
void removeOuter(char *s) {
    int len = strlen(s);
    while (s[0]=='(' && s[len-1]==')') {
        int depth = 0;
        int ok = 1;
        for (int i=0; i<len-1; i++) {
            if (s[i]=='(') depth++;
            else if (s[i]==')') depth--;
            if (depth==0 && i<len-2) {
                ok = 0;
                break;
            }
        }
        if (!ok) break;
        memmove(s, s+1, len-2);
        s[len-2] = '\0';
        len -= 2;
    }
}

void normalizar(char *dest, char *src) {
    strcpy(dest, src);
    removeSpaces(dest);
    removeOuter(dest);
}

// Funções dos testes 
void testarExpressao(int numero, char *posFixa, char *infixaEsperada, float valorEsperado) {

    printf("=====================================\n");
    printf("TESTE %d:\n", numero);
    printf("Posfixa: %s\n", posFixa);

    char copia[512];
    strcpy(copia, posFixa);
    char *obtida = getFormaInFixa(copia);

    printf("Infixa obtida: %s\n", obtida ? obtida : "ERRO");
    printf("Infixa esperada: %s\n", infixaEsperada);

    char normObtida[512], normEsperada[512];
    if (obtida) {
        normalizar(normObtida, obtida);
        normalizar(normEsperada, infixaEsperada);
    }

    // Mostrar valor
    strcpy(copia, posFixa);
    float val = getValorPosFixa(copia);

    printf("Valor obtido: %.2f\nValor esperado: %.2f\n", val, valorEsperado);

    int infOk = (obtida && strcmp(normObtida, normEsperada) == 0);
    int valOk = fabs(val - valorEsperado) < 0.01;

    printf("Status: %s\n\n", (infOk && valOk) ? "PASSOU" : "FALHOU");
}

int main() {

    testarExpressao(1, "3 4 + 5 *", "(3+4)*5", 35);
    testarExpressao(2, "7 2 * 4 +", "7*2+4", 18);
    testarExpressao(3, "8 5 2 4 + * +", "8+5*(2+4)", 38);
    testarExpressao(4, "6 2 / 3 + 4 *", "(6/2+3)*4", 24);
    testarExpressao(5, "9 5 2 8 * 4 + * +", "9 + (5 * (4 + 8 * 2))", 109);
    testarExpressao(6, "2 3 + log 5 /", "log(2+3)/5", 0.14);
    testarExpressao(7, " 10 log 3 ^ 2 +", "(log10)^3 + 2", 3);
    testarExpressao(8, "45 60 + 30 cos *", "(45+60)*cos(30)", 90.93);
    testarExpressao(9, "0.5 45 sen 2 ^ +", "0.5+sen(45)^2", 1);

    return 0;
}