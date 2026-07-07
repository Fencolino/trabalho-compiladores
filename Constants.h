#ifndef CONSTANTS_H
#define CONSTANTS_H

enum TokenId 
{
    EPSILON  = 0,
    DOLLAR   = 1,
    t_KEY_CIN = 2,
    t_KEY_COUT = 3,
    t_KEY_IF = 4,
    t_KEY_ELSE = 5,
    t_KEY_SWITCH = 6,
    t_KEY_CASE = 7,
    t_KEY_DEFAULT = 8,
    t_KEY_FOR = 9,
    t_KEY_WHILE = 10,
    t_KEY_DO = 11,
    t_KEY_BREAK = 12,
    t_KEY_CONTINUE = 13,
    t_KEY_RETURN = 14,
    t_KEY_INT = 15,
    t_KEY_FLOAT = 16,
    t_KEY_DOUBLE = 17,
    t_KEY_CHAR = 18,
    t_KEY_STRING = 19,
    t_KEY_BOOL = 20,
    t_KEY_TRUE = 21,
    t_KEY_FALSE = 22,
    t_KEY_VOID = 23,
    t_KEY_SOMA = 24,
    t_KEY_SUBTRACAO = 25,
    t_KEY_MULTIPLICACAO = 26,
    t_KEY_DIVISAO = 27,
    t_KEY_MODULO = 28,
    t_KEY_EXPONENCIACAO = 29,
    t_KEY_DIVISAO_INTEIRA = 30,
    t_KEY_INCREMENTO = 31,
    t_KEY_DECREMENTO = 32,
    t_KEY_ATRIBUICAO = 33,
    t_KEY_MAIOR_QUE = 34,
    t_KEY_MENOR_QUE = 35,
    t_KEY_MAIOR_OU_IGUAL = 36,
    t_KEY_MENOR_OU_IGUAL = 37,
    t_KEY_IGUAL = 38,
    t_KEY_DIFERENTE = 39,
    t_KEY_AND_LOGICO = 40,
    t_KEY_OR_LOGICO = 41,
    t_KEY_NEGACAO = 42,
    t_KEY_DESLOCAMENTO_DIREITA = 43,
    t_KEY_DESLOCAMENTO_ESQUERDA = 44,
    t_KEY_E_BIT_A_BIT = 45,
    t_KEY_OU_BIT_A_BIT = 46,
    t_KEY_OU_EXCLUSIVO_BIT_A_BIT = 47,
    t_KEY_NAO_BIT_A_BIT = 48,
    t_KEY_ID = 49,
    t_KEY_INT_VALOR = 50,
    t_KEY_BINARIO_VALOR = 51,
    t_KEY_HEXADECIMAL_VALOR = 52,
    t_KEY_REAL_VALOR = 53,
    t_KEY_CHAR_VALOR = 54,
    t_KEY_STRING_VALOR = 55,
    t_KEY_ABRE_CHAVES = 56,
    t_KEY_FECHA_CHAVES = 57,
    t_KEY_ABRE_COLCHETES = 58,
    t_KEY_FECHA_COLCHETES = 59,
    t_KEY_ABRE_PARENTESES = 60,
    t_KEY_FECHA_PARENTESES = 61,
    t_KEY_PONTO_E_VIGULA = 62,
    t_KEY_VIGULA = 63,
    t_KEY_PONTO = 64,
    t_KEY_DOIS_PONTOS = 65
};

const int STATES_COUNT = 145;

extern int SCANNER_TABLE[STATES_COUNT][256];

extern int TOKEN_STATE[STATES_COUNT];

extern const char *SCANNER_ERROR[STATES_COUNT];

const int FIRST_SEMANTIC_ACTION = 131;

const int SHIFT  = 0;
const int REDUCE = 1;
const int ACTION = 2;
const int ACCEPT = 3;
const int GO_TO  = 4;
const int ERROR  = 5;

extern const int PARSER_TABLE[348][181][2];

extern const int PRODUCTIONS[149][2];

extern const char *PARSER_ERROR[348];

#endif
