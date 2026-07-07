#include "GeradorCodigo.h"
#include <fstream>

void GeradorCodigo::addData(string linha)
{
    dataSection.push_back(linha);
}

void GeradorCodigo::addText(string linha)
{
    textSection.push_back(linha);
}

void GeradorCodigo::salvarArquivo(string nomeArquivo)
{
    ofstream arquivo(nomeArquivo);

    arquivo << ".DATA\n";

    for (string linha : dataSection)
        arquivo << linha << "\n";

    arquivo << "\n.TEXT\n";

    for (string linha : textSection)
        arquivo << linha << "\n";

    arquivo.close();
}

string GeradorCodigo::getCodigoCompleto()
{
    string codigo;

    codigo += ".data\n";

    for (string linha : dataSection)
        codigo += linha + "\n";

    codigo += "\n.text\n";

    for (string linha : textSection)
        codigo += linha + "\n";

    return codigo;
}

void GeradorCodigo::atualizarVariavel(string nome, string valor)
{
    for (string &linha : dataSection)
    {
        if (linha.find(nome + " :") != string::npos)
        {
            linha = nome + " : " + valor;
            return;
        }
    }
}