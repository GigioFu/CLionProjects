/*
 * Alunos: Geovane Otávio Marques Rodrigues e Luann Ribeiro Costa
 */

#include <iostream>
#include <vector>
#include <limits>

using namespace std;

// Estrutura para representar uma aresta ponderada
struct Aresta {
    int origem, destino;
    float peso;
};

// Função de comparação para ordenar as arestas por peso
bool compararArestas(const Aresta& a, const Aresta& b) {
    return a.peso < b.peso;
}

// Classe União-Busca para verificar a existência de ciclos
class UniaoBusca {
private:
    vector<int> pai, tamanho;

public:
    UniaoBusca(int n) {
        pai.resize(n);
        tamanho.resize(n, 1);
        for (int i = 0; i < n; i++) {
            pai[i] = i;
        }
    }

    int encontrar(int x) {
        if (pai[x] == x) {
            return x;
        }
        return pai[x] = encontrar(pai[x]);
    }

    void unir(int x, int y) {
        int raizX = encontrar(x);
        int raizY = encontrar(y);
        if (raizX != raizY) {
            if (tamanho[raizX] < tamanho[raizY]) {
                pai[raizX] = raizY;
                tamanho[raizY] += tamanho[raizX];
            } else {
                pai[raizY] = raizX;
                tamanho[raizX] += tamanho[raizY];
            }
        }
    }
};

// Função para encontrar a árvore geradora mínima usando o algoritmo de Kruskal
vector<Aresta> kruskal(vector<Aresta>& arestas, int numVertices) {
    // Ordena as arestas por peso
    sort(arestas.begin(), arestas.end(), compararArestas);

    vector<Aresta> arvoreGeradora;
    UniaoBusca uf(numVertices);

    for (const Aresta& aresta : arestas) {
        int origem = aresta.origem;
        int destino = aresta.destino;

        // Verifica se a adição da aresta cria um ciclo
        if (uf.encontrar(origem) != uf.encontrar(destino)) {
            arvoreGeradora.push_back(aresta);
            uf.unir(origem, destino);
        }
    }

    return arvoreGeradora;
}

// Função para encontrar os caminhos mais curtos usando o algoritmo de Johnson
vector<vector<float>> johnson(vector<Aresta>& arestas, int numVertices) {
    // Adiciona um vértice fictício com arestas de peso zero para todos os outros vértices
    int numVerticesFicticios = numVertices + 1;
    int verticeFicticio = numVertices;

    for (int i = 0; i < numVertices; i++) {
        arestas.push_back({ verticeFicticio, i, 0.0f });
    }

    // Executa o algoritmo de Bellman-Ford para encontrar os caminhos mais curtos
    vector<float> distancias(numVerticesFicticios, numeric_limits<float>::max());
    distancias[verticeFicticio] = 0.0f;

    for (int i = 0; i < numVerticesFicticios - 1; i++) {
        for (const Aresta& aresta : arestas) {
            int u = aresta.origem;
            int v = aresta.destino;
            float peso = aresta.peso;

            if (distancias[u] != numeric_limits<float>::max() && distancias[u] + peso < distancias[v]) {
                distancias[v] = distancias[u] + peso;
            }
        }
    }

    // Verifica se há ciclos de peso negativo
    for (const Aresta& aresta : arestas) {
        int u = aresta.origem;
        int v = aresta.destino;
        float peso = aresta.peso;

        if (distancias[u] != numeric_limits<float>::max() && distancias[u] + peso < distancias[v]) {
            // Há um ciclo de peso negativo, então não é possível encontrar caminhos mais curtos
            return {};
        }
    }

    // Remove o vértice fictício e as arestas adicionadas
    arestas.resize(arestas.size() - numVertices);

    // Ajusta os pesos das arestas usando a diferença das distâncias encontradas pelo Bellman-Ford
    for (Aresta& aresta : arestas) {
        aresta.peso += distancias[aresta.origem] - distancias[aresta.destino];
    }

    // Executa o algoritmo de Dijkstra para encontrar os caminhos mais curtos a partir de cada vértice
    vector<vector<float>> caminhosMaisCurto(numVertices, vector<float>(numVertices, numeric_limits<float>::max()));

    for (int origem = 0; origem < numVertices; origem++) {
        vector<float> distancia(numVertices, numeric_limits<float>::max());
        distancia[origem] = 0.0f;

        vector<bool> visitado(numVertices, false);

        for (int i = 0; i < numVertices; i++) {
            int u = -1;

            // Encontra o vértice não visitado com menor distância
            for (int j = 0; j < numVertices; j++) {
                if (!visitado[j] && (u == -1 || distancia[j] < distancia[u])) {
                    u = j;
                }
            }

            visitado[u] = true;

            // Atualiza as distâncias dos vértices adjacentes
            for (const Aresta& aresta : arestas) {
                int v = aresta.destino;
                float peso = aresta.peso;

                if (u == aresta.origem && distancia[u] != numeric_limits<float>::max() && distancia[u] + peso < distancia[v]) {
                    distancia[v] = distancia[u] + peso;
                }
            }
        }

        // Ajusta as distâncias encontradas pelo algoritmo de Dijkstra
        for (int destino = 0; destino < numVertices; destino++) {
            caminhosMaisCurto[origem][destino] = distancia[destino] + distancias[destino] - distancias[origem];
        }
    }

    return caminhosMaisCurto;
}

int main() {
    int numVertices, numArestas;
    cout << "Digite o numero de vertices: ";
    cin >> numVertices;
    cout << "Digite o numero de arestas: ";
    cin >> numArestas;

    vector<Aresta> arestas(numArestas);
    cout << "Digite as arestas no formato 'origem destino peso':\n";
    for (int i = 0; i < numArestas; i++) {
        cin >> arestas[i].origem >> arestas[i].destino >> arestas[i].peso;
    }

    vector<Aresta> arvoreGeradora = kruskal(arestas, numVertices);
    vector<vector<float>> caminhosMaisCurto = johnson(arestas, numVertices);

    cout << "Arestas da arvore geradora minima:\n";
    for (const Aresta& aresta : arvoreGeradora) {
        cout << aresta.origem << " - " << aresta.destino << " : " << aresta.peso << endl;
    }

    cout << "Caminhos mais curtos:\n";
    for (int i = 0; i < numVertices; i++) {
        for (int j = 0; j < numVertices; j++) {
            cout << "Caminho de " << i << " para " << j << ": ";
            if (caminhosMaisCurto[i][j] == numeric_limits<float>::max()) {
                cout << "Nao existe caminho." << endl;
            } else {
                cout << caminhosMaisCurto[i][j] << endl;
            }
        }
    }

    return 0;
}