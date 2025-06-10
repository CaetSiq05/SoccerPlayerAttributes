#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <fstream>
#include <sstream>
#include <limits>
#include <iomanip>

using namespace std;

// Estrutura para armazenar os dados de um jogador
struct Player {
    int player_fifa_api_id;
    float overall_rating;
    float potential;
    string preferred_foot;
    string attacking_work_rate;
    string defensive_work_rate;
    
    // Construtor padrão
    Player() : player_fifa_api_id(0), overall_rating(0), potential(0) {}
    
    // Construtor com parâmetros
    Player(int id, float rating, float pot, string foot, string att, string def)
        : player_fifa_api_id(id), overall_rating(rating), potential(pot),
          preferred_foot(foot), attacking_work_rate(att), defensive_work_rate(def) {}
    
    // Método para exibir informações do jogador
    void display() const {
        cout << "\nInformações do Jogador:\n";
        cout << "ID: " << player_fifa_api_id << endl;
        cout << "Overall Rating: " << overall_rating << endl;
        cout << "Potential: " << potential << endl;
        cout << "Preferred Foot: " << preferred_foot << endl;
        cout << "Attacking Work Rate: " << attacking_work_rate << endl;
        cout << "Defensive Work Rate: " << defensive_work_rate << endl;
    }
};

// Classe da Tabela Hash
class HashTable {
private:
    static const int TABLE_SIZE = 1000; // Tamanho da tabela
    
    // Estrutura para cada entrada na tabela hash
    struct HashNode {
        int key;
        Player player;
        HashNode* next;
        
        HashNode(int k, const Player& p) : key(k), player(p), next(nullptr) {}
    };
    
    vector<HashNode*> table;
    
    // Função de hash simples
    int hashFunction(int key) {
        return key % TABLE_SIZE;
    }
    
public:
    HashTable() {
        table.resize(TABLE_SIZE, nullptr);
    }
    
    ~HashTable() {
        clear();
    }
    
    // Limpar a tabela hash
    void clear() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            HashNode* entry = table[i];
            while (entry != nullptr) {
                HashNode* prev = entry;
                entry = entry->next;
                delete prev;
            }
            table[i] = nullptr;
        }
    }
    
    // Inserir um jogador na tabela hash
    bool insert(int key, const Player& player) {
        int hashValue = hashFunction(key);
        HashNode* prev = nullptr;
        HashNode* entry = table[hashValue];
        
        while (entry != nullptr && entry->key != key) {
            prev = entry;
            entry = entry->next;
        }
        
        if (entry == nullptr) {
            entry = new HashNode(key, player);
            if (prev == nullptr) {
                table[hashValue] = entry;
            } else {
                prev->next = entry;
            }
            return true;
        } else {
            return false; // Jogador já existe
        }
    }
    
    // Buscar um jogador pelo ID
    Player* search(int key) {
        int hashValue = hashFunction(key);
        HashNode* entry = table[hashValue];
        
        while (entry != nullptr) {
            if (entry->key == key) {
                return &entry->player;
            }
            entry = entry->next;
        }
        
        return nullptr; // Não encontrado
    }
    
    // Remover um jogador
    bool remove(int key) {
        int hashValue = hashFunction(key);
        HashNode* prev = nullptr;
        HashNode* entry = table[hashValue];
        
        while (entry != nullptr && entry->key != key) {
            prev = entry;
            entry = entry->next;
        }
        
        if (entry == nullptr) {
            return false; // Não encontrado
        } else {
            if (prev == nullptr) {
                table[hashValue] = entry->next;
            } else {
                prev->next = entry->next;
            }
            delete entry;
            return true;
        }
    }
    
    // Carregar dados do arquivo CSV
    void loadFromCSV(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Erro ao abrir o arquivo: " << filename << endl;
            return;
        }
        
        string line;
        getline(file, line); // Pular cabeçalho
        
        int count = 0;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() < 5) continue; // Linha inválida
            
            try {
                int id = stoi(tokens[0]);
                float rating = tokens[1].empty() ? 0 : stof(tokens[1]);
                float potential = tokens[2].empty() ? 0 : stof(tokens[2]);
                string foot = tokens[3];
                string att_rate = tokens[4];
                string def_rate = tokens.size() > 5 ? tokens[5] : "";
                
                Player player(id, rating, potential, foot, att_rate, def_rate);
                if (insert(id, player)) {
                    count++;
                }
            } catch (const exception& e) {
                cerr << "Erro ao processar linha: " << line << endl;
                cerr << "Erro: " << e.what() << endl;
            }
        }
        
        file.close();
        cout << "Carregados " << count << " jogadores do arquivo." << endl;
    }
    
    // Exibir todos os jogadores (para debug)
    void displayAll() const {
        cout << "\nLista de Todos os Jogadores:\n";
        for (int i = 0; i < TABLE_SIZE; ++i) {
            HashNode* entry = table[i];
            while (entry != nullptr) {
                entry->player.display();
                entry = entry->next;
            }
        }
    }
    
    // Exibir estatísticas da tabela hash
    void displayStats() const {
        int totalPlayers = 0;
        int maxChainLength = 0;
        int emptyBuckets = 0;
        
        for (int i = 0; i < TABLE_SIZE; ++i) {
            int chainLength = 0;
            HashNode* entry = table[i];
            
            if (entry == nullptr) {
                emptyBuckets++;
                continue;
            }
            
            while (entry != nullptr) {
                chainLength++;
                totalPlayers++;
                entry = entry->next;
            }
            
            if (chainLength > maxChainLength) {
                maxChainLength = chainLength;
            }
        }
        
        cout << "\nEstatísticas da Tabela Hash:\n";
        cout << "Total de jogadores: " << totalPlayers << endl;
        cout << "Tamanho da tabela: " << TABLE_SIZE << endl;
        cout << "Buckets vazios: " << emptyBuckets << " (" 
             << fixed << setprecision(1) << (emptyBuckets * 100.0 / TABLE_SIZE) << "%)" << endl;
        cout << "Comprimento máximo da cadeia: " << maxChainLength << endl;
        cout << "Fator de carga: " << (totalPlayers * 1.0 / TABLE_SIZE) << endl;
    }
};

