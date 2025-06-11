#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>
#include <memory>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <queue>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#endif

using namespace std;
using namespace std::chrono;

// Função para medir o uso de memória em KB (multi-plataforma)
size_t getMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return pmc.WorkingSetSize / 1024;
#else
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // KB em sistemas Linux/Unix
#endif
}

struct Node {
    int player_id;
    float overall_rating;
    float potential;
    string preferred_foot;
    string attacking_work_rate;
    string defensive_work_rate;
    Node* left;
    Node* right;
    int height;

    Node(int id, float rating, float pot, string foot, string atk_work, string def_work) : 
        player_id(id), overall_rating(rating), potential(pot), preferred_foot(foot), 
        attacking_work_rate(atk_work), defensive_work_rate(def_work), 
        left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
public:
    Node* root;
    bool restricaoAcessoMemoria = false;
    const int LIMITE_ACESSOS = 5; // Restrição R22
    int contadorAcessos = 0;

    AVLTree() {
        root = nullptr;
    }

    int getHeight(Node* node) {
        return node ? node->height : 0;
    }

    int getBalance(Node* node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    Node* rightRotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = 1 + max(getHeight(y->left), getHeight(y->right));
        x->height = 1 + max(getHeight(x->left), getHeight(x->right));

        return x;
    }

    Node* leftRotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = 1 + max(getHeight(x->left), getHeight(x->right));
        y->height = 1 + max(getHeight(y->left), getHeight(y->right));

        return y;
    }

    Node* insert(Node* node, int id, float rating, float pot, string foot, string atk_work, string def_work) {
        if (!node) return new Node(id, rating, pot, foot, atk_work, def_work);

        if (id < node->player_id)
            node->left = insert(node->left, id, rating, pot, foot, atk_work, def_work);
        else if (id > node->player_id)
            node->right = insert(node->right, id, rating, pot, foot, atk_work, def_work);
        else
            return node; // IDs duplicados não são permitidos

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);

        if (balance > 1 && id < node->left->player_id)
            return rightRotate(node);
        if (balance < -1 && id > node->right->player_id)
            return leftRotate(node);
        if (balance > 1 && id > node->left->player_id) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && id < node->right->player_id) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    Node* minValueNode(Node* node) {
        Node* current = node;
        while (current && current->left != nullptr)
            current = current->left;
        return current;
    }

    Node* deleteNode(Node* root, int id) {
        if (!root) return root;

        if (id < root->player_id)
            root->left = deleteNode(root->left, id);
        else if (id > root->player_id)
            root->right = deleteNode(root->right, id);
        else {
            if (!root->left || !root->right) {
                Node* temp = root->left ? root->left : root->right;
                if (!temp) {
                    temp = root;
                    root = nullptr;
                } else {
                    *root = *temp;
                }
                delete temp;
            } else {
                Node* temp = minValueNode(root->right);
                root->player_id = temp->player_id;
                root->overall_rating = temp->overall_rating;
                root->potential = temp->potential;
                root->preferred_foot = temp->preferred_foot;
                root->attacking_work_rate = temp->attacking_work_rate;
                root->defensive_work_rate = temp->defensive_work_rate;
                root->right = deleteNode(root->right, temp->player_id);
            }
        }

        if (!root) return root;

        root->height = 1 + max(getHeight(root->left), getHeight(root->right));
        int balance = getBalance(root);

        if (balance > 1 && getBalance(root->left) >= 0)
            return rightRotate(root);
        if (balance > 1 && getBalance(root->left) < 0) {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }
        if (balance < -1 && getBalance(root->right) <= 0)
            return leftRotate(root);
        if (balance < -1 && getBalance(root->right) > 0) {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }

        return root;
    }

    Node* search(Node* node, int id) {
        if (restricaoAcessoMemoria) {
            contadorAcessos++;
            if (contadorAcessos > LIMITE_ACESSOS) {
                throw runtime_error("Limite de acessos à memória excedido!");
            }
        }
        
        if (!node || node->player_id == id)
            return node;

        if (id < node->player_id)
            return search(node->left, id);

        return search(node->right, id);
    }

    void resetContadorAcessos() {
        contadorAcessos = 0;
    }
};

