#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "calculadora.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Funções auxiliares
static int is_operator(char c) {
    return c=='+' || c=='-' || c=='*' || c=='/' || c=='%' || c=='^';
}

static int is_func(const char *s) {
    return strcmp(s, "sen")==0 ||
           strcmp(s, "cos")==0 ||
           strcmp(s, "tg")==0  ||
           strcmp(s, "raiz")==0 ||
           strcmp(s, "log")==0;
}

// Conversão Pós-Fixa → In-Fixa
char * getFormaInFixa(char *StrPosFixa) {

    if (!StrPosFixa) return NULL;

    static char out[1024];
    char stack[512][1024];
    int top = 0;

    char buffer[1024];
    strncpy(buffer, StrPosFixa, sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';

    char *tok = strtok(buffer, " \t\n\r");

    while (tok) {

        if (isdigit(tok[0]) || tok[0]=='.' ||
            ((tok[0]=='+' || tok[0]=='-') && isdigit(tok[1]))) {

            strcpy(stack[top++], tok);
        }

        else if (strlen(tok)==1 && is_operator(tok[0])) {

            if (top < 2) return NULL;

            char b[1024], a[1024], comb[1024];
            strcpy(b, stack[--top]);
            strcpy(a, stack[--top]);

            sprintf(comb, "(%s%c%s)", a, tok[0], b);
            strcpy(stack[top++], comb);
        }

        // Função unária 
        else if (is_func(tok)) {

            if (top < 1) return NULL;

            char a[1024], comb[1024];
            strcpy(a, stack[--top]);

            sprintf(comb, "%s(%s)", tok, a);
            strcpy(stack[top++], comb);
        }

        else {
            return NULL; 
        }

        tok = strtok(NULL, " \t\n\r");
    }

    if (top != 1) return NULL;

    strcpy(out, stack[0]);
    char compact[1024];
    int p = 0;

    for (int i=0; out[i]; i++)
        if (!isspace((unsigned char)out[i]))
            compact[p++] = out[i];

    compact[p] = '\0';
    strcpy(out, compact);

    return out;
}


// Avaliação Pós-Fixa
float getValorPosFixa(char *StrPosFixa) {

    if (!StrPosFixa) return NAN;

    double stack[512];
    int top = 0;

    char buffer[1024];
    strncpy(buffer, StrPosFixa, sizeof(buffer)-1);
    buffer[sizeof(buffer)-1] = '\0';

    char *tok = strtok(buffer, " \t\n\r");

    while (tok) {

        if (isdigit(tok[0]) || tok[0]=='.' ||
            ((tok[0]=='+' || tok[0]=='-') && isdigit(tok[1]))) {

            stack[top++] = atof(tok);
        }

        else if (strlen(tok)==1 && is_operator(tok[0])) {

            if (top < 2) return NAN;

            double b = stack[--top];
            double a = stack[--top];
            double r = 0.0;

            switch(tok[0]) {
                case '+': r = a + b; break;
                case '-': r = a - b; break;
                case '*': r = a * b; break;
                case '/': r = a / b; break;
                case '%': r = fmod(a, b); break;
                case '^': r = pow(a, b); break;
                default: return NAN;
            }

            stack[top++] = r;
        }

        // Função unária 
        else if (is_func(tok)) {

            if (top < 1) return NAN;
            double a = stack[--top];
            double r = 0.0;

            if (!strcmp(tok, "sen"))      r = sin(a * M_PI / 180.0);
            else if (!strcmp(tok, "cos")) r = cos(a * M_PI / 180.0);
            else if (!strcmp(tok, "tg"))  r = tan(a * M_PI / 180.0);
            else if (!strcmp(tok, "raiz")) r = sqrt(a);
            else if (!strcmp(tok, "log"))  r = log10(a);
            else return NAN;

            stack[top++] = r;
        }

        else return NAN;

        tok = strtok(NULL, " \t\n\r");
    }

    return (top == 1) ? (float)stack[0] : NAN;
}
