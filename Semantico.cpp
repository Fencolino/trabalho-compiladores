#include "Semantico.h"
#include "Constants.h"
#include "Semantic_Table.h"
#include <iostream>

using namespace std;

void Semantico::executeAction(int action, const Token *token)
{
    switch (action) {
    case 1:
        this->tipo = token->getLexeme();
        break;

    case 2: { // DECLARAÇÃO DE VARIÁVEL
        this->nome = token->getLexeme();
        adicionarSimboloTabela();
        this->tipoVariavelEsq = this->tipo;

        this->variavelLadoEsquerdo = this->nome;
        break;
    }

    case 3: { // LEITURA DE ID
        this->nome = token->getLexeme();
        this->proximoIdEhExpressao = false;

        int idx = encontrarIndiceVariavelAtiva(this->nome);

        if (idx == -1) {
            throw SemanticError("Variavel `" + this->nome + "` nao declarada neste escopo.");
        }

        this->tipoVariavelEsq = tabelaSimbolos[idx].tipo;
        this->variavelLadoEsquerdo = this->nome;
        break;
    }

    case 4: {

        string ladoEsq = this->variavelLadoEsquerdo;

        int enumEsquerda =
            traduzirTipoParaEnum(
                this->tipoVariavelEsq
                );

        if (pilhaTipos.isEmpty()) {

            throw SemanticError(
                "A expressao nao gerou um tipo resultante."
                );
        }

        int enumDireita =
            pilhaTipos.pop();

        int resultadoAtribuicao =
            SemanticTable::atribType(
                enumEsquerda,
                enumDireita
                );

        if (resultadoAtribuicao == SemanticTable::ERR) {

            throw SemanticError(
                "Variavel '" + ladoEsq +
                "' (tipo " + this->tipoVariavelEsq +
                ") nao pode receber o resultado calculado."
                );
        }
        else if (resultadoAtribuicao == SemanticTable::WAR) {

            this->avisos.append(
                "Aviso: Atribuicao para '" +
                QString::fromStdString(ladoEsq) +
                "' com possivel perda de dados."
                );
        }

        int idx =
            encontrarIndiceVariavelAtiva(
                ladoEsq
                );

        if (idx != -1) {
            tabelaSimbolos[idx].inicializado = true;
        }

        if (!pilhaOperandos.isEmpty()) {

            QString operando = pilhaOperandos.pop();

            gerarLD(operando);

            if (emAtribuicaoVetor) {
                secaoTexto.append("STO 1000");
                secaoTexto.append("LD 1002");
                secaoTexto.append("STO $indr");
                secaoTexto.append("LD 1000");
                secaoTexto.append("STOV " + QString::fromStdString(ladoEsq));
                emAtribuicaoVetor = false;
            } else {
                secaoTexto.append(
                    "STO " +
                    QString::fromStdString(ladoEsq)
                    );
            }
        }

        acumuladorCarregado = false;

        this->variavelLadoEsquerdo = "";

        break;
    }

    case 5: {
        this->contadorGeradorEscopo++;
        this->pilhaEscopo.append(this->contadorGeradorEscopo);
        cout << "\nabriu escopo " << contadorGeradorEscopo;
        break;
    }

    case 6: {

        int escopoAtual = this->pilhaEscopo.last();

        for (Simbolo &s : tabelaSimbolos) {

            if (s.Escopo == escopoAtual) {

                if (s.isRotina || s.isParametro) continue;

                if (!s.usado) {

                    QString msg =
                        "Aviso: Identificador '" +
                        QString::fromStdString(s.nome) +
                        "' declarado e nao utilizado.";

                    avisos.append(msg);
                }
            }
        }

        this->pilhaEscopo.removeLast();

        if (escopoAtual == escopoFuncaoAtual) {
            contadorParametros = 0;
            escopoFuncaoAtual = -1;
        }

        break;
    }

    case 7: {
        if (!tabelaSimbolos.isEmpty()) {
            Simbolo &ultimo = tabelaSimbolos.last();

            ultimo.isRotina = true;
            ultimo.isVariavel = false;
            ultimo.inicializado = true;

            contadorParametros = 0;
            escopoFuncaoAtual = this->pilhaEscopo.last();

            ultimo.escopoInterno = this->pilhaEscopo.last();

            tipoFuncaoAtual = ultimo.tipo;
        }
        break;
    }

    case 48: { // Inicio do corpo da funcao: gera rotulo de entrada + desvio ao redor do corpo
        int id = proximoRotuloFuncao++;
        QString rotEntrada = "FUNC" + QString::number(id);
        QString rotFim     = "FIMFUNC" + QString::number(id);

        if (!tabelaSimbolos.isEmpty()) {
            tabelaSimbolos.last().rotulo = rotEntrada.toStdString();
        }

        secaoTexto.append("JMP " + rotFim);
        secaoTexto.append(rotEntrada + ":");

        pilhaRotulosFimFuncao.push(rotFim);
        break;
    }

    case 49: { // Fim do corpo da funcao: RETURN de seguranca + rotulo de fim (destino do desvio)
        // Evita RETURN duplicado quando o ultimo comando do corpo ja foi um "return".
        if (secaoTexto.isEmpty() || secaoTexto.last() != "RETURN") {
            secaoTexto.append("RETURN");
        }

        if (!pilhaRotulosFimFuncao.isEmpty()) {
            QString rotFim = pilhaRotulosFimFuncao.pop();
            secaoTexto.append(rotFim + ":");
        }
        break;
    }

    case 8: {

        if (!tabelaSimbolos.isEmpty()) {

            Simbolo &ultimo =
                tabelaSimbolos.last();

            ultimo.isVetor = true;

            ultimo.isVariavel = false;

            ultimo.TamanhoVetor =
                stoi(token->getLexeme());
        }

        break;
    }

    case 9: { // Parâmetro
        if (!tabelaSimbolos.isEmpty()) {
            Simbolo &ultimo = tabelaSimbolos.last();

            ultimo.isParametro = true;
            ultimo.inicializado = true;

            contadorParametros++;
            ultimo.numeroParametro = contadorParametros;
        }
        break;
    }

    case 10: {
        int enumTipo;

        if (token->getId() == t_KEY_ID) {

            this->nome = token->getLexeme();
            this->proximoIdEhExpressao = true;

            string tipoDaVariavel = validarUsoVariavel(token->getLexeme());
            enumTipo = traduzirTipoParaEnum(tipoDaVariavel);
        } else {
            enumTipo = traduzirTipoParaEnum(descobrirTipoPeloToken(token->getId()));
        }

        this->pilhaTipos.push(enumTipo);

        this->pilhaOperandos.push(
            QString::fromStdString(token->getLexeme())
            );

        break;
    }

    case 13: {
        if (pilhaTipos.size() < 2) throw SemanticError("Faltam operandos na expressao.");

        int tipoDireita  = pilhaTipos.pop();
        int tipoEsquerda = pilhaTipos.pop();

        int resultadoExpr = SemanticTable::resultType(tipoEsquerda, tipoDireita, SemanticTable::REL);

        if (resultadoExpr == SemanticTable::ERR) {
            throw SemanticError("Tipos incompativeis na operacao relacional.");
        }

        this->pilhaTipos.push(resultadoExpr);

        if (!pilhaOperandos.isEmpty()) {
            QString dir = pilhaOperandos.pop();
            gerarLD(dir);
        }
        secaoTexto.append("STO 1003");
        secaoTexto.append("LD 1001");
        secaoTexto.append("SUB 1003");
        acumuladorCarregado = true;

        break;
    }

    case 11: case 12:
    case 14: case 15: case 16: case 17: case 18: case 19: {

        if (pilhaTipos.size() < 2) throw SemanticError("Faltam operandos na expressao.");

        int tipoDireita = pilhaTipos.pop();
        int tipoEsquerda = pilhaTipos.pop();

        int opTabela;
        if      (action == 11) opTabela = SemanticTable::OR_;
        else if (action == 12) opTabela = SemanticTable::AND;
        else if (action == 14) opTabela = SemanticTable::SUM;
        else if (action == 15) opTabela = SemanticTable::SUB;
        else if (action == 16) opTabela = SemanticTable::MUL;
        else if (action == 17) opTabela = SemanticTable::DIV;
        else if (action == 18) opTabela = SemanticTable::MOD;
        else                   opTabela = SemanticTable::POT;

        int resultadoExpr = SemanticTable::resultType(tipoEsquerda, tipoDireita, opTabela);

        if (resultadoExpr == SemanticTable::ERR) {
            throw SemanticError("Tipos incompativeis na operacao.");
        }

        this->pilhaTipos.push(resultadoExpr);

        if (action == 14) {

            QString direita  = pilhaOperandos.pop();
            QString esquerda = pilhaOperandos.pop();

            // ADD é comutativo: se o resultado atual do ACC está no lado direito,
            // troca os operandos para evitar perder o ACC ao carregar o esquerdo.
            if (direita == "__ACC" && esquerda != "__ACC")
                std::swap(esquerda, direita);

            if (esquerda != "__ACC") {
                if (acumuladorCarregado) {
                    // Salva o ACC atual antes de sobrescrever com o operando esquerdo
                    int t = alocarTemp();
                    secaoTexto.append("STO " + QString::number(t));
                    if (direita == "__ACC")
                        direita = "#" + QString::number(t);
                    for (int i = pilhaOperandos.size() - 1; i >= 0; i--) {
                        if (pilhaOperandos[i] == "__ACC") {
                            pilhaOperandos[i] = "#" + QString::number(t);
                            break;
                        }
                    }
                }
                gerarLD(esquerda);
                acumuladorCarregado = true;
            }

            gerarAritmetica(direita, "ADD", "ADDI");

            pilhaOperandos.push("__ACC");
        }

        if (action == 15) {

            QString direita  = pilhaOperandos.pop();
            QString esquerda = pilhaOperandos.pop();

            if (esquerda != "__ACC") {
                if (acumuladorCarregado) {
                    // Salva o ACC atual antes de sobrescrever com o operando esquerdo
                    int t = alocarTemp();
                    secaoTexto.append("STO " + QString::number(t));
                    if (direita == "__ACC")
                        direita = "#" + QString::number(t);
                    for (int i = pilhaOperandos.size() - 1; i >= 0; i--) {
                        if (pilhaOperandos[i] == "__ACC") {
                            pilhaOperandos[i] = "#" + QString::number(t);
                            break;
                        }
                    }
                }
                gerarLD(esquerda);
                acumuladorCarregado = true;
            }

            gerarAritmetica(direita, "SUB", "SUBI");

            pilhaOperandos.push("__ACC");
        }

        break;
    }

    case 20: {

        string nomeVar = token->getLexeme();

        int idx = encontrarIndiceVariavelAtiva(nomeVar);

        if (idx == -1) {
            throw SemanticError(
                "Variavel `" + nomeVar + "` nao declarada neste escopo."
                );
        }

        tabelaSimbolos[idx].inicializado = true;
        tabelaSimbolos[idx].usado = true;
        this->variavelLadoEsquerdo = nomeVar;

        break;
    }

    case 21: {

        int idx = encontrarIndiceVariavelAtiva(this->nome);

        if (idx == -1) {
            throw SemanticError(
                "Variavel `" + this->nome + "` nao declarada neste escopo."
                );
        }

        tabelaSimbolos[idx].usado = true;

        if (!tabelaSimbolos[idx].inicializado &&
            !tabelaSimbolos[idx].avisadoNaoInicializado) {

            QString msg =
                "Aviso: Variavel '" +
                QString::fromStdString(this->nome) +
                "' pode conter lixo na memoria.";

            avisos.append(msg);

            tabelaSimbolos[idx].avisadoNaoInicializado = true;
        }

        break;
    }

    case 22: {

        if (pilhaTipos.isEmpty()) {
            throw SemanticError(
                "Return sem expressao valida."
                );
        }

        int tipoRetorno = pilhaTipos.pop();

        int tipoFuncao = traduzirTipoParaEnum(tipoFuncaoAtual);

        int resultado =
            SemanticTable::atribType(tipoFuncao, tipoRetorno);

        if (resultado == SemanticTable::ERR) {

            throw SemanticError(
                "Tipo de retorno incompativel com a funcao."
                );
        }

        if (resultado == SemanticTable::WAR) {

            avisos.append(
                "Retorno da funcao pode causar perda de dados."
                );
        }

        if (!pilhaOperandos.isEmpty()) {
            QString operando = pilhaOperandos.pop();
            gerarLD(operando);
        }
        secaoTexto.append("RETURN");
        acumuladorCarregado = false;

        break;
    }

    case 23: {

        string nomeFuncao = pilhaNomesFuncoes.pop();
        QList<int> argumentos = tiposArgumentos.isEmpty() ? QList<int>() : tiposArgumentos.pop();
        bool chamadaEmExpressao = pilhaChamadaEmExpressao.isEmpty() ? false : pilhaChamadaEmExpressao.pop();

        Simbolo *funcao = nullptr;

        for (Simbolo &s : tabelaSimbolos) {
            if (s.nome == nomeFuncao && s.isRotina) {
                funcao = &s;
                break;
            }
        }

        if (funcao == nullptr) {
            throw SemanticError("Erro: a rotina `" + nomeFuncao + "` nao existe.");
        }

        QList<Simbolo> parametros = obterParametros(nomeFuncao);

        if (parametros.size() != argumentos.size()) {
            throw SemanticError("Erro: a funcao `" + nomeFuncao + "` esperava " +
                                to_string(parametros.size()) + " parametros e foram passados " +
                                to_string(argumentos.size()) + ".");
        }

        for (int i = 0; i < parametros.size(); i++) {
            int tipoParametro = traduzirTipoParaEnum(parametros[i].tipo);
            int tipoArgumento = argumentos[i];
            int resultado = SemanticTable::atribType(tipoParametro, tipoArgumento);

            if (resultado == SemanticTable::ERR) {
                throw SemanticError("Tipo incompativel no argumento " +
                                    to_string(i + 1) + " da funcao `" +
                                    nomeFuncao + "`.");
            }
        }

        secaoTexto.append("CALL " + QString::fromStdString(funcao->rotulo));

        if (chamadaEmExpressao) {
            pilhaTipos.push(traduzirTipoParaEnum(funcao->tipo));
            pilhaOperandos.push("__ACC");
            acumuladorCarregado = true;
        }

        break;
    }
    case 24: {

        if (pilhaTipos.isEmpty()) {
            throw SemanticError(
                "Argumento sem tipo valido."
                );
        }

        int tipoArg = pilhaTipos.pop();

        if (tiposArgumentos.isEmpty()) {
            throw SemanticError(
                "Erro interno: argumento fora de uma chamada de funcao."
                );
        }

        int posicao = tiposArgumentos.top().size();

        // Copia o valor do argumento para o parametro correspondente (passagem por copia).
        // So gera codigo se a posicao existir; excesso de argumentos e reportado no #23.
        if (!pilhaNomesFuncoes.isEmpty() && !pilhaOperandos.isEmpty()) {
            QList<Simbolo> parametros = obterParametros(pilhaNomesFuncoes.top());

            if (posicao < parametros.size()) {
                QString operando = pilhaOperandos.pop();
                gerarLD(operando);
                secaoTexto.append("STO " + QString::fromStdString(parametros[posicao].nome));
                acumuladorCarregado = false;
            }
        }

        tiposArgumentos.top().append(tipoArg);

        break;
    }

    case 25: {

        if (pilhaTipos.isEmpty()) {
            throw SemanticError(
                "Erro Semantico: Indice de vetor invalido."
                );
        }

        int tipoIndice = pilhaTipos.pop();

        if (tipoIndice != SemanticTable::INT) {

            throw SemanticError(
                "Erro Semantico: Indice de vetor deve ser inteiro."
                );
        }

        break;
    }

    case 27: { // COUT
        if (!pilhaOperandos.isEmpty()) {
            QString operando = pilhaOperandos.pop();
            gerarLD(operando);
            secaoTexto.append("STO $out_port");
        }

        if (!pilhaTipos.isEmpty()) {
            pilhaTipos.pop();
        }

        acumuladorCarregado = false;
        break;
    }

     case 28: {
        this->pilhaNomesFuncoes.push(this->nome);
        this->tiposArgumentos.push(QList<int>());

        this->pilhaChamadaEmExpressao.push(this->proximoIdEhExpressao);

        if (this->proximoIdEhExpressao) {
            // Descarta o placeholder que o #10 empilhou torcendo que o ID fosse
            // uma leitura de variavel comum (so descobrimos agora que era chamada).
            if (!pilhaTipos.isEmpty())     pilhaTipos.pop();
            if (!pilhaOperandos.isEmpty()) pilhaOperandos.pop();
        }
        break;
    }

    case 29: { // CIN
        secaoTexto.append("LD $in_port");
        secaoTexto.append("STO " + QString::fromStdString(this->variavelLadoEsquerdo));
        this->variavelLadoEsquerdo = "";
        break;
    }

    case 30: { // CIN
        if (!pilhaOperandos.isEmpty()) {
            QString indice = pilhaOperandos.pop();
            bool ehNumero;
            indice.toInt(&ehNumero);

            if (ehNumero) {
                secaoTexto.append("LDI " + indice);
            } else {
                secaoTexto.append("LD " + indice);
            }
            secaoTexto.append("STO $indr");
        }

        secaoTexto.append("LD $in_port");
        secaoTexto.append("STOV " + QString::fromStdString(this->variavelLadoEsquerdo));
        this->variavelLadoEsquerdo = "";
        break;
    }

    case 31: {
        if (pilhaOperandos.size() < 2) {
            throw SemanticError("Erro interno: pilha insuficiente para acesso a vetor.");
        }

        QString indice  = pilhaOperandos.pop();
        QString nomeVet = pilhaOperandos.pop();

        // O branch "if" só é válido quando há um resultado anterior no ACC
        // E o índice é uma variável simples (não foi calculado no ACC).
        // Se indice == "__ACC", o ACC tem o ÍNDICE — não um resultado anterior.
        if (acumuladorCarregado && indice != "__ACC") {
            int tPrev = alocarTemp();
            int tNew  = alocarTemp();
            secaoTexto.append("STO " + QString::number(tPrev));

            gerarLD(indice);
            secaoTexto.append("STO $indr");
            secaoTexto.append("LDV " + nomeVet);
            secaoTexto.append("STO " + QString::number(tNew));

            if (!pilhaOperandos.isEmpty() && pilhaOperandos.top() == "__ACC")
                pilhaOperandos.pop();
            pilhaOperandos.push("#" + QString::number(tPrev));
            pilhaOperandos.push("#" + QString::number(tNew));

            acumuladorCarregado = false;
        } else {
            gerarLD(indice);
            secaoTexto.append("STO $indr");
            secaoTexto.append("LDV " + nomeVet);

            pilhaOperandos.push("__ACC");
            acumuladorCarregado = true;
        }
        break;
    }

    case 32: {
        if (!pilhaOperandos.isEmpty()) {
            QString indice = pilhaOperandos.pop();
            gerarLD(indice);
            secaoTexto.append("STO 1002");
        }
        acumuladorCarregado = false;
        emAtribuicaoVetor = true;
        break;
    }

    case 34: { // Inicio do if: gera branch inverso e empilha rotulo de saida
        if (!pilhaTipos.isEmpty()) pilhaTipos.pop();

        QString rotIf = "R" + QString::number(proximoRotulo++);
        pilhaRotulos.push(rotIf);

        if      (oprel == ">")  secaoTexto.append("BLE " + rotIf);
        else if (oprel == "<")  secaoTexto.append("BGE " + rotIf);
        else if (oprel == ">=") secaoTexto.append("BLT " + rotIf);
        else if (oprel == "<=") secaoTexto.append("BGT " + rotIf);
        else if (oprel == "==") secaoTexto.append("BNE " + rotIf);
        else if (oprel == "!=") secaoTexto.append("BEQ " + rotIf);

        acumuladorCarregado = false;
        break;
    }

    case 35: { // Fim do if (simples ou composto): emite o rotulo final
        if (!pilhaRotulos.isEmpty()) {
            QString rotFim = pilhaRotulos.pop();
            secaoTexto.append(rotFim + ":");
        }
        break;
    }

    case 37: { // Inicio do while: emite rotulo de inicio e empilha
        QString rotIni = "R" + QString::number(proximoRotulo++);
        pilhaRotulos.push(rotIni);
        secaoTexto.append(rotIni + ":");
        break;
    }

    case 38: { // Apos condicao do while: branch inverso para rotulo de saida
        if (!pilhaTipos.isEmpty()) pilhaTipos.pop();

        QString rotFim = "R" + QString::number(proximoRotulo++);
        pilhaRotulos.push(rotFim);

        if      (oprel == ">")  secaoTexto.append("BLE " + rotFim);
        else if (oprel == "<")  secaoTexto.append("BGE " + rotFim);
        else if (oprel == ">=") secaoTexto.append("BLT " + rotFim);
        else if (oprel == "<=") secaoTexto.append("BGT " + rotFim);
        else if (oprel == "==") secaoTexto.append("BNE " + rotFim);
        else if (oprel == "!=") secaoTexto.append("BEQ " + rotFim);

        acumuladorCarregado = false;
        break;
    }

    case 39: { // Fim do while: JMP para inicio, emite rotulo de saida
        if (pilhaRotulos.size() >= 2) {
            QString rotFim = pilhaRotulos.pop();
            QString rotIni = pilhaRotulos.pop();
            secaoTexto.append("JMP " + rotIni);
            secaoTexto.append(rotFim + ":");
        }
        break;
    }

    case 42: { // For: apos init, antes da condicao — emite rotulo de condicao, prepara labels
        QString rotCond = "R" + QString::number(proximoRotulo++);
        QString rotFim  = "R" + QString::number(proximoRotulo++);
        secaoTexto.append(rotCond + ":");
        pilhaRotulos.push(rotCond);
        pilhaRotulos.push(rotFim);
        pilhaStepFor.push(QStringList());
        break;
    }

    case 43: { // For: apos condicao — branch inverso para fim, marca inicio do step
        if (!pilhaTipos.isEmpty()) pilhaTipos.pop();

        QString rotFim = pilhaRotulos.top(); // peek sem remover

        if      (oprel == ">")  secaoTexto.append("BLE " + rotFim);
        else if (oprel == "<")  secaoTexto.append("BGE " + rotFim);
        else if (oprel == ">=") secaoTexto.append("BLT " + rotFim);
        else if (oprel == "<=") secaoTexto.append("BGT " + rotFim);
        else if (oprel == "==") secaoTexto.append("BNE " + rotFim);
        else if (oprel == "!=") secaoTexto.append("BEQ " + rotFim);

        forStepBufferIndex = secaoTexto.size();
        acumuladorCarregado = false;
        break;
    }

    case 44: { // For: apos step, antes do corpo — extrai codigo do step para buffer
        if (!pilhaStepFor.isEmpty()) {
            QStringList &buffer = pilhaStepFor.top();
            for (int i = forStepBufferIndex; i < secaoTexto.size(); i++)
                buffer.append(secaoTexto[i]);
            while (secaoTexto.size() > forStepBufferIndex)
                secaoTexto.removeLast();
        }
        break;
    }

    case 45: { // For: apos corpo — despeja step, JMP para condicao, emite rotulo de fim
        if (!pilhaStepFor.isEmpty()) {
            QStringList stepBuffer = pilhaStepFor.pop();
            for (const QString &linha : stepBuffer)
                secaoTexto.append(linha);
        }
        if (pilhaRotulos.size() >= 2) {
            QString rotFim  = pilhaRotulos.pop();
            QString rotCond = pilhaRotulos.pop();
            secaoTexto.append("JMP " + rotCond);
            secaoTexto.append(rotFim + ":");
        }
        break;
    }

    case 46: { // Incremento no step do for (i++)
        QString var = QString::fromStdString(this->nome);
        secaoTexto.append("LD "   + var);
        secaoTexto.append("ADDI 1");
        secaoTexto.append("STO "  + var);
        acumuladorCarregado = false;
        break;
    }

    case 47: { // Decremento no step do for (i--)
        QString var = QString::fromStdString(this->nome);
        secaoTexto.append("LD "   + var);
        secaoTexto.append("SUBI 1");
        secaoTexto.append("STO "  + var);
        acumuladorCarregado = false;
        break;
    }

    case 40: { // Inicio do do-while: emite rotulo de inicio e empilha
        QString rotIni = "R" + QString::number(proximoRotulo++);
        pilhaRotulos.push(rotIni);
        secaoTexto.append(rotIni + ":");
        break;
    }

    case 41: { // Fim do do-while: branch DIRETO (mesmo sentido) de volta ao inicio
        if (!pilhaTipos.isEmpty()) pilhaTipos.pop();

        if (!pilhaRotulos.isEmpty()) {
            QString rotIni = pilhaRotulos.pop();

            if      (oprel == ">")  secaoTexto.append("BGT " + rotIni);
            else if (oprel == "<")  secaoTexto.append("BLT " + rotIni);
            else if (oprel == ">=") secaoTexto.append("BGE " + rotIni);
            else if (oprel == "<=") secaoTexto.append("BLE " + rotIni);
            else if (oprel == "==") secaoTexto.append("BEQ " + rotIni);
            else if (oprel == "!=") secaoTexto.append("BNE " + rotIni);
        }

        acumuladorCarregado = false;
        break;
    }

    case 36: { // else: gera JMP para fim, emite rotulo do bloco falso
        if (!pilhaRotulos.isEmpty()) {
            QString rotIf  = pilhaRotulos.pop();
            QString rotFim = "R" + QString::number(proximoRotulo++);
            pilhaRotulos.push(rotFim);
            secaoTexto.append("JMP " + rotFim);
            secaoTexto.append(rotIf + ":");
        }
        break;
    }

    case 33: { // Salva operador relacional e gera STO do lado esquerdo
        oprel = token->getLexeme();

        if (!pilhaOperandos.isEmpty()) {
            QString esq = pilhaOperandos.pop();
            gerarLD(esq);
        }
        secaoTexto.append("STO 1001");
        acumuladorCarregado = false;

        break;
    }

    default:
        break;
    }
}

