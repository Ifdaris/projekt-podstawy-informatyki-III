#pragma once

#include <iostream>
#include <vector>

class Agent {
    int idAgenta;
    int idAktualnegoWierzcholka;

  public:
    int moc;
    Agent(int _idAgenta, int _idAktualnegoWierzcholka)
        : idAgenta(_idAgenta),
          idAktualnegoWierzcholka(_idAktualnegoWierzcholka) {}

    void przemiescAgenta(int pozycja) { idAktualnegoWierzcholka = pozycja; }
    int pozycjaAgenta() { return idAktualnegoWierzcholka; }
};