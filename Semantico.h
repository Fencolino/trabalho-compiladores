#ifndef SEMANTICO_H
#define SEMANTICO_H

#include <string>
#include <QList>
#include "Token.h"
#include "SemanticError.h"
#include <QStack>
#include <QStringList>

using namespace std;

class Simbolo {
public:
    string tipo;
    string nome;
    bool inicializado;
    bool usado;
    int Escopo;
    int escopoInterno;

    bool isVariavel;
    bool isVetor;
    int TamanhoVetor;

    bool isRotina;
    bool isParametro;

   int numeroParametro = 0;
    bool avisadoNaoInicializado = false;
    string rotulo; // rotulo de entrada no assembly, valido quando isRotina == true
};

// UTILIZAR PILHA COMO ESCOPO, PARA GERENCIAR OS ESCOPOS, EMPILHAR QUANDO ABRIR CHAVES E DESEMPILHAR QUANDO FECHAR CHAVES

class Semantico {
public:

    Semantico() {
        contadorGeradorEscopo = 0;
        pilhaEscopo.append(0); // escopo global
    }

    QList<Simbolo> tabelaSimbolos;
    QStringList avisos;


    void executeAction(int action, const Token *token);
    void verificarVariaveisGlobaisNaoUsadas();

    QString gerarCodigoAssembly();
    void gerarSecaoData();

private:
    string tipo;
    string nome;
    string tipoVariavelEsq;
    string nomeLadoEsquerdo;
    string variavelLadoEsquerdo = "";
    string tipoFuncaoAtual = "";
    int indiceUltimaFuncao = -1;
    int contadorParametros = 0;
    int escopoFuncaoAtual = -1;

    int contadorGeradorEscopo;

    QList<int> pilhaEscopo;
    QStack<int> pilhaTipos;

    QStack<int> pilhaExpressao;
    QStack<QList<int>> tiposArgumentos; // uma lista por chamada em andamento (suporta chamada aninhada como argumento)
    QStack<string> pilhaNomesFuncoes;

    bool proximoIdEhExpressao = false; // true quando o ID leu via #10 (pode virar chamada em expressao), false via #3 (comando)
    QStack<bool> pilhaChamadaEmExpressao; // contexto (expressao x comando) de cada chamada em andamento

    QStack<QString> pilhaOperandos;
    QStringList secaoData;
    QStringList secaoTexto;

    bool emAtribuicao = false;
    bool proximaVarEhVetor = false;
    bool proximaVarEhParam = false;
    bool proximaVarEhRotina = false;
    bool ignorarProximoID = false;

    bool acumuladorCarregado = false;
    bool emAtribuicaoVetor = false;

    string oprel; // operador relacional corrente: ">", "<", ">=", "<=", "==", "!="

    int proximoRotulo = 1;
    QStack<QString> pilhaRotulos;

    int proximoRotuloFuncao = 1;
    QStack<QString> pilhaRotulosFimFuncao; // rotulo de "fim" (skip) de cada funcao em declaracao, para o JMP-around

    QStack<QStringList> pilhaStepFor; // buffer de step para fors aninhados
    int forStepBufferIndex = -1;       // posicao em secaoTexto onde o step comeca

    int proximoTemp = 1004;
    int alocarTemp() { return proximoTemp++; }

    void adicionarSimboloTabela();
    bool verificarExistencia();
    string descobrirTipoPeloToken(int id);
    string buscarTipoVariavel(string nomeVar);
    string variavelSendoAtribuida = "";
    int traduzirTipoParaEnum(string tipoTexto);
    int traduzirOperadorParaEnum(int tokenId);
    int encontrarIndiceVariavelAtiva(string nomeDesejado);
    string validarUsoVariavel(string nomeDesejado);
    QList<Simbolo> obterParametros(const string& nomeFuncao);

    // Helpers de geracao de codigo
    // Operandos com prefixo "#" (ex: "#1000") sao enderecos de memoria interna
    void gerarLD(const QString& op);
    void gerarAritmetica(const QString& op, const QString& instrMem, const QString& instrImm);

};

#endif