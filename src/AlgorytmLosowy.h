#pragma once
#include <vector>
#include "Wierzcholek.h"
#include <algorithm>
#include <stack>
#include <random>

using namespace std;

// Zmiana: Jeśli to możliwe, używaj const Wierzcholek& dla grafu
deque<int> algorytmlosowy(vector<Wierzcholek>& graf, int start_index, std::mt19937& gen) {
    int n = graf.size();
    if (n == 0) return {};

    // 💡 OPTYMALIZACJA 1: Deklaracje bez zmian (są minimalne)
    vector<bool> visited(n, false);
    deque<int> res;
    stack<pair<int, int>> s;
    vector<int> parent(n, -1);

    // OPTYMALIZACJA 2: Tablica musi być lokalna i alokowana (niestety, bo jest tasowana)
    vector<vector<int>> shuffled_neighbors(n); 

    s.push({start_index, 0});
    parent[start_index] = -1;

    while (!s.empty()) {
        int node = s.top().first;
        int &neighbour_index = s.top().second;

        if (!visited[node]) {
            visited[node] = true;
            res.push_back(node);
        }

        // OPTYMALIZACJA 3: Tasowanie listy sąsiadów raz na wierzchołek
        if (shuffled_neighbors[node].empty()) {
            
            // 💡 KLUCZOWA POPRAWKA: Jeśli pobierzSasiadow() zwraca kopię, 
            // musimy ją skopiować. Poniższa wersja jest czytelna i minimalizuje kopiowanie.
            shuffled_neighbors[node] = graf[node].pobierzSasiadow();
            
            // Tasowanie raz
            shuffle(shuffled_neighbors[node].begin(), shuffled_neighbors[node].end(), gen);
        }

        bool found_unvisited = false;
        const vector<int>& current_neighbors = shuffled_neighbors[node];

        while (neighbour_index < (int)current_neighbors.size()) {
            int neighbor = current_neighbors[neighbour_index];
            neighbour_index++;

            if (!visited[neighbor]) {
                s.push({neighbor, 0});
                parent[neighbor] = node;
                found_unvisited = true;
                break;
            }
        }

        if (!found_unvisited) {
            s.pop();
            if (!s.empty()) {
                res.push_back(s.top().first);
            }
        }
    }
    res.erase(res.begin());
    
    // ZWROT PRZEZ KOPIĘ jest konieczny, ale można by użyć std::move (jeśli to deque) 
    // dla małych zysków, jeśli kolejka jest duża, ale to zależy od kompilatora.
    return res;
}