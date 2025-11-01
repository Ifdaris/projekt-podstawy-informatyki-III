#include <iostream>
#include <vector>

class Agent {
    int idAgenta;
    int idAktualnegoWierzcholka;

  public:
    Agent(int _idAgenta, int _idAktualnegoWierzcholka)
        : idAgenta(_idAgenta),
          idAktualnegoWierzcholka(_idAktualnegoWierzcholka) {}

    void przemiescAgenta( int pozycja) {
        idAktualnegoWierzcholka = pozycja;
        std::cout << "Przesunieto na wierzcholek: " << idAktualnegoWierzcholka << std::endl;
    }

    int pozycjaAgenta() {
        std::cout << "Pozycja agenta: " << idAktualnegoWierzcholka << std::endl;
        return idAktualnegoWierzcholka;
    }
};