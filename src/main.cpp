#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "wierzcholek.h"


int main() {
    std::string line;
    std::ifstream plik("D:/Programming/cmake-sfml-project/res/graf.txt");
    std::vector<Wierzcholek> graf;
    std::stringstream ss;

    int idWierzcholka;
    int idSasiada;

    while (getline(plik, line)){
        ss.clear();

        ss.str(line);
        
        ss >> idWierzcholka;
        Wierzcholek wierzcholek(idWierzcholka);
        while (ss >> idSasiada) {
            wierzcholek.dodajSasiada(idSasiada);
        }
        graf.push_back(wierzcholek);
        ss.clear();
    }

    for (Wierzcholek wierzcholek : graf) {
        wierzcholek.wyswietlDane();
    }

    return 0;
}