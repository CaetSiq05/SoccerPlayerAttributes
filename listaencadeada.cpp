#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

// Estrutura do nó da lista encadeada
struct Jogador {
    int player_fifa_api_id;
    float overall_rating;
    float potential;
    string preferred_foot;
    string attacking_work_rate;
    string defensive_work_rate;
    float crossing;
    float finishing;
    float heading_accuracy;
    float short_passing;
    float volleys;
    float dribbling;
    float curve;
    float free_kick_accuracy;
    float long_passing;
    float ball_control;
    float acceleration;
    float sprint_speed;
    float agility;
    float reactions;
    float balance;
    float shot_power;
    float jumping;
    float stamina;
    float strength;
    float long_shots;
    float aggression;
    float interceptions;
    float positioning;
    float vision;
    float penalties;
    float marking;
    float standing_tackle;
    float sliding_tackle;

    Jogador* proximo;
};

// Função para dividir uma string
vector<string> split(const string& linha, char delimitador) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(linha);

    while (getline(tokenStream, token, delimitador)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Função para criar um nó jogador
Jogador* criarJogador(const vector<string>& dados) {
    if (dados.size() < 34) {
        cout << "Linha ignorada (dados insuficientes)" << endl;
        return nullptr;
    }

    Jogador* novo = new Jogador;

    novo->player_fifa_api_id = stoi(dados[0]);
    novo->overall_rating = stof(dados[1]);
    novo->potential = stof(dados[2]);
    novo->preferred_foot = dados[3];
    novo->attacking_work_rate = dados[4];
    novo->defensive_work_rate = dados[5];
    novo->crossing = stof(dados[6]);
    novo->finishing = stof(dados[7]);
    novo->heading_accuracy = stof(dados[8]);
    novo->short_passing = stof(dados[9]);
    novo->volleys = stof(dados[10]);
    novo->dribbling = stof(dados[11]);
    novo->curve = stof(dados[12]);
    novo->free_kick_accuracy = stof(dados[13]);
    novo->long_passing = stof(dados[14]);
    novo->ball_control = stof(dados[15]);
    novo->acceleration = stof(dados[16]);
    novo->sprint_speed = stof(dados[17]);
    novo->agility = stof(dados[18]);
    novo->reactions = stof(dados[19]);
    novo->balance = stof(dados[20]);
    novo->shot_power = stof(dados[21]);
    novo->jumping = stof(dados[22]);
    novo->stamina = stof(dados[23]);
    novo->strength = stof(dados[24]);
    novo->long_shots = stof(dados[25]);
    novo->aggression = stof(dados[26]);
    novo->interceptions = stof(dados[27]);
    novo->positioning = stof(dados[28]);
    novo->vision = stof(dados[29]);
    novo->penalties = stof(dados[30]);
    novo->marking = stof(dados[31]);
    novo->standing_tackle = stof(dados[32]);
    novo->sliding_tackle = stof(dados[33]);

    novo->proximo = nullptr;

    return novo;
}

// Ler CSV e criar lista encadeada
Jogador* lerCSV(const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cout << "Erro ao abrir o arquivo CSV." << endl;
        return nullptr;
    }

    string linha;
    getline(arquivo, linha); // Ignorar cabeçalho

    Jogador* inicio = nullptr;
    Jogador* atual = nullptr;

    while (getline(arquivo, linha)) {
        vector<string> dados = split(linha, ',');

        Jogador* novo = criarJogador(dados);
        if (novo != nullptr) {
            if (inicio == nullptr) {
                inicio = novo;
                atual = novo;
            } else {
                atual->proximo = novo;
                atual = novo;
            }
        }
    }

    arquivo.close();
    return inicio;
}

// Imprimir lista de jogadores
void imprimirLista(Jogador* inicio) {
    Jogador* atual = inicio;
    while (atual != nullptr) {
        cout << "ID: " << atual->player_fifa_api_id
             << " | Overall: " << atual->overall_rating
             << " | Potential: " << atual->potential
             << " | Foot: " << atual->preferred_foot
             << " | Finishing: " << atual->finishing
             << " | Curve: " << atual->curve
             << " | Penalties: " << atual->penalties
             << endl;
        atual = atual->proximo;
    }
}

// Buscar jogador por ID
Jogador* buscarJogador(Jogador* inicio, int id) {
    Jogador* atual = inicio;
    while (atual != nullptr) {
        if (atual->player_fifa_api_id == id) {
            return atual;
        }
        atual = atual->proximo;
    }
    return nullptr;
}

