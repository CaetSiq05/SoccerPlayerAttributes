#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <algorithm>
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

int main() {
    KDTree tree;
    ifstream file("dataset_limpo3.csv");
    string line;
    getline(file, line); // cabeçalho
    while (getline(file, line)) {
        Player p = parseCSVLine(line);
        tree.insert(p);
    }

    int opcao;
    do {
        cout << "\n1. Inserir\n2. Buscar por ID\n3. Remover por ID\n4. Mostrar todos\n5. Sair\nOpcao: ";
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
    } while (opcao != 5);

    return 0;
}
