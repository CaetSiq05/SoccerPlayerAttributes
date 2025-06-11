#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <algorithm>
#include <random>      // Para gerar números aleatórios
#include <chrono>      // Para medir tempo

using namespace std;

const int K = 36; // 36 atributos numericos

struct Player {
    int player_fifa_api_id;
    float overall_rating;
    float potential;
    string preferred_foot;
    float features[K];

    void print() const {
        cout << "ID: " << player_fifa_api_id
             << " | Overall: " << overall_rating
             << " | Potential: " << potential
             << " | Preferred Foot: " << preferred_foot << endl;
    }
};

struct KDNode {
    Player player;
    KDNode *left, *right;
    KDNode(const Player& p) : player(p), left(nullptr), right(nullptr) {}
};

class KDTree {
    KDNode* root;

    int contarNos(KDNode* node) const {
    if (!node) return 0;
    return 1 + contarNos(node->left) + contarNos(node->right);
    }


    KDNode* insertRec(KDNode* node, const Player& p, int depth) {
        if (!node) return new KDNode(p);
        int cd = depth % K;
        if (p.features[cd] < node->player.features[cd])
            node->left = insertRec(node->left, p, depth + 1);
        else
            node->right = insertRec(node->right, p, depth + 1);
        return node;
    }

    KDNode* searchByID(KDNode* node, int id) {
        if (!node) return nullptr;
        if (node->player.player_fifa_api_id == id) return node;
        KDNode* leftSearch = searchByID(node->left, id);
        if (leftSearch) return leftSearch;
        return searchByID(node->right, id);
    }

    KDNode* removeByID(KDNode* node, int id, int depth) {
        if (!node) return nullptr;
        if (node->player.player_fifa_api_id == id) {
            delete node;
            return nullptr;
        }
        int cd = depth % K;
        node->left = removeByID(node->left, id, depth + 1);
        node->right = removeByID(node->right, id, depth + 1);
        return node;
    }

    void inorder(KDNode* node) {
        if (!node) return;
        inorder(node->left);
        node->player.print();
        inorder(node->right);
    }

public:
    int totalNodes() const {
    return contarNos(root);
    }

    KDTree() : root(nullptr) {}

    void insert(const Player& p) {
        root = insertRec(root, p, 0);
    }

    Player* search(int id) {
        KDNode* res = searchByID(root, id);
        return res ? &res->player : nullptr;
    }

    void remove(int id) {
        root = removeByID(root, id, 0);
    }

    void printAll() {
        inorder(root);
    }
};

float safe_stof(const string& s) {
    try { return stof(s); }
    catch (...) { return 0.0f; }
}

Player parseCSVLine(const string& line) {
    stringstream ss(line);
    string token;
    Player p;

    getline(ss, token, ','); p.player_fifa_api_id = stoi(token);
    getline(ss, token, ','); p.overall_rating = safe_stof(token);
    getline(ss, token, ','); p.potential = safe_stof(token);
    getline(ss, p.preferred_foot, ',');

    // Ignora 2 colunas string
    for (int i = 0; i < 2; i++) getline(ss, token, ',');

    p.features[0] = p.overall_rating;
    p.features[1] = p.potential;

    int idx = 2;
    while (getline(ss, token, ',') && idx < K) {
        p.features[idx++] = safe_stof(token);
    }
    return p;
}

vector<Player> gerarPontos(int n) {
    vector<Player> pontos;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 100.0);

    for (int i = 0; i < n; i++) {
        Player p;
        p.player_fifa_api_id = i;
        p.features[0] = dis(gen);
        p.features[1] = dis(gen);
        pontos.push_back(p);
    }
    return pontos;
}

double medirTempoInsercao(KDTree& tree, const vector<Player>& pontos) {
    auto start = chrono::high_resolution_clock::now();
    for (const Player& p : pontos) {
        tree.insert(p);
    }
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double, micro>(end - start).count();
}

double medirTempoRemocao(KDTree& tree, const vector<Player>& pontos) {
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < pontos.size() / 10; i++) {
        tree.remove(pontos[i].player_fifa_api_id);
    }
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double, micro>(end - start).count();
}

double medirTempoBusca(KDTree& tree, const vector<Player>& pontos) {
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < pontos.size() / 10; i++) {
        tree.search(pontos[i].player_fifa_api_id);
    }
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double, micro>(end - start).count();
}