// LIMITACAO CONHECIDA: a unicidade abaixo e' checada so' dentro do MESMO escopo.
// gerarSecaoData()/STO/LD usam o nome do identificador cru como endereco de
// memoria (sem prefixo de escopo/funcao), entao duas funcoes/blocos diferentes
// que declarem parametro ou variavel local com o mesmo nome (ex: dois "int a")
// vao compartilhar a MESMA celula de memoria em ".data" mesmo sendo simbolos
// semanticamente distintos. Ate isso ser corrigido (nomes teriam que ser
// qualificados por escopo/rotulo da funcao), evite reusar nomes de
// parametro/variavel local entre funcoes diferentes.
void Semantico::adicionarSimboloTabela() {
    int escopoAtual = this->pilhaEscopo.last();

    for (const Simbolo &s : this->tabelaSimbolos) {
        if (s.nome == this->nome && s.Escopo == escopoAtual) {
            string msgErro = "Identificador `" + this->nome + "` ja existe neste escopo";
            throw SemanticError(msgErro);
        }
    }

    Simbolo s;
    s.tipo = this->tipo;
    s.nome = this->nome;
    s.Escopo = escopoAtual;
    s.usado = false;

    s.isVariavel = true;
    s.isVetor = false;
    s.isRotina = false;
    s.isParametro = false;
    s.inicializado = false;
    s.avisadoNaoInicializado = false;
    s.escopoInterno = -1;

    this->tabelaSimbolos.append(s);
    cout << "\nescopo da variavel " << this->nome << " = " << escopoAtual;
}

