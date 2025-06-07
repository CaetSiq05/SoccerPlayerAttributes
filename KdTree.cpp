#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
using namespace std;

const int K = 31; // Número de dimensões (atributos numéricos)

struct Player {
    int player_fifa_api_id;
    float features[K]; // 31 atributos numéricos

    void print() const {
        cout << "ID: " << player_fifa_api_id << "\nAtributos:\n";
        for (int i = 0; i < K; i++) {
            cout << features[i] << " ";
        }
        cout << "\n";
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

    KDNode* findMin(KDNode* node, int d, int depth) {
        if (!node) return nullptr;
        int cd = depth % K;

        if (cd == d) {
            if (!node->left) return node;
            return findMin(node->left, d, depth + 1);
        }

        KDNode* l = findMin(node->left, d, depth + 1);
        KDNode* r = findMin(node->right, d, depth + 1);

        KDNode* minNode = node;
        if (l && l->player.features[d] < minNode->player.features[d]) minNode = l;
        if (r && r->player.features[d] < minNode->player.features[d]) minNode = r;
        return minNode;
    }

    KDNode* deleteRec(KDNode* node, const Player& p, int depth) {
        if (!node) return nullptr;
        int cd = depth % K;

        if (node->player.player_fifa_api_id == p.player_fifa_api_id) {
            if (node->right) {
                KDNode* min = findMin(node->right, cd, depth + 1);
                node->player = min->player;
                node->right = deleteRec(node->right, min->player, depth + 1);
            } else if (node->left) {
                KDNode* min = findMin(node->left, cd, depth + 1);
                node->player = min->player;
                node->right = deleteRec(node->left, min->player, depth + 1);
                node->left = nullptr;
            } else {
                delete node;
                return nullptr;
            }
            return node;
        }

        if (p.features[cd] < node->player.features[cd])
            node->left = deleteRec(node->left, p, depth + 1);
        else
            node->right = deleteRec(node->right, p, depth + 1);

        return node;
    }

    void nearestRec(KDNode* node, const Player& target, KDNode*& best, float& bestDist, int depth) {
        if (!node) return;

        float d = distance(target, node->player);
        if (d < bestDist) {
            bestDist = d;
            best = node;
        }

        int cd = depth % K;
        KDNode* next = (target.features[cd] < node->player.features[cd]) ? node->left : node->right;
        KDNode* other = (next == node->left) ? node->right : node->left;

        nearestRec(next, target, best, bestDist, depth + 1);

        if (fabs(target.features[cd] - node->player.features[cd]) < bestDist)
            nearestRec(other, target, best, bestDist, depth + 1);
    }

    float distance(const Player& a, const Player& b) {
        float dist = 0.0;
        for (int i = 0; i < K; i++) {
            float diff = a.features[i] - b.features[i];
            dist += diff * diff;
        }
        return sqrt(dist);
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

    void remove(const Player& p) {
        root = deleteRec(root, p, 0);
    }

    Player nearest(const Player& target) {
        KDNode* best = nullptr;
        float bestDist = numeric_limits<float>::max();
        nearestRec(root, target, best, bestDist, 0);
        return best ? best->player : Player{};
    }

    void printAll() {
        inorder(root);
    }
};

float safe_stof(const std::string& s) {
    try {
        return std::stof(s);
    } catch (...) {
        return 0.0f;
    }
}

Player parseCSVLine(const string& line) {
    stringstream ss(line);
    string token;
    Player p;
    getline(ss, token, ',');
    p.player_fifa_api_id = stoi(token);

    // Pula 3 colunas categóricas: preferred_foot, attacking_work_rate, defensive_work_rate
    getline(ss, token, ','); // overall_rating
    p.features[0] = safe_stof(token);
    getline(ss, token, ','); // potential
    p.features[1] = safe_stof(token);

    for (int skip = 0; skip < 3; skip++) getline(ss, token, ','); // skip preferred_foot, att_wr, def_wr

    int i = 2;
    while (getline(ss, token, ',') && i < K) {
        p.features[i++] = safe_stof(token);
    }
    return p;
}

int main() {
    KDTree tree;
    ifstream file("dataset_limpo3.csv");
    string line;
    getline(file, line); // Ignora cabeçalho

    while (getline(file, line)) {
        Player p = parseCSVLine(line);
        tree.insert(p);
    }

    int opcao;
    do {
        cout << "\nMenu:\n1. Inserir jogador\n2. Remover jogador\n3. Buscar similar\n4. Listar todos\n5. Sair\nOpcao: ";
        cin >> opcao;
        if (opcao == 1) {
            Player p;
            cout << "ID: "; cin >> p.player_fifa_api_id;
            cout << "Digite os 31 atributos: ";
            for (int i = 0; i < K; i++) cin >> p.features[i];
            tree.insert(p);
        } else if (opcao == 2) {
            Player p;
            cout << "ID do jogador a remover: "; cin >> p.player_fifa_api_id;
            cout << "Digite os 31 atributos: ";
            for (int i = 0; i < K; i++) cin >> p.features[i];
            tree.remove(p);
        } else if (opcao == 3) {
            Player p;
            cout << "Digite os 31 atributos do jogador-alvo: ";
            for (int i = 0; i < K; i++) cin >> p.features[i];
            Player similar = tree.nearest(p);
            cout << "Jogador mais parecido:\n";
            similar.print();
        } else if (opcao == 4) {
            tree.printAll();
        }
    } while (opcao != 5);

    return 0;
}
