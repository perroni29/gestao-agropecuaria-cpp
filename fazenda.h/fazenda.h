#ifndef FAZENDA_H
#define FAZENDA_H

#include <iostream>
#include <string>
#include <sqlite3.h> // Biblioteca para persistência em banco de dados

using namespace std;

// Estrutura para representar os dados do animal
struct Animal {
    int id;
    string nome;
    string tipo;           // Ex: Bovino ou Suíno
    double peso_inicial;   
    double consumo_total;  // Soma da ração consumida
};

// Protótipos das funções do sistema
void inicializarBanco(sqlite3* &db);
void cadastrarAnimal(sqlite3* db);
void gerarRelatorioConversao(sqlite3* db);
bool validarNome(string nome); 

#endif