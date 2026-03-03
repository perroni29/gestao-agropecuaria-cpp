#include <iostream>
#include <string>
#include <vector>
#include <limits> 
#include <stdlib.h> 
#include <algorithm> 
#include <stdexcept> 
#include <iomanip> 
#include <cctype> 
#include "fazenda.h" // Garanta que o arquivo .h tenha este nome exato

// --- Funções Auxiliares de Conversão e Interface (Mantidas do seu código) ---

double stringParaDouble(const std::string& str) {
    std::string temp = str;
    size_t pos = temp.find(',');
    if (pos != std::string::npos) {
        temp.replace(pos, 1, ".");
    }
    try {
        if (temp.empty()) return -1.0;
        size_t charsProcessados = 0;
        double valor = std::stod(temp, &charsProcessados);
        if (charsProcessados < temp.length()) {
             bool apenasEspacos = true;
             for(size_t i = charsProcessados; i < temp.length(); ++i) {
                 if (!std::isspace(static_cast<unsigned char>(temp[i]))) {
                     apenasEspacos = false;
                     break;
                 }
             }
             if (!apenasEspacos) return -1.0;
        }
        if (valor < 0.0) return -1.0;
        return valor;
    } catch (...) {
        return -1.0; 
    }
}

void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pausar() {
    std::cout << "\nPressione Enter para continuar...";
    std::cin.clear();
    if (std::cin.peek() == '\n') std::cin.ignore();
    std::string dummy;
    std::getline(std::cin, dummy);
}

// --- Lógica de Seleção de Animal (Essencial para os Relatórios) ---

int selecionarAnimal(DatabaseManager& db, std::string* nomeAnimalEscolhido = nullptr) {
    std::vector<Animal> animais = db.listarAnimais(); // Busca a lista atualizada do SQLite
    if (animais.empty()) {
        std::cout << "Nenhum animal cadastrado.\n";
        return -1;
    }

    std::cout << "\n--- Lista de Animais ---\n";
    for (const auto& animal : animais) {
        std::cout << "ID: " << animal.id << " | Nome: " << animal.nome << " | Tipo: " << animal.tipo << "\n";
    }

    std::string linhaInput;
    std::cout << "Digite o ID do animal (ou 0 para cancelar): ";
    std::getline(std::cin, linhaInput);
    
    try {
        int id = std::stoi(linhaInput);
        if (id == 0) return -1;
        for (const auto& a : animais) {
            if (a.id == id) {
                if (nomeAnimalEscolhido) *nomeAnimalEscolhido = a.nome;
                return id;
            }
        }
    } catch (...) {}
    
    std::cout << "ID invalido.\n";
    return -1;
}

// --- Funções de Interface vinculadas ao DatabaseManager ---

void cadastrarAnimalUI(DatabaseManager& db) {
    std::string nome, tipo;
    std::cout << "Nome: "; std::getline(std::cin, nome);
    std::cout << "Tipo: "; std::getline(std::cin, tipo);
    if (nome.empty() || tipo.empty()) return;
    std::cout << db.cadastrarAnimal(nome, tipo) << std::endl;
}

void registrarDadosUI(DatabaseManager& db) {
    int id = selecionarAnimal(db);
    if (id == -1) return;
    
    std::string sPeso, sAlim;
    std::cout << "Peso hoje (kg): "; std::getline(std::cin, sPeso);
    std::cout << "Alimentacao hoje (kg): "; std::getline(std::cin, sAlim);
    
    double p = stringParaDouble(sPeso);
    double a = stringParaDouble(sAlim);
    
    if (p >= 0 && a >= 0) {
        if (db.registrarDadosDiarios(id, p, a)) std::cout << "Sucesso!\n";
    } else {
        std::cout << "Valores invalidos.\n";
    }
}

// --- Main: O coração do seu programa ---

int main() {
    DatabaseManager db("fazenda.db"); // Cria/Abre o arquivo de banco de dados
    std::string linhaEscolha;
    int escolha = -1;

    do {
        limparTela();
        std::cout << "\n--- Controle de Gado e Suinos --- \n";
        std::cout << "1. Cadastrar Novo Animal\n2. Registrar Dados Diarios\n3. Relatorio Mensal\n4. Evolucao de Peso\n5. Listar Animais\n6. Excluir Animal\n0. Sair\n";
        std::cout << "Escolha: ";
        std::getline(std::cin, linhaEscolha);

        try { escolha = std::stoi(linhaEscolha); } catch (...) { escolha = -1; }

        switch (escolha) {
            case 1: cadastrarAnimalUI(db); break;
            case 2: registrarDadosUI(db); break;
            case 5: selecionarAnimal(db); break;
            case 0: std::cout << "Saindo...\n"; break;
            default: std::cout << "Opcao invalida.\n"; break;
        }
        if (escolha != 0) pausar();
    } while (escolha != 0);

    return 0;
}
