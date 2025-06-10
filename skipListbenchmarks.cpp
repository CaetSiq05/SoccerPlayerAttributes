#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include <random>

using namespace std;
using namespace chrono;

struct Player {
    int player_fifa_api_id;
    float overall_rating;
    float potential;
    string preferred_foot;
};

class Node {
public:
    int key;
    Player player;
    vector<Node*> forward;

    Node(int k, const Player& p, int level) : key(k), player(p) {
        forward.resize(level + 1, nullptr);
    }
};

class SkipList {
    int maxLevel;
    float probability;
    Node* header;

public:
    SkipList(int maxLevel, float probability) {
        this->maxLevel = maxLevel;
        this->probability = probability;
        header = new Node(-1, {}, maxLevel);
        srand(time(0));
    }

    int randomLevel() {
        int lvl = 0;
        while (((float)rand() / RAND_MAX) < probability && lvl < maxLevel) lvl++;
        return lvl;
    }

    void insert(int key, const Player& player) {
        vector<Node*> update(maxLevel + 1);
        Node* current = header;

        for (int i = maxLevel; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->key < key)
                current = current->forward[i];
            update[i] = current;
        }

        current = current->forward[0];

        if (current && current->key == key) {
            current->player = player; // Atualiza
        } else {
            int rlevel = randomLevel();
            Node* n = new Node(key, player, rlevel);
            for (int i = 0; i <= rlevel; i++) {
                n->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = n;
            }
        }
    }

    Player* search(int key) {
        Node* current = header;
        for (int i = maxLevel; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->key < key)
                current = current->forward[i];
        }

        current = current->forward[0];
        if (current && current->key == key)
            return &current->player;

        return nullptr;
    }

    bool remove(int key) {
        vector<Node*> update(maxLevel + 1);
        Node* current = header;

        for (int i = maxLevel; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->key < key)
                current = current->forward[i];
            update[i] = current;
        }

        current = current->forward[0];

        if (current && current->key == key) {
            for (int i = 0; i <= maxLevel; i++) {
                if (update[i]->forward[i] != current) break;
                update[i]->forward[i] = current->forward[i];
            }
            delete current;
            return true;
        }
        return false;
    }

    size_t countNodes() {
        size_t count = 0;
        Node* current = header->forward[0];
        while (current) {
            count++;
            current = current->forward[0];
        }
        return count;
    }

    void display() {
        Node* current = header->forward[0];
        while (current) {
            cout << "ID: " << current->player.player_fifa_api_id
                 << ", Overall: " << current->player.overall_rating
                 << ", Potential: " << current->player.potential
                 << ", Preferred foot: " << current->player.preferred_foot << endl;
            current = current->forward[0];
        }
    }
};

Player parseCSVLine(const string& line) {
    stringstream ss(line);
    string token;
    Player p;
    getline(ss, token, ','); p.player_fifa_api_id = stoi(token);
    getline(ss, token, ','); p.overall_rating = stof(token);
    getline(ss, token, ','); p.potential = stof(token);
    getline(ss, token, ','); p.preferred_foot = token;
    return p;
}

void benchmark(SkipList& sl, const vector<Player>& jogadores) {
    auto start = high_resolution_clock::now();
    for (const Player& p : jogadores)
        sl.insert(p.player_fifa_api_id, p);
    auto end = high_resolution_clock::now();
    cout << "Tempo de Inserção: " << duration<double, milli>(end - start).count() << " ms\n";

    start = high_resolution_clock::now();
    for (const Player& p : jogadores)
        sl.search(p.player_fifa_api_id);
    end = high_resolution_clock::now();
    cout << "Tempo de Busca: " << duration<double, milli>(end - start).count() << " ms\n";

    start = high_resolution_clock::now();
    for (const Player& p : jogadores)
        sl.remove(p.player_fifa_api_id);
    end = high_resolution_clock::now();
    cout << "Tempo de Remoção: " << duration<double, milli>(end - start).count() << " ms\n";

    size_t mem = sl.countNodes() * sizeof(Node);
    cout << "Uso estimado de memória: " << mem / 1024.0 << " KB\n";

    // Tempo Médio de Acesso (buscas aleatórias)
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, jogadores.size() - 1);

    double soma = 0;
    for (int i = 0; i < 100; i++) {
        int idx = dist(gen);
        auto ini = high_resolution_clock::now();
        sl.search(jogadores[idx].player_fifa_api_id);
        auto fim = high_resolution_clock::now();
        soma += duration<double, micro>(fim - ini).count();
    }
    cout << "Tempo médio de acesso (100 buscas aleatórias): " << (soma / 100.0) << " µs\n";
}

int main() {
    vector<Player> jogadores;
    ifstream file("/mnt/data/dataset_limpo3.csv");
    string line;
    getline(file, line); // cabeçalho
    while (getline(file, line)) {
        jogadores.push_back(parseCSVLine(line));
    }
    file.close();

    SkipList sl(5, 0.5);
    for (const Player& p : jogadores)
        sl.insert(p.player_fifa_api_id, p);

    int op;
    do {
        cout << "\nMENU:\n1. Inserir jogador\n2. Remover jogador\n3. Buscar jogador\n4. Exibir todos\n5. Benchmark\n6. Sair\nOpcao: ";
        cin >> op;

        if (op == 1) {
            Player p;
            cout << "ID: "; cin >> p.player_fifa_api_id;
            cout << "Overall: "; cin >> p.overall_rating;
            cout << "Potential: "; cin >> p.potential;
            cout << "Preferred Foot: "; cin >> p.preferred_foot;
            sl.insert(p.player_fifa_api_id, p);

        } else if (op == 2) {
            int id;
            cout << "ID para remover: "; cin >> id;
            if (sl.remove(id)) cout << "Removido.\n";
            else cout << "Nao encontrado.\n";

        } else if (op == 3) {
            int id;
            cout << "ID para buscar: "; cin >> id;
            Player* p = sl.search(id);
            if (p) p->print();
            else cout << "Nao encontrado.\n";

        } else if (op == 4) {
            sl.display();

        } else if (op == 5) {
            benchmark(sl, jogadores);
        }
    } while (op != 6);

    return 0;
}
