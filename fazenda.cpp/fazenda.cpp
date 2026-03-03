#include "Fazenda.h"

// Inicializa o banco de dados SQLite
void inicializarBanco(sqlite3* &db) {
    if (sqlite3_open("fazenda.db", &db)) {
        cerr << "Erro ao abrir banco de dados!" << endl;
    } else {
        // SQL para criação da tabela de animais
        string sql = "CREATE TABLE IF NOT EXISTS animais ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "nome TEXT NOT NULL,"
                     "tipo TEXT,"
                     "peso_inicial REAL,"
                     "consumo_total REAL DEFAULT 0);";
        sqlite3_exec(db, sql.c_str(), 0, 0, 0);
    }
}

// Lógica de cadastro com persistência
void cadastrarAnimal(sqlite3* db) {
    string nome, tipo;
    double peso;
    
    cout << "Nome do animal: ";
    cin >> nome;
    
    if (!validarNome(nome)) {
        cout << "Nome invalido!" << endl;
        return;
    }
    
    cout << "Tipo (Bovino/Suino): ";
    cin >> tipo;
    cout << "Peso Inicial (kg): ";
    cin >> peso;

    string sql = "INSERT INTO animais (nome, tipo, peso_inicial) VALUES ('" 
                 + nome + "', '" + tipo + "', " + to_string(peso) + ");";
    
    if (sqlite3_exec(db, sql.c_str(), 0, 0, 0) == SQLITE_OK)
        cout << "Animal cadastrado com sucesso!" << endl;
}

bool validarNome(string nome) {
    return !nome.empty() && nome.length() < 50;
}

void gerarRelatorioConversao(sqlite3* db) {
    cout << "\n--- Relatorio de Eficiencia Alimentar ---" << endl;
    // Cálculo: (Peso Atual - Peso Inicial) / Consumo Total
}