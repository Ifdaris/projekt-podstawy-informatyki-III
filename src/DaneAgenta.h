// W pliku DaneAgenta.h
#pragma once
#include <SFML/Graphics.hpp>
#include <cmath> // Do matematyki wektorów
#include <deque>
#include <set>

#include "Agent.h"

class DaneAgenta {
  public:
    // ... (Twoje dotychczasowe pola: kolejka, odwiedzone itd.) ...
    std::deque<int> kolejka;
    std::deque<int> zapasowaKolejka;
    std::set<int> odwiedzone;
    std::deque<int> poprzednieKroki;
    sf::CircleShape koloAgenta;
    Agent agent;
    bool czyZywy = true;
    bool czyUmiera = false;

    // --- NOWE POLA DO ANIMACJI ---
    sf::Vector2f startAnimacji; // Skąd ruszam
    sf::Vector2f celAnimacji;   // Dokąd idę
    float postepAnimacji;       // Od 0.0 do 1.0
    bool wTrakcieRuchu;         // Czy się ruszam?
    float predkoscRuchu;        // Jak szybko (jednostki na sekundę)
    float docelowypromien;

  public:
    // Zaktualizowany konstruktor
    DaneAgenta(int _idAgenta, int _idAktualnegoWierzcholka, sf::Color _kolorAgenta, std::deque<int> _kolejka, int mocAgenta, sf::Vector2f pozycjaStartowaWizualna)
        : agent(_idAgenta, _idAktualnegoWierzcholka) {
        // ... (Twoja inicjalizacja) ...
        koloAgenta.setRadius(8.f);
        docelowypromien = 8.f;
        koloAgenta.setFillColor(_kolorAgenta);
        koloAgenta.setOrigin({koloAgenta.getRadius(), koloAgenta.getRadius()});

        // Ustaw pozycję startową
        koloAgenta.setPosition(pozycjaStartowaWizualna);

        // Inicjalizacja zmiennych animacji
        startAnimacji = pozycjaStartowaWizualna;
        celAnimacji = pozycjaStartowaWizualna;
        postepAnimacji = 0.0f;
        wTrakcieRuchu = false;
        predkoscRuchu = 5.0f; // Szybkość animacji

        kolejka = _kolejka;
        zapasowaKolejka = _kolejka;
        poprzednieKroki.emplace_back(agent.pozycjaAgenta());
        agent.moc = mocAgenta;
    }

    DaneAgenta &operator=(const DaneAgenta &other) {
        this->agent = other.agent;
        this->kolejka = other.kolejka;
        this->zapasowaKolejka = other.zapasowaKolejka;
        this->odwiedzone = other.odwiedzone;
        this->poprzednieKroki = other.poprzednieKroki;
        this->koloAgenta = other.koloAgenta;
        this->czyZywy = other.czyZywy;
        this->czyUmiera = other.czyUmiera;
        this->startAnimacji = other.startAnimacji;
        this->celAnimacji = other.celAnimacji;
        this->postepAnimacji = other.postepAnimacji;
        this->wTrakcieRuchu = other.wTrakcieRuchu;
        this->predkoscRuchu = other.predkoscRuchu;

        return *this;
    }

    void zmienRozmiar(float nowyRozmiar) {
        docelowypromien = nowyRozmiar;
    }

    // Nowa metoda: Rozpocznij ruch do punktu
    void idzDo(sf::Vector2f nowyCel) {
        startAnimacji = koloAgenta.getPosition(); // Startujemy stąd, gdzie jesteśmy
        celAnimacji = nowyCel;
        postepAnimacji = 0.0f;
        wTrakcieRuchu = true;
    }

    // Nowa metoda: Aktualizuj pozycję (wywoływana w każdej klatce)
    void aktualizujAnimacje(float dt) {
        if (wTrakcieRuchu) {

            postepAnimacji += predkoscRuchu * dt;

            if (postepAnimacji >= 1.0f) {
                postepAnimacji = 1.0f;
                wTrakcieRuchu = false;
                koloAgenta.setPosition(celAnimacji); // Dociągnij do celu

                if (czyUmiera)
                    czyZywy = false; // Doleciał do celu (1000,0), więc teraz jest martwy

            } else {
                // Interpolacja Liniowa (Lerp)
                // Wzór: Start + (Cel - Start) * postep
                sf::Vector2f nowaPozycja = startAnimacji + (celAnimacji - startAnimacji) * postepAnimacji;
                koloAgenta.setPosition(nowaPozycja);
            }
        }

        float obecnyPromien = koloAgenta.getRadius();

        if (std::abs(docelowypromien - obecnyPromien) > 0.1f) {
            float nowyPromien = obecnyPromien + (docelowypromien - obecnyPromien) * 5.f * dt;
            koloAgenta.setRadius(nowyPromien);
            koloAgenta.setOrigin({nowyPromien, nowyPromien});
        }
    }

    void cofnij(sf::Vector2f pozycjaCofniecia) { // Zmienione, by przyjmować pozycję
        kolejka.emplace_front(agent.pozycjaAgenta());
        agent.przemiescAgenta(poprzednieKroki.back());
        poprzednieKroki.pop_back();

        // Tu też uruchamiamy animację!
        idzDo(pozycjaCofniecia);
    }

    void smierc() {
        startAnimacji = koloAgenta.getPosition(); // Startujemy stąd, gdzie jesteśmy
        celAnimacji = sf::Vector2f(1000, 0);
        postepAnimacji = 0.0f;
        wTrakcieRuchu = true;

        idzDo(celAnimacji);
    }
};