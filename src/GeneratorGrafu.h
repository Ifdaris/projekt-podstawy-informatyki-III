#pragma once

#include <numeric>
#include <algorithm>
#include <random>
#include <vector>

#include "Wierzcholek.h"

struct Krawedz
{
    int u;
    int v;
};

inline void generujGraf(const int n, const int k,
                        std::vector<Wierzcholek>& graf,
                        std::vector<sf::Vector2f>& pozycjeWierzcholkow)
{
    graf.clear();
    pozycjeWierzcholkow.clear();

    graf.reserve(n);
    pozycjeWierzcholkow.reserve(n);

    std::random_device rd;
    std::mt19937 g(rd());

    for (int i = 0; i < n; i++)
    {
        graf.emplace_back(i, 10.0, 10.0);
        pozycjeWierzcholkow.emplace_back(10.0, 10.0);
    }

    std::vector<std::vector<int>> macierzPomocnicza(n, std::vector<int>(n, 0));

    std::vector<int> permutacja(n);
    std::iota(permutacja.begin(), permutacja.end(), 0);
    std::shuffle(permutacja.begin(), permutacja.end(), g);

    for (int i = 0; i < n - 1; i++)
    {
        int u = permutacja[i];
        int v = permutacja[i + 1];

        macierzPomocnicza[u][v] = 1;
        macierzPomocnicza[v][u] = 1;

        graf[u].dodajSasiada(v);
        graf[v].dodajSasiada(u);
    }

    int pozostaleKrawedzie = k - (n - 1);

    std::vector<Krawedz> E;
    E.reserve(n * (n - 1) / 2);
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            E.push_back({i, j});
        }
    }

    while (pozostaleKrawedzie > 0 && !E.empty())
    {
        std::uniform_int_distribution<int> distE(0, E.size() - 1);
        int idX = distE(g);
        Krawedz wylosowana = E[idX];

        if (macierzPomocnicza[wylosowana.u][wylosowana.v] == 1)
        {
            E[idX] = E.back();
            E.pop_back();
            continue;
        }

        macierzPomocnicza[wylosowana.u][wylosowana.v] = 1;
        macierzPomocnicza[wylosowana.v][wylosowana.u] = 1;

        graf[wylosowana.u].dodajSasiada(wylosowana.v);
        graf[wylosowana.v].dodajSasiada(wylosowana.u);

        E[idX] = E.back();
        E.pop_back();
        pozostaleKrawedzie--;
    }
}
