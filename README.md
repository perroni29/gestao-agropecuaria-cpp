# Sistema de Gestão de Agropecuária (C++)

Este projeto consiste em uma aplicação de console desenvolvida em **C++** integrada ao banco de dados **SQLite** para o gerenciamento técnico de animais em uma fazenda (suínos e bovinos). O sistema foi projetado para facilitar o controle de peso, consumo de ração e análise de eficiência alimentar.

## 🚀 Funcionalidades

* **Cadastro de Animais**: Registro de novos animais com validação de dados e persistência direta no banco de dados.
* **Monitoramento Diário**: Registro de consumo de ração e pesagens periódicas para acompanhamento de ganho de peso.
* **Relatórios de Desempenho**: Geração de relatórios automáticos calculando a **Conversão Alimentar** (eficiência entre o que o animal consome e o quanto ele ganha de peso).
* **Persistência de Dados**: Uso de SQLite para garantir que as informações sejam armazenadas de forma segura e permanente.

## 🛠️ Tecnologias Utilizadas

* **Linguagem**: C++ (Foco em programação modularizada).
* **Banco de Dados**: SQLite3.
* **Versionamento**: Git e GitHub.

## 📂 Estrutura do Projeto

* `main.cpp`: Ponto de entrada da aplicação e controle do fluxo do menu.
* `Fazenda.cpp`: Implementação da lógica de negócios e comandos SQL.
* `Fazenda.h`: Definição das estruturas de dados e protótipos das funções.

## ⚙️ Como Compilar e Executar

Para compilar o projeto, é necessário ter o compilador `g++` e a biblioteca `sqlite3` instalada:

```bash
# Compilação vinculando a biblioteca SQLite
g++ main.cpp Fazenda.cpp -o gestao_fazenda -lsqlite3

# Execução
./gestao_fazenda