size_t usoDeMemoria(const KDTree& tree) {
    return tree.totalNodes() * sizeof(KDNode);
}

double tempoMedioAcesso(KDTree& tree, const vector<Player>& pontos) {
    double soma = 0;
    int n = 100;
    for (int i = 0; i < n; i++) {
        int id = pontos[rand() % pontos.size()].player_fifa_api_id;
        auto ini = chrono::high_resolution_clock::now();
        tree.search(id);
        auto fim = chrono::high_resolution_clock::now();
        soma += chrono::duration<double, micro>(fim - ini).count();
    }
    return soma / n;
}

void testarEscalabilidade() {
    vector<int> tamanhos = {1000, 5000, 10000};
    for (int tam : tamanhos) {
        KDTree tree;
        vector<Player> pontos = gerarPontos(tam);
        double tInsercao = medirTempoInsercao(tree, pontos);
        double tBusca = medirTempoBusca(tree, pontos);
        cout << "\nTamanho: " << tam
             << " | Inserção: " << tInsercao << " µs"
             << " | Busca: " << tBusca << " µs" << endl;
    }
}
double medirLatenciaMedia(KDTree& tree, const vector<Player>& pontos) {
    double total = 0;
    int n = 50;
    for (int i = 0; i < n; i++) {
        Player p = pontos[rand() % pontos.size()];
        auto ini = chrono::high_resolution_clock::now();
        tree.insert(p);
        tree.search(p.player_fifa_api_id);
        tree.remove(p.player_fifa_api_id);
        auto fim = chrono::high_resolution_clock::now();
        total += chrono::duration<double, micro>(fim - ini).count();
    }
    return total / n;
}

void estatisticasBasicas(const vector<Player>& jogadores) {
    if (jogadores.empty()) {
        cout << "Nenhum dado disponível.\n";
        return;
    }

    double somaOverall = 0, somaPotential = 0;
    double somaQuadradoOverall = 0, somaQuadradoPotential = 0;
    int n = jogadores.size();

    for (const auto& j : jogadores) {
        somaOverall += j.overall_rating;
        somaPotential += j.potential;
        somaQuadradoOverall += j.overall_rating * j.overall_rating;
        somaQuadradoPotential += j.potential * j.potential;
    }

    double mediaOverall = somaOverall / n;
    double mediaPotential = somaPotential / n;
    double desvioOverall = sqrt((somaQuadradoOverall / n) - (mediaOverall * mediaOverall));
    double desvioPotential = sqrt((somaQuadradoPotential / n) - (mediaPotential * mediaPotential));

    cout << "\n===== Estatísticas =====\n";
    cout << "Media Overall: " << mediaOverall << ", Desvio Padrão: " << desvioOverall << endl;
    cout << "Media Potential: " << mediaPotential << ", Desvio Padrão: " << desvioPotential << endl;
}

void agruparPorPreferredFoot(const vector<Player>& jogadores) {
    int destro = 0, canhoto = 0, outros = 0;

    for (const auto& j : jogadores) {
        if (j.preferred_foot == "right" || j.preferred_foot == "Right")
            destro++;
        else if (j.preferred_foot == "left" || j.preferred_foot == "Left")
            canhoto++;
        else
            outros++;
    }

    cout << "\n===== Agrupamento por Preferred Foot =====\n";
    cout << "Destros: " << destro << "\nCanhotos: " << canhoto << "\nOutros: " << outros << endl;
}

void filtrarOrdenarJogadores(const vector<Player>& jogadores) {
    vector<Player> filtrados;

    for (const auto& j : jogadores) {
        if (j.overall_rating >= 80) {
            filtrados.push_back(j);
        }
    }

    sort(filtrados.begin(), filtrados.end(), [](const Player& a, const Player& b) {
        return a.potential > b.potential;
    });

    cout << "\n===== Jogadores com Overall >= 80 ordenados por Potential =====\n";
    for (const auto& j : filtrados) {
        j.print();
    }
}

Player buscarSubstitutoSemelhante(const Player& lesionado, const vector<Player>& jogadores) {
    Player substituto;
    double menorDistancia = numeric_limits<double>::max();
    bool encontrado = false;

    for (const auto& j : jogadores) {
        if (j.player_fifa_api_id == lesionado.player_fifa_api_id) continue; // Ignora o próprio jogador

        double dist = 0;
        for (int i = 0; i < K; ++i) {
            dist += pow(j.features[i] - lesionado.features[i], 2);
        }
        dist = sqrt(dist);

        if (dist < menorDistancia) {
            menorDistancia = dist;
            substituto = j;
            encontrado = true;
        }
    }

    if (encontrado)
        return substituto;
    else
        return Player(); // Retorna vazio se não encontrar
}


