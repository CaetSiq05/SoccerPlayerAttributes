#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <chrono>
using namespace std;

struct Player {
    int player_fifa_api_id;
    float overall_rating;
    float potential;
    string preferred_foot;

    void print() const {
        cout << "ID: " << player_fifa_api_id
             << " | Overall: " << overall_rating
             << " | Potential: " << potential
             << " | Foot: " << preferred_foot << endl;
    }
};

class Node {
public:
    int id;
    Player player;
    vector<Node*> forward;

    Node(int id, const Player& p, int level) : id(id), player(p) {
        forward.resize(level + 1, nullptr);
    }
};

class SkipList {
private:
    int maxLevel;
    float probability;
    Node* header;

    int randomLevel() {
        int lvl = 0;
        while (((float)rand() / RAND_MAX) < probability && lvl < maxLevel) {
            lvl++;
        }
        return lvl;
    }

public:
    SkipList(int maxLevel, float probability) : maxLevel(maxLevel), probability(probability) {
        header = new Node(-1, {}, maxLevel);
    }

    void insert(int id, const Player& player) {
        vector<Node*> update(maxLevel + 1);
        Node* current = header;

        for (int i = maxLevel; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->id < id) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];
        if (current && current->id == id) {
            current->player = player; // Atualiza se já existir
        } else {
            int lvl = randomLevel();
            Node* newNode = new Node(id, player, lvl);
            for (int i = 0; i <= lvl; i++) {
                newNode->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = newNode;
            }
        }
    }

    Player* search(int id) {
        Node* current = header;
        for (int i = maxLevel; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->id < id) {
                current = current->forward[i];
            }
        }
        current = current->forward[0];
        if (current && current->id == id) {
            return &current->player;
        }
        return nullptr;
    }

    bool remove(int id) {
        vector<Node*> update(maxLevel + 1);
        Node* current = header;

        for (int i = maxLevel; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->id < id) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];
        if (current && current->id == id) {
            for (int i = 0; i <= maxLevel; i++) {
                if (update[i]->forward[i] != current) break;
                update[i]->forward[i] = current->forward[i];
            }
            delete current;
            return true;
        }
        return false;
    }

    void display() const {
        Node* current = header->forward[0];
        while (current) {
            current->player.print();
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
    getline(ss, p.preferred_foot, ',');

    return p;
}

int main() {
    SkipList skiplist(5, 0.5);

    ifstream file("dataset_limpo3.csv");
    string line;
    getline(file, line); // Pula cabeçalho

    while (getline(file, line)) {
        Player p = parseCSVLine(line);
        skiplist.insert(p.player_fifa_api_id, p);
    }
    file.close();

    int opcao;
    do {
        cout << "\n1. Inserir\n2. Buscar\n3. Remover\n4. Mostrar Todos\n5. Sair\nEscolha: ";
        cin >> opcao;

        if (opcao == 1) {
            Player p;
            cout << "ID: "; cin >> p.player_fifa_api_id;
            cout << "Overall: "; cin >> p.overall_rating;
            cout << "Potential: "; cin >> p.potential;
            cout << "Preferred Foot: "; cin >> p.preferred_foot;
            skiplist.insert(p.player_fifa_api_id, p);
        } else if (opcao == 2) {
            int id;
            cout << "Digite o ID: ";
            cin >> id;
            Player* found = skiplist.search(id);
            if (found) found->print();
            else cout << "Jogador nao encontrado.\n";
        } else if (opcao == 3) {
            int id;
            cout << "Digite o ID: ";
            cin >> id;
            if (skiplist.remove(id)) cout << "Removido com sucesso!\n";
            else cout << "Jogador nao encontrado.\n";
        } else if (opcao == 4) {
            skiplist.display();
        }
    } while (opcao != 5);

    return 0;
}
