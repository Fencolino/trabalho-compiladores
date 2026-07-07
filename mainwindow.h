#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "Semantico.h" // necessário para usar Simbolo

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void tratarBotaoCompilar();

private:
    Ui::MainWindow *ui;

    // 🔹 NOVO: método para preencher a tabela
    void preencherTabelaSimbolos(const QList<Simbolo> &tabela);
};

#endif // MAINWINDOW_H