// Função para limpar o buffer de entrada
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Função para ler um número inteiro com validação
int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cout << "Entrada inválida. Por favor, digite um número.\n";
            clearInputBuffer();
        } else {
            clearInputBuffer();
            return value;
        }
    }
}

// Função para ler um número float com validação
float readFloat(const string& prompt) {
    float value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cout << "Entrada inválida. Por favor, digite um número.\n";
            clearInputBuffer();
        } else {
            clearInputBuffer();
            return value;
        }
    }
}

// Função para exibir o menu e processar a escolha do usuário
void showMenu(HashTable& playerTable) {
    while (true) {
        cout << "\n===== MENU DA TABELA HASH =====\n";
        cout << "1. Inserir novo jogador\n";
        cout << "2. Buscar jogador por ID\n";
        cout << "3. Remover jogador por ID\n";
        cout << "4. Carregar dados do arquivo CSV\n";
        cout << "5. Exibir estatísticas da tabela\n";
        cout << "6. Sair\n";
        cout << "==============================\n";
        
        int choice = readInt("Escolha uma opção: ");
        
        switch (choice) {
            case 1: { // Inserir novo jogador
                cout << "\n--- Inserir Novo Jogador ---\n";
                int id = readInt("ID do jogador: ");
                float rating = readFloat("Overall Rating: ");
                float potential = readFloat("Potential: ");
                
                cout << "Preferred Foot (left/right): ";
                string foot;
                getline(cin, foot);
                
                cout << "Attacking Work Rate: ";
                string att_rate;
                getline(cin, att_rate);
                
                cout << "Defensive Work Rate: ";
                string def_rate;
                getline(cin, def_rate);
                
                Player newPlayer(id, rating, potential, foot, att_rate, def_rate);
                if (playerTable.insert(id, newPlayer)) {
                    cout << "Jogador inserido com sucesso!\n";
                } else {
                    cout << "Erro: Já existe um jogador com este ID.\n";
                }
                break;
            }
            case 2: { // Buscar jogador
                cout << "\n--- Buscar Jogador ---\n";
                int id = readInt("Digite o ID do jogador: ");
                Player* player = playerTable.search(id);
                if (player != nullptr) {
                    player->display();
                } else {
                    cout << "Jogador com ID " << id << " não encontrado.\n";
                }
                break;
            }
            case 3: { // Remover jogador
                cout << "\n--- Remover Jogador ---\n";
                int id = readInt("Digite o ID do jogador a ser removido: ");
                if (playerTable.remove(id)) {
                    cout << "Jogador removido com sucesso.\n";
                } else {
                    cout << "Jogador com ID " << id << " não encontrado.\n";
                }
                break;
            }
            case 4: { // Carregar do CSV
                cout << "\n--- Carregar Dados do CSV ---\n";
                playerTable.clear();
                playerTable.loadFromCSV("dataset_limpo3.csv");
                break;
            }
            case 5: { // Exibir estatísticas
                playerTable.displayStats();
                break;
            }
            case 6: { // Sair
                cout << "Saindo do programa...\n";
                return;
            }
            default: {
                cout << "Opção inválida. Por favor, tente novamente.\n";
                break;
            }
        }
    }
}

int main() {
    HashTable playerTable;
    
    // Carregar dados do arquivo CSV automaticamente no início
    playerTable.loadFromCSV("dataset_limpo3.csv");
    
    // Mostrar menu interativo
    showMenu(playerTable);
    
    return 0;
}