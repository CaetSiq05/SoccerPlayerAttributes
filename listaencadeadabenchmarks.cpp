#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono> //biblioteca medir tempo de execucao
#include <random> //biblioteca sortear aleatorio (tempo medio de acesso)
#include <cmath> // para sqrt

using namespace std;
using namespace chrono;

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
pair<Jogador*, double> adicionarJogador(Jogador* inicio) {
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

    auto start = chrono::high_resolution_clock::now(); //mede tempo inicial
    novo->proximo = inicio;
    auto end = chrono::high_resolution_clock::now();
    double tempo = chrono::duration<double, milli>(end - start).count();//calcula tempo total
    return make_pair(novo, tempo);

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

// contar nos
int contarNos(Jogador* lista) {
    int count = 0;
    while (lista != nullptr) {
        count++;
        lista = lista->proximo;
    }
    return count;
}

//Medir Tempo Médio de Acesso
vector<int> coletarIDs(Jogador* lista) {
    vector<int> ids;
    while (lista != nullptr) {
        ids.push_back(lista->player_fifa_api_id);
        lista = lista->proximo;
    }
    return ids;
}

void medirTempoMedioAcesso(Jogador* lista, int numBuscas) {
    // Coleta todos os IDs válidos da lista
    vector<int> ids = coletarIDs(lista);

    // Sorteador aleatório
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, ids.size() - 1);

    double somaTempos = 0;

    for (int i = 0; i < numBuscas; i++) {
        int idAleatorio = ids[dis(gen)]; // sorteia um ID válido

        auto start = chrono::high_resolution_clock::now();
        buscarJogador(lista, idAleatorio);
        auto end = chrono::high_resolution_clock::now();

        double tempo = chrono::duration<double, micro>(end - start).count();
        somaTempos += tempo;
    }

    double media = somaTempos / numBuscas;
    cout << "Tempo médio de acesso: " << media << " µs" << endl;
}

//

Jogador* lerCSVLimitado(const string& nomeArquivo, int maxLinhas) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cout << "Erro ao abrir o arquivo CSV." << endl;
        return nullptr;
    }

    string linha;
    getline(arquivo, linha); // Ignora cabeçalho

    Jogador* inicio = nullptr;
    Jogador* atual = nullptr;

    int count = 0;
    while (getline(arquivo, linha) && count < maxLinhas) {
        vector<string> dados = split(linha, ',');
        Jogador* novo = criarJogador(dados);
        if (novo != nullptr) {
            if (!inicio) {
                inicio = novo;
                atual = novo;
            } else {
                atual->proximo = novo;
                atual = novo;
            }
            count++;
        }
    }

    arquivo.close();
    return inicio;
}

//Escalabilidade e latencia
void testarEscalabilidadeELatencia(const string& nomeArquivo) {
    vector<int> tamanhos = {1000, 5000, 10000, 20000};

    for (int tamanho : tamanhos) {
        cout << "\n🔍 Testando com " << tamanho << " jogadores..." << endl;

        Jogador* lista = lerCSVLimitado(nomeArquivo, tamanho);
        vector<int> ids = coletarIDs(lista);

        if (ids.empty()) {
            cout << "Lista vazia. Pulando..." << endl;
            continue;
        }

        // Inicializa aleatoriedade
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, ids.size() - 1);

        double somaBusca = 0;
        double somaLatencia = 0;

        for (int i = 0; i < 100; i++) {
            int id = ids[dis(gen)];

            // -------- Tempo de busca (Escalabilidade)
            auto startBusca = chrono::high_resolution_clock::now();
            buscarJogador(lista, id);
            auto endBusca = chrono::high_resolution_clock::now();
            somaBusca += chrono::duration<double, micro>(endBusca - startBusca).count();

            // -------- Tempo de Latência (inserir + buscar + remover)
            Jogador* novo = new Jogador;
            novo->player_fifa_api_id = 999999 + i;
            novo->overall_rating = 70;
            novo->potential = 80;
            novo->preferred_foot = "right";
            novo->attacking_work_rate = "high";
            novo->defensive_work_rate = "medium";
            novo->finishing = novo->curve = novo->penalties = 50;
            novo->proximo = nullptr;

            auto startLatencia = chrono::high_resolution_clock::now();

            // Inserção no início
            novo->proximo = lista;
            lista = novo;

            // Busca
            buscarJogador(lista, novo->player_fifa_api_id);

            // Remoção
            lista = removerJogador(lista, novo->player_fifa_api_id);

            auto endLatencia = chrono::high_resolution_clock::now();
            somaLatencia += chrono::duration<double, micro>(endLatencia - startLatencia).count();
        }

        double mediaBusca = somaBusca / 100.0;
        double mediaLatencia = somaLatencia / 100.0;

        cout << "📈 Tempo médio de busca: " << mediaBusca << " µs" << endl;
        cout << "⏱️ Latência média (inserção + busca + remoção): " << mediaLatencia << " µs" << endl;

        liberarLista(lista);
    }
}


