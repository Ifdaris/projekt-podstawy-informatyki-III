#pragma once
#include <deque>
#include "Wierzcholek.h"
#include <algorithm>
#include <stack>
#include <vector>

using namespace std;

// Zmiana: Jeśli to możliwe, używaj const Wierzcholek&
deque<int> dfs(vector<Wierzcholek>& graf, int start_index = 0) {
    int n = graf.size();
    if (n == 0) return {};

    vector<bool> visited(n, false);
    deque<int> res;
    stack<pair<int, int>> s;
    vector<int> parent(n, -1);

    s.push({start_index, 0});
    parent[start_index] = -1;

    // OPTYMALIZACJA 1: Tablica do przechowywania posortowanych sąsiadów
    // Musimy sortować tylko raz na wierzchołek.
    vector<vector<int>> sorted_neighbors(n);
    
    // OPTYMALIZACJA 2: Wypełnij i posortuj sąsiadów PRZED rozpoczęciem DFS.
    // Dzięki temu unikamy wielokrotnego sortowania w pętli while.
    for (int i = 0; i < n; ++i) {
        // Zakładam, że graf[i].pobierzSasiadow() zwraca kopię lub const&
        sorted_neighbors[i] = graf[i].pobierzSasiadow(); 
        sort(sorted_neighbors[i].begin(), sorted_neighbors[i].end());
    }


    while (!s.empty()) {
        int node = s.top().first;
        int &neighbor_index = s.top().second;

        if (!visited[node]) {
            visited[node] = true;
            res.push_back(node);
        }

        // --- ZMIANA: Używamy posortowanej i już gotowej listy ---
        const vector<int>& neighbors = sorted_neighbors[node]; 

        // Znajdź następnego nieodwiedzonego sąsiada
        bool found_unvisited = false;
        while (neighbor_index < neighbors.size()) {
            int neighbor = neighbors[neighbor_index];
            neighbor_index++;

            if (!visited[neighbor]) {
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
                // Cofanie się jest częścią trasy DFS, więc dodajemy wierzchołek
                res.push_back(s.top().first);
            }
        }
    }
    // Usunięcie elementu początkowego, który został dodany tylko do zainicjowania
    res.erase(res.begin());
    return res;
}