class GerenciadorJogadoresAVL {
private:
    AVLTree tree;
    unordered_map<int, Node*> hash_table;
    size_t LIMITE_ELEMENTOS = 500;  // Agora não é mais const para poder ser modificado
    queue<int> fila_ids;
    bool limiteAtivo = true;       // Controle para ativar/desativar a restrição
    bool habilitarRestricoesProcessamento = true; // Controle da restrição R10
    bool habilitarLatencia = false;
    mt19937 gerador;
    uniform_int_distribution<int> distribuicaoLatencia{300, 700}; // 300ms a 700ms
    bool habilitarDadosAnomalos = false;
    mt19937 geradorAnomalias;
    bernoulli_distribution distribuicaoAnomalias{0.1}; // 10% de chance

    void removerMaisAntigoSeNecessario() {
        if (limiteAtivo && hash_table.size() > LIMITE_ELEMENTOS) {
            int id_remover = fila_ids.front();
            tree.root = tree.deleteNode(tree.root, id_remover);
            hash_table.erase(id_remover);
            fila_ids.pop();
            cout << "Atenção: Limite de " << LIMITE_ELEMENTOS 
                 << " elementos atingido. Jogador ID " << id_remover 
                 << " foi removido automaticamente.\n";
        }
    }

    void simularLatenciaBancoDados() {
        if (habilitarLatencia) {
            int delay_ms = distribuicaoLatencia(gerador);
            cout << "[Simulando latência: " << delay_ms << "ms]\n";
            this_thread::sleep_for(chrono::milliseconds(delay_ms));
        }
    }

