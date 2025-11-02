#pragma once

#include <iostream>
#include <vector>

class Wierzcholek {
    int idWierzcholka;
    int pozycjaX;
    int pozycjaY;
    std::vector<int> sasiedzi;

  public:
    Wierzcholek(int _id, int _pozycjaX, int _pozycjaY) : idWierzcholka(_id), pozycjaX(_pozycjaX), pozycjaY(_pozycjaY) {}

    void dodajSasiada(int sasiadId) { sasiedzi.push_back(sasiadId); }
    
    const std::vector<int> pobierzSasiadow() { return sasiedzi; }

    void dodajPozycje(int x, int y) {
        pozycjaX = x;
        pozycjaY = y;
    }

    void wyswietlDane() {
        std::cout << "Identyfikator wierzcholka: " << idWierzcholka
                  << std::endl;
        std::cout << " Pozycja X: " << pozycjaX << std::endl;
        std::cout << " Pozycja Y: " << pozycjaY << std::endl;
        std::cout << " Lista sasiadow: " << std::endl;
        for (int sasiad : sasiedzi) {
            std::cout << "  - " << sasiad << std::endl;
        }
    }

};