bool Semantico::verificarExistencia() {
    bool encontrado = false;
    for (const Simbolo &s : this->tabelaSimbolos) {
        if (s.nome == this->nome) {
            this->tipoVariavelEsq = s.tipo;
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        string msgErro = "Erro Semantico: Variavel `" + this->nome + "` nao declarada.";
        throw SemanticError(msgErro);
    }
    return encontrado;
}

string Semantico::descobrirTipoPeloToken(int id) {
    if (id == t_KEY_INT_VALOR)         return "int";
    if (id == t_KEY_REAL_VALOR)        return "float";
    if (id == t_KEY_CHAR_VALOR)        return "char";
    if (id == t_KEY_STRING_VALOR)      return "string";
    if (id == t_KEY_BINARIO_VALOR)     return "int";
    if (id == t_KEY_HEXADECIMAL_VALOR) return "int";
    if (id == t_KEY_TRUE)  return "bool";
    if (id == t_KEY_FALSE) return "bool";

    return "desconhecido";
}

string Semantico::buscarTipoVariavel(string nomeVar) {
    for (const Simbolo &s : this->tabelaSimbolos) {
        if (s.nome == nomeVar) {
            return s.tipo;
        }
    }
    return "desconhecido";
}

int Semantico::traduzirTipoParaEnum(string tipoTexto) {
    if (tipoTexto == "int") return SemanticTable::INT;
    if (tipoTexto == "float") return SemanticTable::FLO;
    if (tipoTexto == "char") return SemanticTable::CHA;
    if (tipoTexto == "string") return SemanticTable::STR;
    if (tipoTexto == "bool") return SemanticTable::BOO;
    return SemanticTable::ERR;
}

int Semantico::traduzirOperadorParaEnum(int tokenId) {
    if (tokenId == t_KEY_SOMA) return SemanticTable::SUM;
    if (tokenId == t_KEY_SUBTRACAO) return SemanticTable::SUB;
    if (tokenId == t_KEY_MULTIPLICACAO) return SemanticTable::MUL;
    if (tokenId == t_KEY_DIVISAO) return SemanticTable::DIV;
    if (tokenId == t_KEY_MAIOR_QUE) return SemanticTable::REL;
    if (tokenId == t_KEY_MENOR_QUE) return SemanticTable::REL;
    if (tokenId == t_KEY_MAIOR_OU_IGUAL) return SemanticTable::REL;
    if (tokenId == t_KEY_MENOR_OU_IGUAL) return SemanticTable::REL;
    if (tokenId == t_KEY_IGUAL) return SemanticTable::REL;
    if (tokenId == t_KEY_DIFERENTE) return SemanticTable::REL;
    if (tokenId == t_KEY_MODULO) return SemanticTable::MOD;
    if (tokenId == t_KEY_EXPONENCIACAO) return SemanticTable::POT;
    if (tokenId == t_KEY_AND_LOGICO) return SemanticTable::AND;
    if (tokenId == t_KEY_OR_LOGICO) return SemanticTable::OR_;

    return -1;

}

int Semantico::encontrarIndiceVariavelAtiva(string nomeDesejado) {
    for (int i = tabelaSimbolos.size() - 1; i >= 0; i--) {
        if (tabelaSimbolos[i].nome == nomeDesejado && this->pilhaEscopo.contains(tabelaSimbolos[i].Escopo)) {
            return i;
        }
    }
    return -1;
}

// Retorna os parametros de uma funcao/procedimento, na ordem de declaracao.
QList<Simbolo> Semantico::obterParametros(const string& nomeFuncao) {
    QList<Simbolo> parametros;

    int escopoInterno = -1;
    for (const Simbolo &s : tabelaSimbolos) {
        if (s.nome == nomeFuncao && s.isRotina) {
            escopoInterno = s.escopoInterno;
            break;
        }
    }

    if (escopoInterno == -1) return parametros;

    for (const Simbolo &s : tabelaSimbolos) {
        if (s.isParametro && s.Escopo == escopoInterno) {
            parametros.append(s);
        }
    }

    return parametros;
}

string Semantico::validarUsoVariavel(string nomeDesejado) {
    int idx = encontrarIndiceVariavelAtiva(nomeDesejado);

    if (idx == -1) {
        throw SemanticError("Erro Semantico: Variavel `" + nomeDesejado + "` nao declarada neste escopo.");
    }

    tabelaSimbolos[idx].usado = true;

    if (!tabelaSimbolos[idx].inicializado && !tabelaSimbolos[idx].avisadoNaoInicializado) {
        QString msg = "Aviso: Variavel '" + QString::fromStdString(nomeDesejado) + "' pode conter lixo na memoria.";
        this->avisos.append(msg);
        tabelaSimbolos[idx].avisadoNaoInicializado = true;
    }

    return tabelaSimbolos[idx].tipo;
}

void Semantico::verificarVariaveisGlobaisNaoUsadas() {
    for (Simbolo &s : tabelaSimbolos) {
        if (s.Escopo == 0) {

            if (s.isRotina || s.isParametro) continue;

            if (!s.usado) {
                QString msg = "Aviso: Identificador '" + QString::fromStdString(s.nome) +
                              "' declarado e nao utilizado.";
                avisos.append(msg);
            }
        }
    }
}

QString Semantico::gerarCodigoAssembly() {

    QString codigo;

    codigo += ".data\n";

    for (const QString &linha : secaoData) {
        codigo += linha + "\n";
    }

    codigo += "\n.text\n";

    for (const QString &linha : secaoTexto) {
        codigo += linha + "\n";
    }

    return codigo;
}

void Semantico::gerarSecaoData() {

    secaoData.clear();

    for (const Simbolo &s : tabelaSimbolos) {

        if (s.isRotina)
            continue;

        if (s.isVetor) {

            QString linha =
                QString::fromStdString(s.nome) +
                " : ";

            for (int i = 0; i < s.TamanhoVetor; i++) {

                linha += "0";

                if (i < s.TamanhoVetor - 1) {
                    linha += ",";
                }
            }

            secaoData.append(linha);
        }

        else {

            secaoData.append(
                QString::fromStdString(s.nome) +
                " : 0"
                );
        }
    }

    // O Bipide nao aceita uma secao .data vazia (programa sem nenhuma
    // variavel declarada, ex: so' funcoes/literais). Garante uma linha minima.
    if (secaoData.isEmpty()) {
        secaoData.append("__pad : 0");
    }
}

void Semantico::gerarLD(const QString& op) {
    if (op == "__ACC") return;
    if (op.startsWith("#")) {
        secaoTexto.append("LD " + op.mid(1));
        return;
    }
    bool ehNumero;
    op.toInt(&ehNumero);
    if (ehNumero) secaoTexto.append("LDI " + op);
    else          secaoTexto.append("LD "  + op);
}

// Gera instrucao aritmetica (ADD/SUB etc.) para o operando direito.
// instrMem = "ADD", instrImm = "ADDI"
void Semantico::gerarAritmetica(const QString& op, const QString& instrMem, const QString& instrImm) {
    if (op.startsWith("#")) {
        secaoTexto.append(instrMem + " " + op.mid(1));
        return;
    }
    bool ehNumero;
    op.toInt(&ehNumero);
    if (ehNumero) secaoTexto.append(instrImm + " " + op);
    else          secaoTexto.append(instrMem + " " + op);
}