    void simularInterrupcao() {
        if (habilitarRestricoesProcessamento) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    // Função para injetar anomalias nos dados
    void aplicarRuidoDados(float& rating, float& potential) {
        if (habilitarDadosAnomalos && distribuicaoAnomalias(geradorAnomalias)) {
            // Gerar valores extremos
            uniform_real_distribution<float> distRating(0.0f, 100.0f);
            uniform_real_distribution<float> distPotential(0.0f, 100.0f);
            
            rating = distRating(geradorAnomalias);
            potential = distPotential(geradorAnomalias);
            
            cout << "[ALERTA: Dados anômalos injetados! Rating: " << rating 
                 << ", Potential: " << potential << "]\n";
        }
    }

public:
    GerenciadorJogadoresAVL(const string& caminho_csv) : geradorAnomalias(random_device{}()) {
        ifstream file(caminho_csv);
        if (!file.is_open()) {
            cerr << "Erro ao abrir o arquivo.\n";
            exit(1);
        }

        string linha;
        getline(file, linha);  // cabeçalho
        vector<string> colunas;
        stringstream ss_header(linha);
        string col;
        while (getline(ss_header, col, ',')) colunas.push_back(col);

        while (getline(file, linha)) {
            stringstream ss(linha);
            string valor;
            int player_id = 0;
            float rating = 0.0, potential = 0.0;
            string foot, atk_work, def_work;

            for (int i = 0; i < colunas.size(); ++i) {
                getline(ss, valor, ',');
                if (colunas[i] == "player_fifa_api_id") player_id = stoi(valor);
                if (colunas[i] == "overall_rating") rating = stof(valor);
                if (colunas[i] == "potential") potential = stof(valor);
                if (colunas[i] == "preferred_foot") foot = valor;
                if (colunas[i] == "attacking_work_rate") atk_work = valor;
                if (colunas[i] == "defensive_work_rate") def_work = valor;
            }

            tree.root = tree.insert(tree.root, player_id, rating, potential, foot, atk_work, def_work);
            hash_table[player_id] = tree.search(tree.root, player_id);
        }

        file.close();
    }

    void buscarPorId(int id) {
        simularInterrupcao(); // Interrupção antes da operação
        simularLatenciaBancoDados();
        try {
            tree.resetContadorAcessos();
            Node* node = tree.search(tree.root, id);
            if (node) {
                cout << "Jogador encontrado com " << tree.contadorAcessos 
                     << " acessos à memória.\n";
                     cout << "ID: " << node->player_id << "\n";
                     cout << "Overall Rating: " << node->overall_rating << "\n";
                     cout << "Potential: " << node->potential << "\n";
                     cout << "Preferred Foot: " << node->preferred_foot << "\n";
                     cout << "Attacking Work Rate: " << node->attacking_work_rate << "\n";
                     cout << "Defensive Work Rate: " << node->defensive_work_rate << "\n";
            } else {
                cout << "Jogador não encontrado! (Acessos: " 
                     << tree.contadorAcessos << ")\n";
            }
        } catch (const runtime_error& e) {
            cout << "ERRO: " << e.what() << endl;
        }
    }

    void removerJogador(int id) {
        simularInterrupcao(); // Interrupção antes da operação
        simularLatenciaBancoDados();
        if (hash_table.find(id) != hash_table.end()) {
            tree.root = tree.deleteNode(tree.root, id);
            hash_table.erase(id);
            cout << "Jogador removido com sucesso!\n";
        } else {
            cout << "Jogador não encontrado!\n";
        }
    }

    void inserirJogador(int id, float rating, float potential, string foot, 
                       string atk_work, string def_work) {
        float rating_original = rating;
        float potential_original = potential;
        
        aplicarRuidoDados(rating, potential);
        simularInterrupcao(); // Interrupção antes da operação
        simularLatenciaBancoDados();
        // Se for um novo jogador (não apenas atualização)
        if (hash_table.find(id) == hash_table.end()) {
            fila_ids.push(id);  // Adiciona à fila de controle
        }
        
        // Insere ou atualiza o jogador
        tree.root = tree.insert(tree.root, id, rating, potential, foot, atk_work, def_work);
        hash_table[id] = tree.search(tree.root, id);
        
        // Aplica a restrição R2
        removerMaisAntigoSeNecessario();
        cout << "Jogador inserido com sucesso! (Total: " << hash_table.size() 
             << "/" << (limiteAtivo ? to_string(LIMITE_ELEMENTOS) : "∞") << ")\n";
        

        if (hash_table.find(id) == hash_table.end()) {
            fila_ids.push(id);
        }
        
        tree.root = tree.insert(tree.root, id, rating, potential, foot, atk_work, def_work);
        hash_table[id] = tree.search(tree.root, id);
        removerMaisAntigoSeNecessario();
        
        if (rating != rating_original || potential != potential_original) {
            cout << "Jogador inserido com DADOS ANÔMALOS! ";
        } else {
            cout << "Jogador inserido com sucesso! ";
        }
        cout << "(Total: " << hash_table.size() << "/" << LIMITE_ELEMENTOS << ")\n";
    }

    // Funções de benchmark (mantidas iguais, exceto pela chamada a getMemoryUsage())
    double benchmarkInsercao(int num_operacoes) {
        auto start = high_resolution_clock::now();
        
        for(int i = 0; i < num_operacoes; i++) {
            int id = 1000000 + i;
            tree.root = tree.insert(tree.root, id, 70.0 + (i%30), 80.0 + (i%20), 
                                  "right", "medium", "medium");
            hash_table[id] = tree.search(tree.root, id);
        }
        
        auto end = high_resolution_clock::now();
        duration<double> duration = end - start;
        return duration.count();
    }

    double benchmarkRemocao(int num_operacoes) {
        for(int i = 0; i < num_operacoes; i++) {
            int id = 2000000 + i;
            tree.root = tree.insert(tree.root, id, 70.0 + (i%30), 80.0 + (i%20), 
                                  "right", "medium", "medium");
            hash_table[id] = tree.search(tree.root, id);
        }

        auto start = high_resolution_clock::now();
        
        for(int i = 0; i < num_operacoes; i++) {
            int id = 2000000 + i;
            tree.root = tree.deleteNode(tree.root, id);
            hash_table.erase(id);
        }
        
        auto end = high_resolution_clock::now();
        duration<double> duration = end - start;
        return duration.count();
    }

    double benchmarkBusca(int num_operacoes) {
        vector<int> ids;
        for(int i = 0; i < num_operacoes; i++) {
            int id = 3000000 + i;
            tree.root = tree.insert(tree.root, id, 70.0 + (i%30), 80.0 + (i%20), 
                                  "right", "medium", "medium");
            hash_table[id] = tree.search(tree.root, id);
            ids.push_back(id);
        }

        random_device rd;
        mt19937 g(rd());
        shuffle(ids.begin(), ids.end(), g);

        auto start = high_resolution_clock::now();
        
        for(int id : ids) {
            tree.search(tree.root, id);
        }
        
        auto end = high_resolution_clock::now();
        duration<double> duration = end - start;
        return duration.count();
    }

    void benchmarkMemoria(int num_elementos) {
        size_t memoria_inicial = getMemoryUsage();
        
        for(int i = 0; i < num_elementos; i++) {
            int id = 4000000 + i;
            tree.root = tree.insert(tree.root, id, 70.0 + (i%30), 80.0 + (i%20), 
                                  "right", "medium", "medium");
            hash_table[id] = tree.search(tree.root, id);
        }
        
        size_t memoria_final = getMemoryUsage();
        cout << "Uso de memória para " << num_elementos << " elementos: " 
             << (memoria_final - memoria_inicial) << " KB\n";
    }

    void benchmarkEscalabilidade() {
        vector<int> tamanhos = {1000, 5000, 10000, 50000, 100000};
        
        cout << "\n=== TESTE DE ESCALABILIDADE ===\n";
        cout << "Tamanho | Tempo Inserção (s) | Tempo Busca (s) | Tempo Remoção (s)\n";
        
        for(int tamanho : tamanhos) {
            tree.root = nullptr;
            hash_table.clear();
            
            auto inicio_ins = high_resolution_clock::now();
            benchmarkInsercao(tamanho);
            auto fim_ins = high_resolution_clock::now();
            double tempo_ins = duration_cast<duration<double>>(fim_ins - inicio_ins).count();
            
            auto inicio_busca = high_resolution_clock::now();
            benchmarkBusca(tamanho);
            auto fim_busca = high_resolution_clock::now();
            double tempo_busca = duration_cast<duration<double>>(fim_busca - inicio_busca).count();
            
            auto inicio_rem = high_resolution_clock::now();
            benchmarkRemocao(tamanho);
            auto fim_rem = high_resolution_clock::now();
            double tempo_rem = duration_cast<duration<double>>(fim_rem - inicio_rem).count();
            
            cout << tamanho << " | " << tempo_ins << " | " << tempo_busca << " | " << tempo_rem << "\n";
        }
    }

    void benchmarkLatencia() {
        const int num_operacoes = 10000;
        vector<int> ids;
        
        for(int i = 0; i < num_operacoes; i++) {
            int id = 5000000 + i;
            tree.root = tree.insert(tree.root, id, 70.0 + (i%30), 80.0 + (i%20), 
                                  "right", "medium", "medium");
            hash_table[id] = tree.search(tree.root, id);
            ids.push_back(id);
        }
        
        random_device rd;
        mt19937 g(rd());
        shuffle(ids.begin(), ids.end(), g);
        
        auto start = high_resolution_clock::now();
        
        for(int i = 0; i < num_operacoes; i++) {
            tree.search(tree.root, ids[i]);
            
            if(i % 2 == 0) {
                tree.root = tree.deleteNode(tree.root, ids[i]);
                hash_table.erase(ids[i]);
                
                tree.root = tree.insert(tree.root, ids[i], 75.0, 85.0, 
                                      "left", "high", "low");
                hash_table[ids[i]] = tree.search(tree.root, ids[i]);
            }
        }
        
        auto end = high_resolution_clock::now();
        duration<double> duration = end - start;
        
        cout << "Latência média para operações combinadas: " 
             << (duration.count() / num_operacoes) * 1000 << " ms por operação\n";
    }

    // =============================================
    // NOVAS FUNCIONALIDADES DE ANÁLISE DE DADOS
    // =============================================

    // Cálculos estatísticos básicos
    void calcularEstatisticas() {
        vector<float> ratings;
        vector<float> potentials;
        
        for(auto& pair : hash_table) {
            ratings.push_back(pair.second->overall_rating);
            potentials.push_back(pair.second->potential);
        }
        
        // Cálculos para overall_rating
        float soma_rating = accumulate(ratings.begin(), ratings.end(), 0.0f);
        float media_rating = soma_rating / ratings.size();
        
        float sq_sum_rating = inner_product(ratings.begin(), ratings.end(), ratings.begin(), 0.0f);
        float stdev_rating = sqrt(sq_sum_rating / ratings.size() - media_rating * media_rating);
        
        // Cálculos para potential
        float soma_potential = accumulate(potentials.begin(), potentials.end(), 0.0f);
        float media_potential = soma_potential / potentials.size();
        
        float sq_sum_potential = inner_product(potentials.begin(), potentials.end(), potentials.begin(), 0.0f);
        float stdev_potential = sqrt(sq_sum_potential / potentials.size() - media_potential * media_potential);
        
        cout << fixed << setprecision(2);
        cout << "\n=== ESTATÍSTICAS DOS JOGADORES ===\n";
        cout << "Overall Rating:\n";
        cout << "  Média: " << media_rating << "\n";
        cout << "  Desvio Padrão: " << stdev_rating << "\n";
        cout << "  Min: " << *min_element(ratings.begin(), ratings.end()) << "\n";
        cout << "  Max: " << *max_element(ratings.begin(), ratings.end()) << "\n";
        
        cout << "\nPotential:\n";
        cout << "  Média: " << media_potential << "\n";
        cout << "  Desvio Padrão: " << stdev_potential << "\n";
        cout << "  Min: " << *min_element(potentials.begin(), potentials.end()) << "\n";
        cout << "  Max: " << *max_element(potentials.begin(), potentials.end()) << "\n";
    }

    // Distribuição por pé preferido
    void distribuirPorPePreferido() {
        map<string, int> contagem;
        
        for(auto& pair : hash_table) {
            contagem[pair.second->preferred_foot]++;
        }
        
        cout << "\n=== DISTRIBUIÇÃO POR PÉ PREFERIDO ===\n";
        for(auto& item : contagem) {
            cout << item.first << ": " << item.second << " jogadores ("
                 << (item.second * 100.0 / hash_table.size()) << "%)\n";
        }
    }

    // Filtragem por rating mínimo
    vector<Node*> filtrarPorRatingMinimo(float min_rating) {
        vector<Node*> resultado;
        
        for(auto& pair : hash_table) {
            if(pair.second->overall_rating >= min_rating) {
                resultado.push_back(pair.second);
            }
        }
        
        return resultado;
    }

    // Ordenação por potencial (decrescente)
    vector<Node*> ordenarPorPotencial() {
        vector<Node*> jogadores;
        
        for(auto& pair : hash_table) {
            jogadores.push_back(pair.second);
        }
        
        sort(jogadores.begin(), jogadores.end(), [](Node* a, Node* b) {
            return a->potential > b->potential;
        });
        
        return jogadores;
    }

    // Exibir resultados de filtros/ordenação
    void exibirJogadores(const vector<Node*>& jogadores, int limite = 10) {
        cout << "\n=== JOGADORES ENCONTRADOS ===\n";
        int mostrar = min(limite, (int)jogadores.size());
        
        for(int i = 0; i < mostrar; i++) {
            Node* node = jogadores[i];
            cout << "ID: " << node->player_id << " | Rating: " << node->overall_rating
                 << " | Potencial: " << node->potential << " | Pé: " << node->preferred_foot
                 << " | Ataque: " << node->attacking_work_rate 
                 << " | Defesa: " << node->defensive_work_rate << "\n";
        }
        
        if(jogadores.size() > mostrar) {
            cout << "... e mais " << (jogadores.size() - mostrar) << " jogadores\n";
        }
    }

    // Menu de análise de dados
    void menu_analise() {
        while(true) {
            cout << "\n=== MENU DE ANÁLISE DE DADOS ===\n";
            cout << "1. Calcular estatísticas básicas\n";
            cout << "2. Distribuição por pé preferido\n";
            cout << "3. Filtrar por rating mínimo\n";
            cout << "4. Ordenar por potencial\n";
            cout << "5. Voltar\n";
            
            string opcao;
            cout << "Escolha uma opção: ";
            cin >> opcao;
            
            if(opcao == "1") {
                calcularEstatisticas();
            } else if(opcao == "2") {
                distribuirPorPePreferido();
            } 
            else if(opcao == "3") {
                float min_rating;
                cout << "Digite o rating mínimo: ";
                cin >> min_rating;
                vector<Node*> resultado = filtrarPorRatingMinimo(min_rating);
                exibirJogadores(resultado);
            } else if(opcao == "4") {
                vector<Node*> resultado = ordenarPorPotencial();
                exibirJogadores(resultado);
            } else if(opcao == "5") {
                break;
            } else {
                cout << "Opção inválida!\n";
            }
            
            cout << "\nPressione Enter para continuar...";
            cin.ignore();
            cin.get();
        }
    }

    // Função para controlar a restrição
    void setRestricaoProcessamento(bool habilitar) {
        habilitarRestricoesProcessamento = habilitar;
        cout << "Restrição R10 " << (habilitar ? "ativada" : "desativada") 
             << " (interrupções de 100ms)\n";
    }
    void toggleLatencia() {
        habilitarLatencia = !habilitarLatencia;
        cout << "Restrição R12 " << (habilitarLatencia ? "ativada" : "desativada")
             << " (latência 300-700ms)\n";
    }

    void toggleDadosAnomalos() {
        habilitarDadosAnomalos = !habilitarDadosAnomalos;
        cout << "Restrição R18 " << (habilitarDadosAnomalos ? "ativada" : "desativada")
             << " (10%. dados anômalos)\n";
    }

    // Retorna o limite atual de elementos
    size_t getLimiteElementos() const { 
        return LIMITE_ELEMENTOS; 
    }

    // Retorna se o limite está ativo
    bool getLimiteAtivo() const { 
        return limiteAtivo; 
    }

    void toggleLimiteElementos() {
        limiteAtivo = !limiteAtivo;
        cout << "Restrição R2 " << (limiteAtivo ? "ativada" : "desativada")
             << " (Limite: " << (limiteAtivo ? to_string(LIMITE_ELEMENTOS) : "ilimitado") << ")\n";
    }

    void ajustarLimiteElementos(size_t novoLimite) {
        LIMITE_ELEMENTOS = novoLimite;
        cout << "Novo limite definido para " << LIMITE_ELEMENTOS << " elementos\n";
        // Remove elementos excedentes se necessário
        while (limiteAtivo && hash_table.size() > LIMITE_ELEMENTOS) {
            removerMaisAntigoSeNecessario();
        }
    }

    void toggleRestricaoAcessos() {
        tree.restricaoAcessoMemoria = !tree.restricaoAcessoMemoria;
        cout << "Restrição R22 " << (tree.restricaoAcessoMemoria ? "ativada" : "desativada")
             << " (limite: " << tree.LIMITE_ACESSOS << " acessos por busca)\n";
    }
};

void menu_benchmark() {
    string caminho = "dataset_limpo3.csv";
    GerenciadorJogadoresAVL gerenciador(caminho);
    
    while(true) {
        cout << "\n=== MENU DE BENCHMARK ===\n";
        cout << "1. Tempo de Inserção\n";
        cout << "2. Tempo de Remoção\n";
        cout << "3. Tempo de Busca\n";
        cout << "4. Uso de Memória\n";
        cout << "5. Escalabilidade\n";
        cout << "6. Latência Média\n";
        cout << "7. Voltar\n";
        
        string opcao;
        cout << "Escolha uma opção: ";
        cin >> opcao;
        
        if(opcao == "1") {
            double tempo = gerenciador.benchmarkInsercao(10000);
            cout << "Tempo para inserir 10,000 elementos: " << tempo << " segundos\n";
        } else if(opcao == "2") {
            double tempo = gerenciador.benchmarkRemocao(10000);
            cout << "Tempo para remover 10,000 elementos: " << tempo << " segundos\n";
        } else if(opcao == "3") {
            double tempo = gerenciador.benchmarkBusca(10000);
            cout << "Tempo para buscar 10,000 elementos: " << tempo << " segundos\n";
        } else if(opcao == "4") {
            gerenciador.benchmarkMemoria(10000);
            gerenciador.benchmarkMemoria(50000);
            gerenciador.benchmarkMemoria(100000);
        } else if(opcao == "5") {
            gerenciador.benchmarkEscalabilidade();
        } else if(opcao == "6") {
            gerenciador.benchmarkLatencia();
        } else if(opcao == "7") {
            break;
        } else {
            cout << "Opção inválida!\n";
        }
        
        cout << "\nPressione Enter para continuar...";
        cin.ignore();
        cin.get();
    }
}

void menu_avl() {
    string caminho = "dataset_limpo3.csv";
    GerenciadorJogadoresAVL gerenciador(caminho);

    while (true) {
        cout << "\n=== GERENCIADOR DE JOGADORES (AVL) ===\n";
        cout << "1. Buscar jogador por ID\n";
        cout << "2. Inserir novo jogador\n";
        cout << "3. Remover jogador por ID\n";
        cout << "4. Executar Benchmarks\n";
        cout << "5. Análise de Dados\n";
        cout << "6. Controle de Restrições\n";
        cout << "7. Sair\n";

        string opcao;
        cout << "Escolha uma opção: ";
        cin >> opcao;

        if (opcao == "1") {
            int id;
            cout << "Digite o ID do jogador: ";
            cin >> id;
            gerenciador.buscarPorId(id);
        } else if (opcao == "2") {
            int id;
            float rating, potential;
            string foot, atk_work, def_work;
            
            cout << "ID do jogador: ";
            cin >> id;
            cout << "Overall Rating: ";
            cin >> rating;
            cout << "Potential: ";
            cin >> potential;
            cout << "Preferred Foot (left/right): ";
            cin >> foot;
            cout << "Attacking Work Rate (low/medium/high): ";
            cin >> atk_work;
            cout << "Defensive Work Rate (low/medium/high): ";
            cin >> def_work;
            
            gerenciador.inserirJogador(id, rating, potential, foot, atk_work, def_work);
        } else if (opcao == "3") {
            int id;
            cout << "Digite o ID do jogador a ser removido: ";
            cin >> id;
            gerenciador.removerJogador(id);
        } else if (opcao == "4") {
            menu_benchmark();
        } else if (opcao == "5") {
            gerenciador.menu_analise();
        } else if (opcao == "6") {  // Novo menu de controle
            cout << "\n=== CONTROLE DE RESTRIÇÕES ===\n";
            cout << "1. Ativar/Desativar Restrição R10 (Interrupções 100ms)\n";
            cout << "2. Alternar Latência Banco (R12)\n";
            cout << "3. Alternar Dados Anômalos (R18)\n";
            cout << "4. " << (gerenciador.getLimiteAtivo() ? "Desativar" : "Ativar") 
                 << " limite de elementos (R2)\n";
            cout << "5. Ajustar limite máximo (atual: " << gerenciador.getLimiteElementos() << ")\n";
            cout << "6. Limite de Acessos (R22)\n";
            cout << "7. Voltar\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpa o buffer
            char op = getchar();
            if (op == '1') {
                static bool status = true;
                gerenciador.setRestricaoProcessamento(!status);
                status = !status;
            }
            else if (op == '2') {
                gerenciador.toggleLatencia();
            }
            else if (op == '3') {
                gerenciador.toggleDadosAnomalos();
            }
            else if (op == '4') {
                gerenciador.toggleLimiteElementos();
            }
            else if (op == '5') {
                size_t novoLimite;
                cout << "Novo limite (1-10000): ";
                cin >> novoLimite;
                if (novoLimite > 0 && novoLimite <= 10000) {
                    gerenciador.ajustarLimiteElementos(novoLimite);
                } else {
                    cout << "Valor inválido!\n";
                }
            }
            else if (op == '6') {
                gerenciador.toggleRestricaoAcessos();
            }
            else if (op == '7') {
                cout << "\n";
            }
            else {
                break;
            }
        } else if (opcao == "7") {
            break;
        } else {
            cout << "Opção inválida!\n";
        }

        cout << "\nPressione Enter para continuar...";
        cin.ignore();
        cin.get();
    }
}

int main() {
    menu_avl();
    return 0;
}