// Adicionar novo jogador manualmente
Jogador* adicionarJogador(Jogador* inicio) {
    Jogador* novo = new Jogador;

    cout << "Digite o ID do jogador: ";
    cin >> novo->player_fifa_api_id;
    cout << "Overall: ";
    cin >> novo->overall_rating;
    cout << "Potential: ";
    cin >> novo->potential;
    cout << "Preferred foot (right/left): ";
    cin >> novo->preferred_foot;
    cout << "Attacking work rate: ";
    cin >> novo->attacking_work_rate;
    cout << "Defensive work rate: ";
    cin >> novo->defensive_work_rate;
    cout << "Finishing: ";
    cin >> novo->finishing;
    cout << "Curve: ";
    cin >> novo->curve;
    cout << "Penalties: ";
    cin >> novo->penalties;

    // O resto dos atributos você pode seguir este mesmo modelo
    // Para simplificar, vamos deixar alguns valores padrão
    novo->crossing = 50;
    novo->heading_accuracy = 50;
    novo->short_passing = 50;
    novo->volleys = 50;
    novo->dribbling = 50;
    novo->free_kick_accuracy = 50;
    novo->long_passing = 50;
    novo->ball_control = 50;
    novo->acceleration = 50;
    novo->sprint_speed = 50;
    novo->agility = 50;
    novo->reactions = 50;
    novo->balance = 50;
    novo->shot_power = 50;
    novo->jumping = 50;
    novo->stamina = 50;
    novo->strength = 50;
    novo->long_shots = 50;
    novo->aggression = 50;
    novo->interceptions = 50;
    novo->positioning = 50;
    novo->vision = 50;
    novo->marking = 50;
    novo->standing_tackle = 50;
    novo->sliding_tackle = 50;

    novo->proximo = inicio;
    return novo;
}

// Remover jogador por ID
Jogador* removerJogador(Jogador* inicio, int id) {
    if (inicio == nullptr) return nullptr;

    if (inicio->player_fifa_api_id == id) {
        Jogador* temp = inicio;
        inicio = inicio->proximo;
        delete temp;
        cout << "Jogador removido com sucesso!" << endl;
        return inicio;
    }

    Jogador* atual = inicio;
    while (atual->proximo != nullptr) {
        if (atual->proximo->player_fifa_api_id == id) {
            Jogador* temp = atual->proximo;
            atual->proximo = temp->proximo;
            delete temp;
            cout << "Jogador removido com sucesso!" << endl;
            return inicio;
        }
        atual = atual->proximo;
    }

    cout << "Jogador nao encontrado." << endl;
    return inicio;
}

// Liberar memória
void liberarLista(Jogador* inicio) {
    while (inicio != nullptr) {
        Jogador* temp = inicio;
        inicio = inicio->proximo;
        delete temp;
    }
}

int main() {
    string nomeArquivo = "dataset_limpo3.csv";
    Jogador* lista = lerCSV(nomeArquivo);

    int opcao;
    do {
        cout << "\n--- MENU ---\n";
        cout << "1. Adicionar jogador\n";
        cout << "2. Buscar jogador por ID\n";
        cout << "3. Remover jogador\n";
        cout << "4. Imprimir todos os jogadores\n";
        cout << "5. Sair\n";
        cout << "Escolha uma opcao: ";
        cin >> opcao;

        switch (opcao) {
            case 1:
                lista = adicionarJogador(lista);
                break;
            case 2: {
                int id;
                cout << "Digite o ID do jogador a buscar: ";
                cin >> id;
                Jogador* encontrado = buscarJogador(lista, id);
                if (encontrado != nullptr) {
                    cout << "Jogador encontrado: ID " << encontrado->player_fifa_api_id
                         << " | Overall: " << encontrado->overall_rating
                         << " | Foot: " << encontrado->preferred_foot << endl;
                } else {
                    cout << "Jogador nao encontrado.\n";
                }
                break;
            }
            case 3: {
                int id;
                cout << "Digite o ID do jogador a remover: ";
                cin >> id;
                lista = removerJogador(lista, id);
                break;
            }
            case 4:
                imprimirLista(lista);
                break;
            case 5:
                cout << "Encerrando programa...\n";
                liberarLista(lista);
                break;
            default:
                cout << "Opcao invalida!\n";
        }
    } while (opcao != 5);

    return 0;
}
