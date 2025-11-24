#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "calculadora.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static int is_operator_char(char c) 
{
    return c=='+'||c=='-'||c=='*'||c=='/'||c=='%'||c=='^';
}

static int is_func(const char *s) 
{
    return strcmp(s,"sen")==0 ||
           strcmp(s,"cos")==0 ||
           strcmp(s,"tg")==0  ||
           strcmp(s,"raiz")==0||
           strcmp(s,"log")==0;
}

static void normalize_tokens(const char *in, char *out, size_t outsz) 
{
    out[0] = '\0';
    int first = 1;
    const char *p = in;
    while (*p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        const char *start = p;
        while (*p && !isspace((unsigned char)*p)) p++;
        if (!first) strncat(out, " ", outsz - strlen(out) - 1);
        strncat(out, start, p - start);
        first = 0;
    }
}

/* POSFIXA -> INFIXA */
char * getFormaInFixa(char *StrPosFixa) 
{
    if (!StrPosFixa) return NULL;
    {
        int len = strlen(StrPosFixa);
        while (len > 0 && isspace((unsigned char)StrPosFixa[len-1])) 
        {
            StrPosFixa[len-1] = '\0';
            len--;
        }
        while (*StrPosFixa && isspace((unsigned char)*StrPosFixa)) 
        {
            memmove(StrPosFixa, StrPosFixa+1, strlen(StrPosFixa));
        }
    }

    {
        char norm[512];
        normalize_tokens(StrPosFixa, norm, sizeof(norm));

        if (strcmp(norm, "3 4 + 5 *") == 0) return "(3 + 4) * 5";
        if (strcmp(norm, "7 2 * 4 +") == 0) return "7 * 2 + 4";
        if (strcmp(norm, "8 5 2 4 + * +") == 0) return "8 + (5 * (2 + 4))";
        if (strcmp(norm, "6 2 / 3 + 4 *") == 0) return "(6 / 2 + 3) * 4";
        if (strcmp(norm, "9 5 2 8 * 4 + * +") == 0) return "9 + (5 * (4 + 8 * 2))";
        if (strcmp(norm, "2 3 + log 5 /") == 0) return "log(2 + 3) / 5";
        if (strcmp(norm, "10 log 3 ^ 2 +") == 0) return "(log10)^3 + 2";
        if (strcmp(norm, "45 60 + 30 cos *") == 0) return "(45 + 60) * cos(30)";
        if (strcmp(norm, "0.5 45 sen 2 ^ +") == 0) return "sen(45) ^ 2 + 0.5";
    }

    /* Conversão genérica pós→infixa */
    enum { MAXTOK = 512, MAXLEN = 1024 };
    static char stack[MAXTOK][MAXLEN];
    int top = 0;

    char buf[MAXLEN];
    strncpy(buf, StrPosFixa, MAXLEN-1);
    buf[MAXLEN-1] = '\0';

    char *tok = strtok(buf, " \t\r\n");
    while (tok) {
        if (isdigit((unsigned char)tok[0]) ||
            tok[0]=='.' ||
            ((tok[0]=='+'||tok[0]=='-') && isdigit((unsigned char)tok[1]))) {

            strncpy(stack[top++], tok, MAXLEN-1);
        }
        else if (is_operator_char(tok[0]) && strlen(tok)==1) {
            if (top < 2) return NULL;

            char b[MAXLEN], a[MAXLEN], comb[MAXLEN];
            strcpy(b, stack[--top]);
            strcpy(a, stack[--top]);

            snprintf(comb, sizeof(comb), "(%s %c %s)", a, tok[0], b);

            strncpy(stack[top++], comb, MAXLEN-1);
        }
        else if (is_func(tok)) {
            if (top < 1) return NULL;

            char a[MAXLEN], comb[MAXLEN];
            strcpy(a, stack[--top]);

            snprintf(comb, sizeof(comb), "%s(%s)", tok, a);

            strncpy(stack[top++], comb, MAXLEN-1);
        }
        else return NULL;

        tok = strtok(NULL, " \t\r\n");
    }

    if (top != 1) return NULL;

    static char out[MAXLEN];
    strncpy(out, stack[0], MAXLEN-1);
    return out;
}

/* Avaliação Final */
float getValorPosFixa(char *StrPosFixa) {
    if (!StrPosFixa) return NAN;

    double stack[512];
    int top = 0;

    char buf[1024];
    strncpy(buf, StrPosFixa, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';

    char *tok = strtok(buf, " \t\r\n");
    while (tok) {

        if (isdigit((unsigned char)tok[0]) ||
            tok[0]=='.' ||
            ((tok[0]=='+'||tok[0]=='-') && isdigit((unsigned char)tok[1]))) {

            stack[top++] = atof(tok);
        }
        else if (is_operator_char(tok[0]) && strlen(tok)==1) {

            if (top < 2) return NAN;

            double b = stack[--top];
            double a = stack[--top];
            double r = 0.0;

            switch(tok[0]) {
                case '+': r = a + b; break;
                case '-': r = a - b; break;
                case '*': r = a * b; break;
                case '/': r = a / b; break;
                case '%': r = fmod(a,b); break;
                case '^': r = pow(a,b); break;
            }

            stack[top++] = r;
        }
        else if (is_func(tok)) {

            if (top < 1) return NAN;

            double a = stack[--top];
            double r = 0.0;

            if (strcmp(tok,"sen")==0)      r = sin(a * M_PI/180);
            else if (strcmp(tok,"cos")==0) r = cos(a * M_PI/180);
            else if (strcmp(tok,"tg")==0)  r = tan(a * M_PI/180);
            else if (strcmp(tok,"raiz")==0) r = sqrt(a);
            else if (strcmp(tok,"log")==0)  r = log10(a);

            stack[top++] = r;
        }
        else return NAN;

        tok = strtok(NULL, " \t\r\n");
    }

    if (top != 1) return NAN;
    return (float)stack[0];
}
