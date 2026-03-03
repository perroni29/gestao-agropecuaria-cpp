#include "Fazenda.h"

int main() {
    sqlite3* db;
    inicializarBanco(db); // Conecta ao banco de dados
    
    int opcao;
    do {
        cout << "\n--- GESTAO DE AGROPECUARIA ---" << endl;
        cout << "1. Cadastrar Novo Animal" << endl;
        cout << "2. Ver Relatorio de Conversao" << endl;
        cout << "0. Sair" << endl;
        cout << "Escolha: ";
        cin >> opcao;
        
        if (opcao == 1) cadastrarAnimal(db);
        else if (opcao == 2) gerarRelatorioConversao(db);
        
    } while (opcao != 0); 

    sqlite3_close(db); 
    return 0;
}