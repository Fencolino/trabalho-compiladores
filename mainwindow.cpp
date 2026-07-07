#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Lexico.h"
#include "Sintatico.h"
#include "Semantico.h"
#include <string>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->BotaoCompilar,
            SIGNAL(clicked(bool)),
            this,
            SLOT(tratarBotaoCompilar())
            );

    QFont font = ui->CampoSaida->font();
    font.setPointSize(14);
    ui->CampoSaida->setFont(font);
    ui->CampoEntrada->setFont(font);

    ui->tabelaSimbolos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::preencherTabelaSimbolos(const QList<Simbolo> &tabela) {

    ui->tabelaSimbolos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tabelaSimbolos->clearContents();
    ui->tabelaSimbolos->setRowCount(0);

    for (int i = 0; i < tabela.size(); i++) {

        const Simbolo &s = tabela[i];

        ui->tabelaSimbolos->insertRow(i);

        ui->tabelaSimbolos->setItem(i, 0,
                                    new QTableWidgetItem(QString::fromStdString(s.nome)));

        ui->tabelaSimbolos->setItem(i, 1,
                                    new QTableWidgetItem(QString::fromStdString(s.tipo)));

        QString categoria = "Variável";
        if (s.isVetor) categoria = "Vetor";
        else if (s.isRotina) categoria = "Função";
        else if (s.isParametro) categoria = "Parâmetro";

        ui->tabelaSimbolos->setItem(i, 2,
                                    new QTableWidgetItem(categoria));

        ui->tabelaSimbolos->setItem(i, 3,
                                    new QTableWidgetItem(QString::number(s.Escopo)));
        ui->tabelaSimbolos->setItem(i, 4,
                                    new QTableWidgetItem(s.inicializado ? "Sim" : "Não"));

        ui->tabelaSimbolos->setItem(i, 5,
                                    new QTableWidgetItem(s.usado ? "Sim" : "Não"));

        QString numParam = (s.isParametro)
                               ? QString::number(s.numeroParametro)
                               : "-";

        ui->tabelaSimbolos->setItem(i, 6,
                                    new QTableWidgetItem(numParam));
    }

    ui->tabelaSimbolos->resizeColumnsToContents();
}

void MainWindow::tratarBotaoCompilar() {
    ui->CampoSaida->clear();
    ui->CampoAssembly->clear();

    ui->tabelaSimbolos->clearContents();
    ui->tabelaSimbolos->setRowCount(0);

    QString codigoParaCompilar = ui->CampoEntrada->toPlainText();

    if (codigoParaCompilar.trimmed().isEmpty()) {
        ui->CampoSaida->append("Aviso: Digite algum código antes de compilar.");
        return;
    }

    Lexico lex;
    Sintatico sint;
    Semantico sem;

    sem.avisos.clear();

    std::string textoC = codigoParaCompilar.toStdString();
    lex.setInput(textoC.c_str());

    try {
        sint.parse(&lex, &sem);

        sem.verificarVariaveisGlobaisNaoUsadas();

        sem.gerarSecaoData();

        QString codigoAsm =
            sem.gerarCodigoAssembly();

        ui->CampoAssembly->setText(codigoAsm);

        ui->CampoSaida->append(">>> Programa compilado sem erros fatais!");

        preencherTabelaSimbolos(sem.tabelaSimbolos);

        if (!sem.avisos.isEmpty()) {
            for (const QString &aviso : sem.avisos) {
                ui->CampoSaida->append(aviso);
            }
        }

    }
    catch (const LexicalError &err) {
        QString msg = QString::fromStdString(err.getMessage());
        ui->CampoSaida->append("ERRO LÉXICO: " + msg);
    }
    catch (const SyntacticError &err) {
        QString msg = QString::fromStdString(err.getMessage());
        ui->CampoSaida->append("ERRO SINTÁTICO: " + msg);
    }
    catch (const SemanticError &err) {
        QString msg = QString::fromStdString(err.getMessage());
        ui->CampoSaida->append("ERRO SEMÂNTICO: " + msg);

        preencherTabelaSimbolos(sem.tabelaSimbolos);

        for (const QString &aviso : sem.avisos) {
            ui->CampoSaida->append(aviso);
        }
    }
    catch (const AnalysisError* err) {
        if (err) {
            ui->CampoSaida->append("Erro de Análise (Ponteiro): " +
                                   QString::fromStdString(err->getMessage()));
        }
    }
    catch (...) {
        ui->CampoSaida->append("ERRO CRÍTICO: Ocorreu uma exceção desconhecida.");
    }
}