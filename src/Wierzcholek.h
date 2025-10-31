#include <iostream>
#include <vector>

class Wierzcholek {
    int id;
    std::vector<int> sasiedzi;

public:
    Wierzcholek(int _id): id(_id) {}

    void dodajSasiada(int sasiadId) {
        sasiedzi.push_back(sasiadId);
    }

    void wyswietlDane() {
        std::cout << "Identyfikator wierzcholka: " << id << std::endl;
        std::cout << " Lista sasiadow: " << std::endl;
        for (int sasiad : sasiedzi) {
            std::cout << "  - " << sasiad << std::endl;
        }
    }

};