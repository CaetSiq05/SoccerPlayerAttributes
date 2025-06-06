#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
using namespace std;

struct Player {
    int player_fifa_api_id;
    float overall_rating;
    float potential;
    string preferred_foot;
    string attacking_work_rate;
    string defensive_work_rate;
    float crossing, finishing, heading_accuracy, short_passing, volleys, dribbling;
    float curve, free_kick_accuracy, long_passing, ball_control, acceleration;
    float sprint_speed, agility, reactions, balance, shot_power, jumping;
    float stamina, strength, long_shots, aggression, interceptions, positioning;
    float vision, penalties, marking, standing_tackle, sliding_tackle;

    void print() const {
        cout << "ID: " << player_fifa_api_id << " | Overall: " << overall_rating
             << " | Potential: " << potential << " | Foot: " << preferred_foot
             << " | Finishing: " << finishing << " | Curve: " << curve
             << " | Penalties: " << penalties << endl;
    }
};

class Node {
public:
    float key;
    vector<Player> players;
    vector<Node*> forward;

    Node(float k, const Player& p, int level) : key(k) {
        players.push_back(p);
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
        while ((float)rand() / RAND_MAX < probability && lvl < maxLevel) {
            lvl++;
        }
        return lvl;
    }

    void insert(float key, const Player& player) {
        vector<Node*> update(maxLevel + 1);
        Node* current = header;

        for (int i = maxLevel; i >= 0; i--) {
            while (current->forward[i] != nullptr && current->forward[i]->key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current != nullptr && current->key == key) {
            current->players.push_back(player);
        } else {
            int rlevel = randomLevel();
            Node* n = new Node(key, player, rlevel);
            for (int i = 0; i <= rlevel; i++) {
                n->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = n;
            }
        }
    }

    vector<Player>* search(float key) {
        Node* current = header;
        for (int i = maxLevel; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->key < key) {
                current = current->forward[i];
            }
        }
        current = current->forward[0];
        if (current && current->key == key) {
            return &current->players;
        }
        return nullptr;
    }

    void remove(float key) {
        vector<Node*> update(maxLevel + 1);
        Node* current = header;

        for (int i = maxLevel; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current && current->key == key) {
            for (int i = 0; i <= maxLevel; i++) {
                if (update[i]->forward[i] != current)
                    break;
                update[i]->forward[i] = current->forward[i];
            }
            delete current;
        }
    }

    void display() const {
        vector<Node*> nodes;
        Node* node = header->forward[0];
        while (node != nullptr) {
            nodes.push_back(node);
            node = node->forward[0];
        }
        sort(nodes.begin(), nodes.end(), [](Node* a, Node* b) {
            return a->key > b->key;
        });
        for (Node* n : nodes) {
            for (const Player& p : n->players) {
                p.print();
            }
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
    getline(ss, p.attacking_work_rate, ',');
    getline(ss, p.defensive_work_rate, ',');

    float* attrs[] = {
        &p.crossing, &p.finishing, &p.heading_accuracy, &p.short_passing,
        &p.volleys, &p.dribbling, &p.curve, &p.free_kick_accuracy, &p.long_passing,
        &p.ball_control, &p.acceleration, &p.sprint_speed, &p.agility,
        &p.reactions, &p.balance, &p.shot_power, &p.jumping, &p.stamina,
        &p.strength, &p.long_shots, &p.aggression, &p.interceptions, &p.positioning,
        &p.vision, &p.penalties, &p.marking, &p.standing_tackle, &p.sliding_tackle
    };

    for (float* attr : attrs) {
        getline(ss, token, ',');
        *attr = stof(token);
    }
    return p;
}

int main() {
    SkipList skiplist(5, 0.5);
    string path = "dataset_limpo3.csv";
    ifstream file(path);
    string line;
    getline(file, line);
    while (getline(file, line)) {
        Player p = parseCSVLine(line);
        skiplist.insert(p.overall_rating, p);
    }
    file.close();

    int opcao;
    do {
        cout << "\nMenu:\n1. Inserir\n2. Buscar\n3. Remover\n4. Imprimir\n5. Sair\nOpcao: ";
        cin >> opcao;
        if (opcao == 1) {
            Player p;
            cout << "ID: "; cin >> p.player_fifa_api_id;
            cout << "Overall: "; cin >> p.overall_rating;
            cout << "Potential: "; cin >> p.potential;
            cout << "Preferred Foot: "; cin >> p.preferred_foot;
            cout << "Finishing: "; cin >> p.finishing;
            cout << "Curve: "; cin >> p.curve;
            cout << "Penalties: "; cin >> p.penalties;
            skiplist.insert(p.overall_rating, p);
        } else if (opcao == 2) {
            float key;
            cout << "Digite o overall_rating: ";
            cin >> key;
            vector<Player>* results = skiplist.search(key);
            if (results) {
                for (const Player& p : *results) {
                    p.print();
                }
            } else {
                cout << "Nenhum jogador encontrado com essa nota.\n";
            }
        } else if (opcao == 3) {
            float key;
            cout << "Digite o overall_rating: ";
            cin >> key;
            skiplist.remove(key);
        } else if (opcao == 4) {
            skiplist.display();
        }
    } while (opcao != 5);

    return 0;
}
