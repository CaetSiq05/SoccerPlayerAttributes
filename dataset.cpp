#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

int main() {
    string arquivo = "C:/Users/Eve/Desktop/Repositorio ETD/SoccerPlayerAttributes/dataset_limpo.csv";
    ifstream fin(arquivo);

    if (!fin.is_open()) {
        cout << "Erro ao abrir o arquivo." << endl;
        return 1;
    }

    string linha;
    vector<string> cabecalho;
    vector<vector<string>> dados;

    // Ler cabeçalho
    if (getline(fin, linha)) {
        stringstream ss(linha);
        string coluna;
        while (getline(ss, coluna, ',')) {
            cabecalho.push_back(coluna);
        }
    }

    // Ler os dados
    while (getline(fin, linha)) {
        stringstream ss(linha);
        string valor;
        vector<string> linhaDados;

        while (getline(ss, valor, ',')) {
            linhaDados.push_back(valor);
        }

        // Garante que a linha tem o mesmo tamanho do cabeçalho
        if (!linhaDados.empty()) {
            while (linhaDados.size() < cabecalho.size()) {
                linhaDados.push_back("");
            }
            dados.push_back(linhaDados);
        }
    }

    fin.close();

    // 🏗️ Definir largura da coluna
    const int largura = 15;

    // 📄 Imprimir cabeçalho
    cout << endl;
    for (const auto& coluna : cabecalho) {
        cout << setw(largura) << left << coluna;
    }
    cout << endl;

    // 🔗 Linha separadora
    cout << string(largura * cabecalho.size(), '-') << endl;

    // 📊 Imprimir dados (limitar para não ficar gigante)
    int linhas_para_imprimir = min(10, (int)dados.size());  // Mostra até 10 linhas

    for (int i = 0; i < linhas_para_imprimir; i++) {
        for (const auto& valor : dados[i]) {
            cout << setw(largura) << left << valor;
        }
        cout << endl;
    }

    return 0;
}

}