int main() {
    KDTree tree;
    ifstream file("dataset_limpo3.csv");
    vector<Player> historicoInseridos;

    string line;
    getline(file, line); // cabeçalho

    while (getline(file, line)) {
    Player p = parseCSVLine(line);
    tree.insert(p);
    historicoInseridos.push_back(p);  // <-- isso resolve seu problema
}

    while (getline(file, line)) {
        Player p = parseCSVLine(line);
        tree.insert(p);
    }

    int opcao;
    do {
         cout << "\n--- MENU ---\n";
        cout << "1. Adicionar jogador\n";
        cout << "2. Buscar jogador por ID\n";
        cout << "3. Remover jogador\n";
        cout << "4. Imprimir todos os jogadores\n";
        cout << "5. Benchmarks\n";
        cout << "6. Calculo Estatisticos\n";
        cout << "7. Agrupar Por PreferredFoot\n";
        cout << "8. Ordenar por Overall\n";
        cout << "9. Substituicao de Jogador\n";
        cout << "10. Sair\n";
        cout << "Escolha uma opcao: ";
        cin >> opcao;

        if (opcao == 1) {
            Player p;
            cout << "ID: "; cin >> p.player_fifa_api_id;
            cout << "Overall: "; cin >> p.overall_rating;
            cout << "Potential: "; cin >> p.potential;
            cout << "Preferred Foot: "; cin >> p.preferred_foot;
            fill(begin(p.features), end(p.features), 0.0f);
            p.features[0] = p.overall_rating;
            p.features[1] = p.potential;
            tree.insert(p);
            historicoInseridos.push_back(p); // <-- Adiciona ao vetor para estatísticas

            tree.insert(p);
        } else if (opcao == 2) {
            int id;
            cout << "Digite o ID: ";
            cin >> id;
            Player* found = tree.search(id);
            if (found) found->print();
            else cout << "Jogador nao encontrado.\n";
        } else if (opcao == 3) {
            int id;
            cout << "Digite o ID: ";
            cin >> id;
            tree.remove(id);
            cout << "Remocao finalizada (caso o ID existisse).\n";
        } else if (opcao == 4) {
            tree.printAll();
        }

        else if (opcao == 5){
        auto pontos = gerarPontos(5000);
        KDTree tree;

        double tinsercao = medirTempoInsercao(tree, pontos);
        double tbusca = medirTempoBusca(tree, pontos);
        double tremocao = medirTempoRemocao(tree, pontos);
        double tmedio = tempoMedioAcesso(tree, pontos);
        double tlatencia = medirLatenciaMedia(tree, pontos);
        size_t memoria = usoDeMemoria(tree);

        cout << "\n======= BENCHMARKS - KD-TREE =======\n";
        cout << "Tempo de Inserção: " << tinsercao << " µs\n";
        cout << "Tempo de Busca: " << tbusca << " µs\n";
        cout << "Tempo de Remoção: " << tremocao << " µs\n";
        cout << "Tempo Médio de Acesso: " << tmedio << " µs\n";
        cout << "Latência Média (inserção + busca + remoção): " << tlatencia << " µs\n";
        cout << "Uso estimado de Memória: " << memoria / 1024.0 << " KB\n";

        cout << "\n--- Teste de Escalabilidade ---\n";
        testarEscalabilidade();
        }

          else if (opcao == 6) {
            estatisticasBasicas(historicoInseridos);
        }

          else if (opcao == 7) {
            agruparPorPreferredFoot(historicoInseridos);
        }
          else if (opcao == 8) {
           filtrarOrdenarJogadores(historicoInseridos);
        }
        else if (opcao == 9) {
    int id;
    cout << "Digite o ID do jogador lesionado: ";
    cin >> id;
    Player* lesionado = tree.search(id);
    if (lesionado) {
        Player substituto = buscarSubstitutoSemelhante(*lesionado, historicoInseridos);
        cout << "Jogador substituto mais semelhante encontrado:\n";
        substituto.print();
    } else {
        cout << "Jogador lesionado nao encontrado.\n";
    }

        }

} while (opcao != 9);

    return 0;
}

