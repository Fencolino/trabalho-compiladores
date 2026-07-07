#ifndef GERADORCODIGO_H
#define GERADORCODIGO_H

#include <vector>
#include <string>

using namespace std;

class GeradorCodigo
{
private:
    vector<string> dataSection;
    vector<string> textSection;

public:
    void addData(string linha);
    void addText(string linha);

    void salvarArquivo(string nomeArquivo);
    void atualizarVariavel(string nome, string valor);

    string getCodigoCompleto();
};

#endif