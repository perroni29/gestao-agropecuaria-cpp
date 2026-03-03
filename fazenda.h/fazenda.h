#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h>

using namespace std;

// Estrutura para a lista geral de animais
struct Animal {
    int id;
    string nome;
    string tipo;
};

// Estrutura para o relatório de peso
struct RegistroPeso {
    string data;
    double peso;
};

// Estrutura para o relatório de consumo
struct RegistroAlimentacao {
    string data;
    double alimentacao;
};

class DatabaseManager {
private:
    sqlite3* db;

public:
    DatabaseManager(string nomeBanco);
    ~DatabaseManager();

    // Métodos solicitados pelo seu novo main.cpp
    string cadastrarAnimal(string nome, string tipo);
    bool registrarDadosDiarios(int id, double peso, double alimentacao);
    vector<Animal> listarAnimais();
    vector<RegistroPeso> getEvolucaoPeso(int id);
    vector<RegistroAlimentacao> getRegistrosUltimoMes(int id);
    bool excluirAnimal(int id);
};

#endif
