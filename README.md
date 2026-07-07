# Trabalho Final - Compiladores

Compilador com interface gráfica desenvolvido em C++/Qt, contendo as etapas de análise léxica, sintática e semântica, além de geração de código assembly.

## Autores

- David Smith Souza
- Lucas Rodrigues Pinheiro
- Samuel Cesário Pereira Trizotto de Andrade

## Funcionalidades

- Editor de texto para digitar o código-fonte a ser compilado
- Análise léxica, sintática e semântica com relato de erros específicos de cada etapa
- Tabela de símbolos (nome, tipo, categoria, escopo, inicialização e uso)
- Geração de código assembly a partir do código analisado

## Requisitos

- [Qt Creator](https://www.qt.io/download-qt-installer) com Qt 6 (o projeto foi desenvolvido com o kit **Desktop Qt 6.11.0 MinGW 64-bit**)
- Compilador compatível com C++17 (MinGW, incluso na instalação do Qt)

## Como rodar

### Pelo Qt Creator (recomendado)

1. Abra o Qt Creator.
2. Abra o arquivo [`Tete.pro`](Tete.pro) (`Arquivo > Abrir Arquivo ou Projeto...`).
3. Selecione o kit **Desktop Qt 6.11.0 MinGW 64-bit** (ou outro kit Qt 6 disponível).
4. Clique no botão **Compilar/Executar** (ícone de play verde no canto inferior esquerdo) ou use o atalho **Ctrl+R**.
5. Com a aplicação aberta, digite um código-fonte no campo de entrada e clique em **Compilar** para ver o resultado da análise e o código assembly gerado.

### Via linha de comando (alternativa)

```bash
qmake Tete.pro
mingw32-make      # ou make, no Linux/macOS
```

O executável gerado ficará na pasta de build (`build/.../release` ou `debug`, dependendo da configuração).
