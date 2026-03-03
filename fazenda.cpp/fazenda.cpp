#include <iostream>
#include <string>
#include <vector>
#include <limits> // Para limpar o buffer de entrada
#include <stdlib.h> // Para system("cls") ou system("clear")
#include <algorithm> // Para std::replace em stringParaDouble
#include <stdexcept> // Para std::stod e exceções
#include <iomanip> // Para formatação da saída
#include <cctype> // Para std::isspace, std::tolower
#include "fazenda.h"
#include "sqlite3.h"

// --- Função Auxiliar para Conversão ---
// Converte uma string (com ponto ou vírgula) para double
// Retorna -1.0 em caso de erro.
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

        // Verifica se houve caracteres não numéricos após o número, ignorando espaços
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

        // Permite 0.0, mas não números negativos explicitamente
        if (valor < 0.0) return -1.0;
        return valor;
    } catch (const std::invalid_argument&) {
        return -1.0; // String não começa com número válido
    } catch (const std::out_of_range&) {
        return -1.0; // Número muito grande/pequeno
    }
}


// --- Funções Auxiliares da Interface ---

void limparInput() {
    std::cin.clear(); // Limpa flags de erro
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void pausar() {
    std::cout << "\nPressione Enter para continuar...";
    std::cin.clear();
    std::string dummy;
    // Consome o Enter residual, se houver, antes de esperar o novo Enter
    if (std::cin.peek() == '\n') {
        std::cin.ignore();
    }
    std::getline(std::cin, dummy); // Espera o Enter do usuário
}


void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void mostrarMenu() {
    std::cout << "\n--- Controle de Gado e Suinos --- \n";
    std::cout << "1. Cadastrar Novo Animal\n";
    std::cout << "2. Registrar Dados Diarios (Peso/Alimentacao)\n";
    std::cout << "3. Relatorio: Consumo Mensal Detalhado\n";
    std::cout << "4. Relatorio: Evolucao de Peso\n";
    std::cout << "5. Listar Animais\n";
    std::cout << "6. Excluir Animal\n";
    std::cout << "0. Sair\n";
    std::cout << "Escolha uma opcao: ";
}

int selecionarAnimal(DatabaseManager& db, std::string* nomeAnimalEscolhido = nullptr) {
    std::vector<Animal> animais = db.listarAnimais();
    if (animais.empty()) {
        std::cout << "Nenhum animal cadastrado.\n";
        return -1;
    }

    std::cout << "\n--- Lista de Animais ---\n";
    for (const auto& animal : animais) {
        std::cout << "ID: " << animal.id << " | Nome: " << animal.nome << " | Tipo: " << animal.tipo << "\n";
    }

    int idEscolhido = -1;
    std::string linhaInput;
    while(true) {
        std::cout << "Digite o ID do animal (ou 0 para cancelar): ";
        std::getline(std::cin, linhaInput);

        try {
            idEscolhido = std::stoi(linhaInput);
        } catch (const std::exception&) {
            std::cout << "Entrada invalida. Por favor, digite um numero.\n";
            continue;
        }

        if (idEscolhido == 0) {
            std::cout << "Operacao cancelada.\n";
            return -1;
        }

        bool idValido = false;
        for (const auto& animal : animais) {
            if (animal.id == idEscolhido) {
                idValido = true;
                if (nomeAnimalEscolhido != nullptr) {
                    *nomeAnimalEscolhido = animal.nome;
                }
                break;
            }
        }

        if (idValido) {
            return idEscolhido;
        } else {
            std::cout << "ID nao encontrado. Tente novamente.\n";
        }
    }
}

// --- Funções do Menu ---

void cadastrarAnimalUI(DatabaseManager& db) {
    std::string nome, tipo;
    std::cout << "--- Cadastro de Animal ---\n";
    std::cout << "Nome (ex: Mimosa, Bacon): ";
    std::getline(std::cin, nome);

    if (nome.empty()) {
        std::cout << "Erro: O nome do animal nao pode ser vazio.\n";
        return;
    }

    std::cout << "Tipo (vaca, porco): ";
    std::getline(std::cin, tipo);

    if (tipo.empty()) {
        std::cout << "Erro: O tipo do animal nao pode ser vazio.\n";
        return;
    }

    std::string resultado = db.cadastrarAnimal(nome, tipo);
    std::cout << resultado << std::endl;
}

void registrarDadosUI(DatabaseManager& db) {
    std::cout << "--- Registro Diario ---\n";
    int animalId = selecionarAnimal(db);
    if (animalId == -1) {
        return;
    }

    std::string inputStr;
    double peso = -1.0, alimentacao = -1.0;

    while (true) {
        std::cout << "Digite o peso (kg) de hoje (use virgula ou ponto): ";
        if (!std::getline(std::cin, inputStr)) {
             std::cout << "Erro de leitura. Tente novamente.\n";
             std::cin.clear();
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
             continue;
        }
        peso = stringParaDouble(inputStr);
        if (peso < 0.0) {
            std::cout << "Peso invalido. Digite um numero positivo (ex: 50 ou 50,5).\n";
        } else {
            break;
        }
    }

    while (true) {
        std::cout << "Digite o total de alimentacao (kg) de hoje (use virgula ou ponto): ";
         if (!std::getline(std::cin, inputStr)) {
             std::cout << "Erro de leitura. Tente novamente.\n";
             std::cin.clear();
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
             continue;
        }
        alimentacao = stringParaDouble(inputStr);
        // Permite 0.0 como valor válido, mas não negativos retornados por erro
        if (alimentacao < 0.0) {
            std::cout << "Alimentacao invalida. Digite um numero nao-negativo (ex: 1,2 ou 0.5 ou 0).\n";
        } else {
            break;
        }
    }

    // Código de Debug removido daqui

    if (db.registrarDadosDiarios(animalId, peso, alimentacao)) {
        std::cout << "Dados registrados com sucesso!\n";
    } else {
        std::cout << "Falha ao registrar dados.\n";
    }
}


void relatorioMensalUI(DatabaseManager& db) {
    std::cout << "--- Relatorio de Consumo Mensal Detalhado ---\n";
    int animalId = selecionarAnimal(db);
    if (animalId == -1) {
        return;
    }

    std::vector<RegistroAlimentacao> registros = db.getRegistrosUltimoMes(animalId);
    double consumoTotal = 0.0;

    std::cout << "-------------------------------------\n";
    std::cout << "Consumo diario no ultimo mes (ultimos 30 dias):\n";

    if (registros.empty()) {
        std::cout << "Nenhum registro encontrado no ultimo mes.\n";
    } else {
        // Guarda a formatação original do cout
        std::ios_base::fmtflags flagsOriginals = std::cout.flags();
        std::streamsize precisaoOriginal = std::cout.precision();

        std::cout << std::fixed << std::setprecision(1); // Formata para 1 casa decimal

        for(const auto& reg : registros) {
            std::cout << reg.data << " -> " << reg.alimentacao << " kg\n";
            consumoTotal += reg.alimentacao;
        }

        std::cout << "-------------------------------------\n";
        std::cout << "Consumo TOTAL no ultimo mes: " << consumoTotal << " kg\n";

        // Restaura a formatação original do cout
        std::cout.flags(flagsOriginals);
        std::cout.precision(precisaoOriginal);
    }
    std::cout << "-------------------------------------\n";
}

void evolucaoPesoUI(DatabaseManager& db) {
    std::cout << "--- Relatorio de Evolucao de Peso ---\n";
    int animalId = selecionarAnimal(db);
    if (animalId == -1) {
        return;
    }

    std::vector<RegistroPeso> evolucao = db.getEvolucaoPeso(animalId);

    std::cout << "-------------------------------------\n";
    std::cout << "Evolucao (Data -> Peso):\n";
    if (evolucao.empty()) {
        std::cout << "Nenhum registro de peso encontrado para este animal.\n";
    } else {
        // Guarda formatação original
        std::ios_base::fmtflags flagsOriginals = std::cout.flags();
        std::streamsize precisaoOriginal = std::cout.precision();

        std::cout << std::fixed << std::setprecision(1); // Formata para 1 casa decimal
        for(const auto& reg : evolucao) {
            std::cout << reg.data << " -> " << reg.peso << " kg\n";
        }

        // Restaura formatação original
        std::cout.flags(flagsOriginals);
        std::cout.precision(precisaoOriginal);
    }
    std::cout << "-------------------------------------\n";

    if (evolucao.size() >= 2) {
        double pesoInicial = evolucao.front().peso;
        double pesoFinal = evolucao.back().peso;
        double ganhoDePeso = pesoFinal - pesoInicial;

        double consumoMensal = 0;
        std::vector<RegistroAlimentacao> registrosAlim = db.getRegistrosUltimoMes(animalId);
        for(const auto& reg : registrosAlim){
            consumoMensal += reg.alimentacao;
        }

        std::cout << "\nCalculo de Conversao (Exemplo com dados mensais):\n";

        if (ganhoDePeso > 0 && consumoMensal > 0) {
             // Guarda formatação original
            std::ios_base::fmtflags flagsOriginals = std::cout.flags();
            std::streamsize precisaoOriginal = std::cout.precision();

            std::cout << std::fixed << std::setprecision(2); // Formata para 2 casas decimais
            std::cout << "Conversao Alimentar (baseado no consumo mensal e ganho total): "
                      << (consumoMensal / ganhoDePeso) << " kg de alimento / kg ganho\n";

            // Restaura formatação original
            std::cout.flags(flagsOriginals);
            std::cout.precision(precisaoOriginal);
        } else if (ganhoDePeso <= 0) {
            std::cout << "Nao houve ganho de peso nos registros para calcular conversao.\n";
        } else {
             std::cout << "Nao houve consumo de alimento registrado no periodo para calcular conversao.\n";
        }
    }
}

void excluirAnimalUI(DatabaseManager& db) {
    std::cout << "--- Excluir Animal ---\n";
    std::string nomeAnimal;
    int animalId = selecionarAnimal(db, &nomeAnimal);

    if (animalId == -1) {
        return;
    }

    char confirmacao = 'n';
    std::string linhaConfirm;
    while (true) {
        std::cout << "Tem CERTEZA que deseja excluir o animal '" << nomeAnimal << "' (ID: " << animalId << ")?\n";
        std::cout << "TODOS os registros de peso e alimentacao dele serao PERDIDOS.\n";
        std::cout << "Digite 's' para confirmar ou 'n' para cancelar: ";
        std::getline(std::cin, linhaConfirm);

        if (!linhaConfirm.empty()) {
            confirmacao = std::tolower(static_cast<unsigned char>(linhaConfirm[0]));
        } else {
             confirmacao = 'n';
        }

        if (confirmacao == 's' || confirmacao == 'n') {
            break;
        } else {
            std::cout << "Opcao invalida. Digite 's' ou 'n'.\n";
        }
    }

    if (confirmacao == 's') {
        if (db.excluirAnimal(animalId)) {
            std::cout << "Animal '" << nomeAnimal << "' excluido com sucesso.\n";
        } else {
            std::cout << "Falha ao excluir o animal.\n";
        }
    } else {
        std::cout << "Exclusao cancelada.\n";
    }
}

// --- Função Principal ---

int main() {
    // Locale removido

    DatabaseManager db("fazenda.db");
    int escolha = -1;
    std::string linhaEscolha;

    do {
        limparTela();
        mostrarMenu();

        std::getline(std::cin, linhaEscolha);

        try {
            escolha = std::stoi(linhaEscolha);
        } catch (const std::exception&) {
            escolha = -1; // Define como inválido se não for número
        }

        switch (escolha) {
            case 1:
                cadastrarAnimalUI(db);
                pausar();
                break;
            case 2:
                registrarDadosUI(db);
                pausar();
                break;
            case 3:
                relatorioMensalUI(db);
                pausar();
                break;
            case 4:
                evolucaoPesoUI(db);
                pausar();
                break;
            case 5:
                limparTela();
                std::cout << "--- Listando todos os animais ---\n";
                selecionarAnimal(db);
                pausar();
                break;
            case 6:
                excluirAnimalUI(db);
                pausar();
                break;
            case 0:
                std::cout << "Saindo...\n";
                break;
            default:
                if(linhaEscolha.empty()) {
                     std::cout << "Entrada vazia. Tente novamente.\n";
                } else if (escolha == -1){
                     std::cout << "Opcao invalida. Por favor, digite um numero do menu.\n";
                } else {
                     std::cout << "Opcao invalida. Tente novamente.\n";
                }
                pausar();
                break;
        }

    } while (escolha != 0);

    return 0;
}
