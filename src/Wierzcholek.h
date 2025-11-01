#include <iostream>
#include <vector>

class Wierzcholek {
    int idWierzcholka;
    std::vector<int> sasiedzi;

  public:
    Wierzcholek(int _id) : idWierzcholka(_id) {}

    void dodajSasiada(int sasiadId) { sasiedzi.push_back(sasiadId); }

    void wyswietlDane() {
        std::cout << "Identyfikator wierzcholka: " << idWierzcholka
                  << std::endl;
        std::cout << " Lista sasiadow: " << std::endl;
        for (int sasiad : sasiedzi) {
            std::cout << "  - " << sasiad << std::endl;
        }
    }

    std::vector<int> pobierzSasiadow() { return sasiedzi; }
};