#pragma once
#include <vector>
#include "Wierzcholek.h"
#include <algorithm>
#include <stack>

using namespace std;

vector<int> dfs(vector<Wierzcholek>& graf, int start_index = 0) {
    int n = graf.size();
    if (n == 0) return {};

    vector<bool> visited(n, false);
    vector<int> res;

    // Stos przechowuje pary (wierzchołek, indeks następnego sąsiada do sprawdzenia)
    stack<pair<int, int>> s;
    vector<int> parent(n, -1);

    s.push({start_index, 0});
    parent[start_index] = -1;

    //cout << "Rozpoczynam od wierzcholka " << start_index << endl;

    while (!s.empty()) {
        int node = s.top().first;
        int &neighbor_index = s.top().second;

        if (!visited[node]) {
            // Pierwsze odwiedzenie tego wierzchołka
            visited[node] = true;
            res.push_back(node);

            if (parent[node] != -1) {
                //cout << "Przechodze z " << parent[node] << " do " << node << endl;
            }
        }

        // Pobierz listę sąsiadów
        vector<int> neighbors = graf[node].pobierzSasiadow();
        // Sortuj sąsiadów w kolejności rosnącej
        sort(neighbors.begin(), neighbors.end());

        // Znajdź następnego nieodwiedzonego sąsiada
        bool found_unvisited = false;
        while (neighbor_index < neighbors.size()) {
            int neighbor = neighbors[neighbor_index];
            neighbor_index++;

            if (!visited[neighbor]) {
                // Znaleziono nieodwiedzonego sąsiada - przechodzimy do niego
                s.push({neighbor, 0});
                parent[neighbor] = node;
                found_unvisited = true;
                break;
            }
        }

        if (!found_unvisited) {
            // Brak nieodwiedzonych sąsiadów - cofamy się
            s.pop();
            if (!s.empty()) {
                //cout << "Cofam sie z " << node << " do " << s.top().first << endl;
            }
        }
    }

    return res;
}
