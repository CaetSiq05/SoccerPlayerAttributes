#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <fstream>
#include <sstream>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstddef>
#include <numeric>
#include <map>
#include <cmath>

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
    
    // Obter todos os jogadores
    vector<Player> getAllPlayers() const {
        vector<Player> players;
        for (int i = 0; i < TABLE_SIZE; ++i) {
            HashNode* entry = table[i];
            while (entry != nullptr) {
                players.push_back(entry->player);
                entry = entry->next;
            }
        }
        return players;
    }
    
    // 1. Cálculo estatístico sobre os dados
    void calculateStatistics() {
        auto players = getAllPlayers();
        if (players.empty()) {
            cout << "Nenhum jogador cadastrado para análise.\n";
            return;
        }
        
        // Cálculo de médias
        float total_rating = 0, total_potential = 0;
        for (const auto& p : players) {
            total_rating += p.overall_rating;
            total_potential += p.potential;
        }
        float avg_rating = total_rating / players.size();
        float avg_potential = total_potential / players.size();
        
        // Cálculo de desvios padrão
        float rating_sq_sum = 0, potential_sq_sum = 0;
        for (const auto& p : players) {
            rating_sq_sum += pow(p.overall_rating - avg_rating, 2);
            potential_sq_sum += pow(p.potential - avg_potential, 2);
        }
        float rating_stddev = sqrt(rating_sq_sum / players.size());
        float potential_stddev = sqrt(potential_sq_sum / players.size());
        
        // Distribuição por pé preferido
        map<string, int> foot_dist;
        
        for (const auto& p : players) {
            foot_dist[p.preferred_foot]++;
        }
        
        // Exibir resultados
        cout << "\n=== Estatísticas dos Jogadores ===\n";
        cout << "Total de jogadores: " << players.size() << "\n";
        cout << fixed << setprecision(2);
        cout << "Média de Overall Rating: " << avg_rating << "\n";
        cout << "Desvio padrão do Rating: " << rating_stddev << "\n";
        cout << "Média de Potential: " << avg_potential << "\n";
        cout << "Desvio padrão do Potential: " << potential_stddev << "\n";
        
        cout << "\nDistribuição por Pé Preferido:\n";
        for (const auto& pair : foot_dist) {
            cout << pair.first << ": " << pair.second << " (" 
                 << fixed << setprecision(1) 
                 << (100.0 * pair.second / players.size()) << "%)\n";
        }
    }
    
    // 2. Classificação ou agrupamento de elementos
    void groupAndClassify() {
        auto players = getAllPlayers();
        if (players.empty()) {
            cout << "Nenhum jogador cadastrado para classificação.\n";
            return;
        }
        
        // Agrupar por pé preferido
        map<string, vector<Player>> foot_groups;
        
        for (const auto& p : players) {
            foot_groups[p.preferred_foot].push_back(p);
        }
        
        cout << "\n=== Classificação por Pé Preferido ===\n";
        for (const auto& group : foot_groups) {
            cout << "\nPé: " << group.first << " (" 
                 << group.second.size() << " jogadores)\n";
            
            // Encontrar o melhor rating no grupo
            auto max_rating = max_element(group.second.begin(), group.second.end(),
                [](const Player& a, const Player& b) {
                    return a.overall_rating < b.overall_rating;
                });
            
            cout << "Melhor rating no grupo: " << max_rating->overall_rating 
                 << " (ID: " << max_rating->player_fifa_api_id << ")\n";
            
            // Encontrar o maior potencial no grupo
            auto max_potential = max_element(group.second.begin(), group.second.end(),
                [](const Player& a, const Player& b) {
                    return a.potential < b.potential;
                });
            
            cout << "Maior potencial no grupo: " << max_potential->potential 
                 << " (ID: " << max_potential->player_fifa_api_id << ")\n";
        }
    }
    
    // 3. Filtragem e ordenação dos dados
    void filterAndSort(float min_rating, float max_rating, 
                       float min_potential, float max_potential,
                       const string& foot_preference = "") {
        auto players = getAllPlayers();
        if (players.empty()) {
            cout << "Nenhum jogador cadastrado para filtragem.\n";
            return;
        }
        
        // Filtrar jogadores
        vector<Player> filtered;
        copy_if(players.begin(), players.end(), back_inserter(filtered),
            [min_rating, max_rating, min_potential, max_potential, foot_preference](const Player& p) {
                bool matches = true;
                matches &= p.overall_rating >= min_rating && p.overall_rating <= max_rating;
                matches &= p.potential >= min_potential && p.potential <= max_potential;
                if (!foot_preference.empty()) {
                    matches &= p.preferred_foot == foot_preference;
                }
                return matches;
            });
        
        if (filtered.empty()) {
            cout << "Nenhum jogador encontrado com os critérios especificados.\n";
            return;
        }
        
        // Ordenar por rating (decrescente)
        sort(filtered.begin(), filtered.end(), 
            [](const Player& a, const Player& b) {
                return a.overall_rating > b.overall_rating;
            });
        
        // Exibir resultados
        cout << "\n=== Jogadores Filtrados e Ordenados ===\n";
        cout << "Critérios:\n";
        cout << "Overall Rating entre: " << min_rating << " e " << max_rating << "\n";
        cout << "Potential entre: " << min_potential << " e " << max_potential << "\n";
        if (!foot_preference.empty()) {
            cout << "Pé preferido: " << foot_preference << "\n";
        }
        cout << "Total encontrado: " << filtered.size() << "\n\n";
        
        // Limitar a exibição para os 10 primeiros
        int display_limit = min(10, (int)filtered.size());
        for (int i = 0; i < display_limit; i++) {
            cout << "#" << i+1 << " ";
            filtered[i].display();
            cout << "-----------------\n";
        }
        
        if (filtered.size() > display_limit) {
            cout << "... e mais " << (filtered.size() - display_limit) << " jogadores\n";
        }
    }
    
    // Método para calcular estatísticas detalhadas de colisões
    void collisionStats() const {
        int totalPlayers = 0;
        int collisions = 0;
        int maxChainLength = 0;
        int emptyBuckets = 0;
        vector<int> chainLengths(TABLE_SIZE, 0);
        
        for (int i = 0; i < TABLE_SIZE; ++i) {
            int chainLength = 0;
            HashNode* entry = table[i];
            
            if (entry == nullptr) {
                emptyBuckets++;
                continue;
            }
            
            while (entry != nullptr) {
                chainLength++;
                entry = entry->next;
            }
            
            chainLengths[i] = chainLength;
            totalPlayers += chainLength;
            
            if (chainLength > maxChainLength) {
                maxChainLength = chainLength;
            }
            
            // Cada elemento adicional na cadeia conta como colisão
            if (chainLength > 1) {
                collisions += (chainLength - 1);
            }
        }
        
        double collisionRate = (totalPlayers > 0) ? (double)collisions / totalPlayers : 0.0;
        double loadFactor = (double)totalPlayers / TABLE_SIZE;
        
        cout << "\nEstatísticas Detalhadas de Colisões:\n";
        cout << "====================================\n";
        cout << "Total de jogadores: " << totalPlayers << endl;
        cout << "Total de colisões: " << collisions << endl;
        cout << "Taxa de colisão: " << fixed << setprecision(2) << (collisionRate * 100) << "%\n";
        cout << "Fator de carga: " << loadFactor << endl;
        cout << "Buckets vazios: " << emptyBuckets << " (" 
             << fixed << setprecision(1) << (emptyBuckets * 100.0 / TABLE_SIZE) << "%)\n";
        cout << "Comprimento máximo da cadeia: " << maxChainLength << endl;
        cout << "Comprimento médio das cadeias não vazias: " 
             << (double)(totalPlayers) / (TABLE_SIZE - emptyBuckets) << endl;
        
        // Histograma de comprimentos de cadeia
        cout << "\nHistograma de Comprimentos de Cadeia:\n";
        int max_to_show = min(maxChainLength, 10);
        for (int i = 1; i <= max_to_show; ++i) {
            int count = count_if(chainLengths.begin(), chainLengths.end(), 
                               [i](int len) { return len == i; });
            cout << "Cadeias com " << i << " elemento(s): " << count << endl;
        }
        if (maxChainLength > max_to_show) {
            cout << "Cadeias com mais de " << max_to_show << " elementos: " 
                 << count_if(chainLengths.begin(), chainLengths.end(), 
                            [max_to_show](int len) { return len > max_to_show; }) << endl;
        }
    }
    
    // Método para benchmark de desempenho
    void performanceBenchmark() {
        const vector<int> sizes = {1000, 5000, 10000, 20000, 50000};
        
        cout << "\nBenchmark de Escalabilidade:\n";
        cout << "============================\n";
        
        for (int TEST_SIZE : sizes) {
            vector<int> testKeys;
            clear();
            
            // Gerar chaves únicas
            for (int i = 0; i < TEST_SIZE; ++i) {
                testKeys.push_back(rand() % (TEST_SIZE * 10));
            }

            // Inserção
            auto start = chrono::high_resolution_clock::now();
            for (int key : testKeys) {
                Player p(key, 70.0f, 80.0f, "right", "medium", "medium");
                insert(key, p);
            }
            auto end = chrono::high_resolution_clock::now();
            auto insert_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();

            // Busca
            start = chrono::high_resolution_clock::now();
            for (int key : testKeys) {
                search(key);
            }
            end = chrono::high_resolution_clock::now();
            auto search_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();

            // Estimativa de memória
            size_t totalMemory = TEST_SIZE * (sizeof(Player) + sizeof(HashNode));

            // Teste de tempo médio de acesso a elementos aleatórios já inseridos
            start = chrono::high_resolution_clock::now();
            for (int i = 0; i < TEST_SIZE; ++i) {
                int idx = rand() % TEST_SIZE;
                search(testKeys[idx]);
            }
            end = chrono::high_resolution_clock::now();
            auto access_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();

            // Medida de latência média (operação combinada)
            start = chrono::high_resolution_clock::now();
            for (int i = 0; i < TEST_SIZE; ++i) {
                Player p(testKeys[i], 70.0f, 80.0f, "right", "medium", "medium");
                insert(testKeys[i], p);
                search(testKeys[i]);
                remove(testKeys[i]);
            }
            end = chrono::high_resolution_clock::now();
            auto latency_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();

            // Remoção
            start = chrono::high_resolution_clock::now();
            for (int key : testKeys) {
                remove(key);
            }
            end = chrono::high_resolution_clock::now();
            auto removal_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
            
            // Limpar a tabela após o teste
            clear();
            
            cout << "\nTamanho do teste: " << TEST_SIZE << " elementos\n";
            cout << "Tempo total para inserir: " << insert_time << " ms\n";
            cout << "Tempo médio por inserção: " 
                 << fixed << setprecision(6) << (insert_time * 1000.0 / TEST_SIZE) << " μs\n";
            cout << "Tempo total para buscar: " << search_time << " ms\n";
            cout << "Tempo médio por busca: " 
                 << fixed << setprecision(6) << (search_time * 1000.0 / TEST_SIZE) << " μs\n";
            cout << "Tempo total para remover: " << (removal_time / 1000) << " ms\n";
            cout << "Tempo médio de remoção: "
                 << fixed << setprecision(6) << (removal_time * 1.0 / TEST_SIZE) << " μs\n";
            cout << "Uso estimado de memória: " << totalMemory / 1024.0 << " KB\n";
            cout << "Tempo médio de acesso (busca aleatória): " 
                 << fixed << setprecision(6) << (access_time * 1000.0 / TEST_SIZE) << " μs\n";
            cout << "Latência média (inserção + busca + remoção): "
                 << fixed << setprecision(6) << (latency_time * 1000.0 / TEST_SIZE) << " μs\n";
        }
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
        cout << "6. Exibir estatísticas de colisões\n";
        cout << "7. Executar benchmark de desempenho\n";
        cout << "8. Calcular estatísticas dos jogadores\n";
        cout << "9. Classificar/Agrupar jogadores\n";
        cout << "10. Filtrar e ordenar jogadores\n";
        cout << "0. Sair\n";
        cout << "==============================\n";
        
        int choice;
        cout << "Escolha uma opção: ";
        cin >> choice;
        cin.ignore();
        
        switch (choice) {
            case 1: { // Inserir novo jogador
                cout << "\n--- Inserir Novo Jogador ---\n";
                int id;
                float rating, potential;
                string foot, att_rate, def_rate;
                
                cout << "ID do jogador: ";
                cin >> id;
                cin.ignore();
                
                cout << "Overall Rating: ";
                cin >> rating;
                cin.ignore();
                
                cout << "Potential: ";
                cin >> potential;
                cin.ignore();
                
                cout << "Preferred Foot (left/right): ";
                getline(cin, foot);
                
                cout << "Attacking Work Rate: ";
                getline(cin, att_rate);
                
                cout << "Defensive Work Rate: ";
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
                int id;
                cout << "Digite o ID do jogador: ";
                cin >> id;
                cin.ignore();
                
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
                int id;
                cout << "Digite o ID do jogador a ser removido: ";
                cin >> id;
                cin.ignore();
                
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
            case 6: { // Estatísticas de colisões
                playerTable.collisionStats();
                break;
            }
            case 7: { // Benchmark de desempenho
                playerTable.performanceBenchmark();
                playerTable.loadFromCSV("dataset_limpo3.csv");
                break;
            }
            case 8: { // Cálculo estatístico
                playerTable.calculateStatistics();
                break;
            }
            case 9: { // Classificação/Agrupamento
                playerTable.groupAndClassify();
                break;
            }
            case 10: { // Filtragem e ordenação
                cout << "\n--- Filtrar e Ordenar Jogadores ---\n";
                float min_rating, max_rating, min_potential, max_potential;
                string foot_pref;
                
                cout << "Overall Rating mínimo: ";
                cin >> min_rating;
                cin.ignore();
                
                cout << "Overall Rating máximo: ";
                cin >> max_rating;
                cin.ignore();
                
                cout << "Potential mínimo: ";
                cin >> min_potential;
                cin.ignore();
                
                cout << "Potential máximo: ";
                cin >> max_potential;
                cin.ignore();
                
                cout << "Pé preferido (deixe em branco para qualquer): ";
                getline(cin, foot_pref);
                
                playerTable.filterAndSort(min_rating, max_rating, 
                                         min_potential, max_potential, 
                                         foot_pref);
                break;
            }
            case 0: { // Sair
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