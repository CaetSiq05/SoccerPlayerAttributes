#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>


//Definição da estrutura
using namespace std;

struct Node {
    int player_id;
    float overall_rating;
    Node* left;
    Node* right;
    int height;

    Node(int id, float rating) : player_id(id), overall_rating(rating), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
public:
    Node* root;

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

    Node* insert(Node* node, int id, float rating) {
        if (!node) return new Node(id, rating);

        if (rating < node->overall_rating)
            node->left = insert(node->left, id, rating);
        else
            node->right = insert(node->right, id, rating);

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);

        if (balance > 1 && rating < node->left->overall_rating)
            return rightRotate(node);
        if (balance < -1 && rating >= node->right->overall_rating)
            return leftRotate(node);
        if (balance > 1 && rating >= node->left->overall_rating) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        if (balance < -1 && rating < node->right->overall_rating) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    void searchRange(Node* node, float min_rating, float max_rating, vector<pair<int, float>>& results) {
        if (!node) return;

        if (min_rating < node->overall_rating)
            searchRange(node->left, min_rating, max_rating, results);
        if (node->overall_rating >= min_rating && node->overall_rating <= max_rating)
            results.push_back({ node->player_id, node->overall_rating });
        if (max_rating > node->overall_rating)
            searchRange(node->right, min_rating, max_rating, results);
    }

    void inorder(Node* node, vector<pair<int, float>>& results) {
        if (!node) return;
        inorder(node->left, results);
        results.push_back({ node->player_id, node->overall_rating });
        inorder(node->right, results);
    }
};

//Gerenciador de jogadores
class GerenciadorJogadoresAVL {
private:
    AVLTree tree;
    unordered_map<int, map<string, string>> hash_table;

public:
    GerenciadorJogadoresAVL(const string& caminho_csv) {
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
            map<string, string> jogador;
            int player_id = 0;
            float rating = 0.0;
            for (int i = 0; i < colunas.size(); ++i) {
                getline(ss, valor, ',');
                jogador[colunas[i]] = valor;
                if (colunas[i] == "player_fifa_api_id") player_id = stoi(valor);
                if (colunas[i] == "overall_rating") rating = stof(valor);
            }

            tree.root = tree.insert(tree.root, player_id, rating);
            hash_table[player_id] = jogador;
        }

        file.close();
    }

    void buscarPorId(int id) {
        if (hash_table.find(id) != hash_table.end()) {
            auto& j = hash_table[id];
            cout << "ID: " << j["player_fifa_api_id"] << " | Rating: " << j["overall_rating"]
                 << " | Pé: " << j["preferred_foot"] << " | Potencial: " << j["potential"]
                 << " | Ataque: " << j["attacking_work_rate"] << " | Defesa: " << j["defensive_work_rate"]
                 << " | Aceleração: " << j["acceleration"] << endl;
        } else {
            cout << "Jogador não encontrado!\n";
        }
    }

    void buscarPorRating(float min_rating, float max_rating) {
        vector<pair<int, float>> resultados;
        tree.searchRange(tree.root, min_rating, max_rating, resultados);
        sort(resultados.begin(), resultados.end(), [](auto& a, auto& b) {
            return a.second > b.second;
        });

        cout << resultados.size() << " jogadores encontrados:\n";
        for (int i = 0; i < min((int)resultados.size(), 10); ++i) {
            int id = resultados[i].first;
            auto& j = hash_table[id];
            cout << "ID: " << j["player_fifa_api_id"] << " | Rating: " << j["overall_rating"]
                 << " | Pé: " << j["preferred_foot"] << " | Potencial: " << j["potential"]
                 << " | Ataque: " << j["attacking_work_rate"] << " | Defesa: " << j["defensive_work_rate"]
                 << " | Aceleração: " << j["acceleration"] << endl;
        }
    }

    void listarTodosOrdenados() {
        vector<pair<int, float>> resultados;
        tree.inorder(tree.root, resultados);
        reverse(resultados.begin(), resultados.end());

        cout << "Top 20 jogadores:\n";
        for (int i = 0; i < min((int)resultados.size(), 20); ++i) {
            int id = resultados[i].first;
            auto& j = hash_table[id];
            cout << "ID: " << j["player_fifa_api_id"] << " | Rating: " << j["overall_rating"]
                 << " | Pé: " << j["preferred_foot"] << " | Potencial: " << j["potential"]
                 << " | Ataque: " << j["attacking_work_rate"] << " | Defesa: " << j["defensive_work_rate"]
                 << " | Aceleração: " << j["acceleration"] << endl;
        }
    }
};

//Menu
void menu_avl() {
    string caminho = "dataset_limpo3.csv";  // Atualize se necessário
    GerenciadorJogadoresAVL gerenciador(caminho);

    while (true) {
        cout << "\n=== GERENCIADOR DE JOGADORES (AVL) ===\n";
        cout << "1. Buscar jogador por ID\n";
        cout << "2. Buscar jogadores por faixa de rating\n";
        cout << "3. Listar todos os jogadores ordenados por rating\n";
        cout << "4. Sair\n";

        string opcao;
        cout << "Escolha uma opção: ";
        cin >> opcao;

        if (opcao == "1") {
            int id;
            cout << "Digite o ID do jogador: ";
            cin >> id;
            gerenciador.buscarPorId(id);
        } else if (opcao == "2") {
            float min_rating, max_rating;
            cout << "Rating mínimo (0-100): ";
            cin >> min_rating;
            cout << "Rating máximo (0-100): ";
            cin >> max_rating;
            gerenciador.buscarPorRating(min_rating, max_rating);
        } else if (opcao == "3") {
            gerenciador.listarTodosOrdenados();
        } else if (opcao == "4") {
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