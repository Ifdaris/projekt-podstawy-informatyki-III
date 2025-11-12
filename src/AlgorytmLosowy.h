#pragma once
#include <vector>
#include "Wierzcholek.h"
#include <algorithm>
#include <stack>
#include <random>

using namespace std;

vector<int> algorytmlosowy(vector<Wierzcholek>& graf, int start_index = 0, unsigned int seed = 0) {
    int n = graf.size();
    if (n == 0) return {};

    vector<bool> visited(n, false);
    vector<int> res;

    stack<pair<int, int>> s;
    vector<int> parent(n, -1);

    vector<vector<int>> shuffled(n);
    vector<char> prepared(n, 0);

    std::mt19937 gen;
    if (seed == 0) {
        std::random_device rd;
        gen = std::mt19937(rd());
    } else {
        gen = std::mt19937(seed);
    }

    s.push({start_index, 0});
    parent[start_index] = -1;

    while (!s.empty()) {
        int node = s.top().first;
        int &neighbour_index = s.top().second;

        if (!visited[node]) {
            visited[node] = true;
            res.push_back(node);
        }

        if (!prepared[node]) {
            shuffled[node] = graf[node].pobierzSasiadow();
            shuffle(shuffled[node].begin(), shuffled[node].end(), gen);
            prepared[node] = 1;
        }

        bool found_unvisited = false;
        while (neighbour_index < (int)shuffled[node].size()) {
            int neighbor = shuffled[node][neighbour_index];
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

    return res;

}