//Cálculo estatístico sobre os dados
void estatisticasLista(Jogador* lista) {
    int count = 0;
    float soma = 0.0, somaQuadrados = 0.0;

    Jogador* atual = lista;
    while (atual != nullptr) {
        float rating = atual->overall_rating;
        soma += rating;
        somaQuadrados += rating * rating;
        count++;
        atual = atual->proximo;
    }

    if (count == 0) {
        cout << "Lista vazia.\n";
        return;
    }

    float media = soma / count;
    float variancia = (somaQuadrados / count) - (media * media);
    float desvioPadrao = sqrt(variancia);

    cout << "Média do Overall Rating: " << media << endl;
    cout << "Desvio padrão: " << desvioPadrao << endl;
}

//agrupamento
void agruparPorPreferredFoot(Jogador* inicio) {
    Jogador* atual = inicio;
    cout << "\n--- Jogadores com pé preferido: Right ---\n";
    while (atual != nullptr) {
        if (atual->preferred_foot == "right")
            cout << "ID: " << atual->player_fifa_api_id << " | Overall: " << atual->overall_rating << endl;
        atual = atual->proximo;
    }

    atual = inicio;
    cout << "\n--- Jogadores com pé preferido: Left ---\n";
    while (atual != nullptr) {
        if (atual->preferred_foot == "left")
            cout << "ID: " << atual->player_fifa_api_id << " | Overall: " << atual->overall_rating << endl;
        atual = atual->proximo;
    }
}

//filtragem jogadores com >overall
void filtrarPorOverall(Jogador* inicio, float minimo) {
    Jogador* atual = inicio;
    cout << "\n--- Jogadores com Overall >= " << minimo << " ---\n";
    while (atual != nullptr) {
        if (atual->overall_rating >= minimo)
            cout << "ID: " << atual->player_fifa_api_id << " | Overall: " << atual->overall_rating << endl;
        atual = atual->proximo;
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
        cout << "5. Uso de memoria\n";
        cout << "6. Tempo medio de Acesso\n";
        cout << "7. Escalabilidade e latência média\n";
        cout << "8. Cálculo estatístico sobre os dados\n";
        cout << "9. Agrupar por PreferredFoot\n";
        cout << "10. Filtragem por Overall\n";
        cout << "11. Sair\n";
        cout << "Escolha uma opcao: ";
        cin >> opcao;

        switch (opcao) {
            case 1:{
               
                pair<Jogador*, double> resultado = adicionarJogador(lista);
                lista = resultado.first;
                double tempo = resultado.second;

                cout << "Tempo de inserção: " << tempo << " ms" << endl;
            break;
            
            }
            case 2: {
                int id;
                cout << "Digite o ID do jogador a buscar: ";
                cin >> id;

                auto start = chrono::high_resolution_clock::now();
                Jogador* encontrado = buscarJogador(lista, id);
                auto end = chrono::high_resolution_clock::now();

                if (encontrado != nullptr) {
                    cout << "Jogador encontrado: ID " << encontrado->player_fifa_api_id
                         << " | Overall: " << encontrado->overall_rating
                         << " | Foot: " << encontrado->preferred_foot << endl;

                        double tempoBusca = chrono::duration<double, micro>(end - start).count();
                        cout << "Tempo de Busca: " << tempoBusca << " µs" << endl;

                } else {
                    cout << "Jogador nao encontrado.\n";
                    double tempoBusca = chrono::duration<double, micro>(end - start).count();
                    cout << "Tempo de remoção: " << tempoBusca << " µs" << endl;
                }
                break;
            }
            case 3: {
                int id;
                cout << "Digite o ID do jogador a remover: ";
                cin >> id;
                auto start = high_resolution_clock::now();
                lista = removerJogador(lista, id);
                auto end = high_resolution_clock::now();
                double tempoRemocao = duration<double, micro>(end - start).count(); // tempo em microssegundos
                cout << "Tempo de remoção: " << tempoRemocao << " µs" << endl;
                break;
            }
            case 4:
                imprimirLista(lista);
                break;

            case 5: {
                int totalNos = contarNos(lista);
                size_t tamanhoNo = sizeof(Jogador); // tamanho em bytes de cada nó

                size_t totalBytes = totalNos * tamanhoNo;
                double totalKB = totalBytes / 1024.0;

                cout << "Total de nós: " << totalNos << endl;
                cout << "Uso estimado de memória: " << totalKB << " KB" << endl;
                break; }

            case 6: {   
                medirTempoMedioAcesso(lista, 100); // 100 buscas aleatórias
                break;
                }

            case 7: {
                testarEscalabilidadeELatencia("dataset_limpo3.csv");
                break;
                }

            case 8:{
                estatisticasLista(lista);
                break;
                }

            case 9:{
                agruparPorPreferredFoot(lista);
                break;}

            case 10:{
                float minimo;
                cout << "Digite o mínimo de overall: ";
                cin >> minimo;
                filtrarPorOverall(lista, minimo);
                    break;}

            case 11:{
                cout << "Encerrando programa...\n";
                liberarLista(lista);
                break;}

            default:
                cout << "Opcao invalida!\n";
            }
    } while (opcao != 11);

    return 0;
}
