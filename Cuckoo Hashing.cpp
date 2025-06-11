#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include <random>

using namespace std;

const int MAX_REHASHES = 32;  // número máximo de realocações antes de rehash

struct Player {
    int player_fifa_api_id;
    float overall_rating;
    float potential;
    string preferred_foot;
    string attacking_work_rate;
    string defensive_work_rate;

    Player() : player_fifa_api_id(0), overall_rating(0), potential(0) {}

    Player(int id, float rating, float pot, string foot, string att, string def)
        : player_fifa_api_id(id), overall_rating(rating), potential(pot),
          preferred_foot(foot), attacking_work_rate(att), defensive_work_rate(def) {}
};

class CuckooHashTable {
private:
    struct Entry {
        bool occupied = false;
        int key;
        Player value;
    };

    vector<Entry> table1, table2;
    size_t capacity;
    size_t totalCollisions;
    size_t rehashCount;
    hash<int> hasher;
    mt19937 rng;

    size_t hash1(int key) {
        return hasher(key) % capacity;
    }

    size_t hash2(int key) {
        return (hasher(key * 31)) % capacity;
    }

    void rehash() {
        vector<Entry> old1 = table1;
        vector<Entry> old2 = table2;
        capacity *= 2;
        table1.assign(capacity, Entry());
        table2.assign(capacity, Entry());
        totalCollisions = 0;
        rehashCount++;

        for (const auto& entry : old1) {
            if (entry.occupied) insert(entry.key, entry.value);
        }
        for (const auto& entry : old2) {
            if (entry.occupied) insert(entry.key, entry.value);
        }
    }

public:
    CuckooHashTable(size_t init_capacity = 101) : capacity(init_capacity), totalCollisions(0), rehashCount(0), rng(random_device{}()) {
        table1.resize(capacity);
        table2.resize(capacity);
    }

    bool insert(int key, const Player& player) {
        if (search(key)) return false;

        Player currentValue = player;
        int currentKey = key;

        for (int i = 0; i < MAX_REHASHES; ++i) {
            size_t idx1 = hash1(currentKey);
            if (!table1[idx1].occupied) {
                table1[idx1] = { true, currentKey, currentValue };
                return true;
            }
            totalCollisions++;
            swap(currentKey, table1[idx1].key);
            swap(currentValue, table1[idx1].value);

            size_t idx2 = hash2(currentKey);
            if (!table2[idx2].occupied) {
                table2[idx2] = { true, currentKey, currentValue };
                return true;
            }
            totalCollisions++;
            swap(currentKey, table2[idx2].key);
            swap(currentValue, table2[idx2].value);
        }

        rehash();
        return insert(currentKey, currentValue); // tenta inserir novamente após rehash
    }

    Player* search(int key) {
        size_t idx1 = hash1(key);
        if (table1[idx1].occupied && table1[idx1].key == key)
            return &table1[idx1].value;

        size_t idx2 = hash2(key);
        if (table2[idx2].occupied && table2[idx2].key == key)
            return &table2[idx2].value;

        return nullptr;
    }

    bool remove(int key) {
        size_t idx1 = hash1(key);
        if (table1[idx1].occupied && table1[idx1].key == key) {
            table1[idx1].occupied = false;
            return true;
        }

        size_t idx2 = hash2(key);
        if (table2[idx2].occupied && table2[idx2].key == key) {
            table2[idx2].occupied = false;
            return true;
        }

        return false;
    }

    void displayStats() {
        int count = 0;
        for (const auto& e : table1)
            if (e.occupied) count++;
        for (const auto& e : table2)
            if (e.occupied) count++;

        cout << "Capacidade total (duas tabelas): " << 2 * capacity << endl;
        cout << "Elementos armazenados: " << count << endl;
        cout << "Fator de carga: " << (double)count / (2 * capacity) << endl;
        cout << "Total de colisões: " << totalCollisions << endl;
        cout << "Número de rehashes: " << rehashCount << endl;
    }
};

int main() {
    CuckooHashTable tabela;

    // Exemplo de inserção
    Player p1(101, 75.0f, 85.0f, "right", "medium", "high");
    Player p2(202, 80.0f, 90.0f, "left", "high", "medium");
    Player p3(303, 82.0f, 88.0f, "right", "high", "high");
    Player p4(404, 78.0f, 86.0f, "left", "low", "medium");

    tabela.insert(p1.player_fifa_api_id, p1);
    tabela.insert(p2.player_fifa_api_id, p2);
    tabela.insert(p3.player_fifa_api_id, p3);
    tabela.insert(p4.player_fifa_api_id, p4);

    if (auto* p = tabela.search(101)) {
        cout << "Jogador encontrado: ID " << p->player_fifa_api_id << ", Rating: " << p->overall_rating << endl;
    }

    tabela.remove(202);
    tabela.displayStats();

    return 0;
}
