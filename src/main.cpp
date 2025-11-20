#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <time.h>
#include <vector>

#include "AlgorytmLosowy.h"
#include "DFS.h"
#include "DaneAgenta.h"
#include "Wierzcholek.h"
#include "bonk.h"

int main() {
    srand(time(NULL));

    std::string line;

    std::ifstream plik("res/graf2.txt");
    std::vector<Wierzcholek> graf;
    std::vector<sf::Vector2f> pozycjeWierzcholkow;
    std::stringstream ss;

    int idWierzcholka;
    int idSasiada;
    float pozycjaX;
    float pozycjaY;

    // Wczytywanie grafu z pliku
    while (getline(plik, line)) {
        ss.clear();
        ss.str(line);
        ss >> idWierzcholka >> pozycjaX >> pozycjaY;

        Wierzcholek wierzcholek(idWierzcholka, pozycjaX, pozycjaY);
        pozycjeWierzcholkow.push_back(sf::Vector2f(pozycjaX, pozycjaY));
        while (ss >> idSasiada) {
            wierzcholek.dodajSasiada(idSasiada);
        }
        graf.push_back(wierzcholek);
        ss.clear();
    }
    plik.close();

    // Tworzenie agentow
    int iloscAgentowDFS = 4;
    int iloscAgentowLosowych = 3;
    std::vector<DaneAgenta> agenci;

    int idAgentow = 0;

    vector<sf::Color> kolory = {sf::Color::Red, sf::Color::Blue, sf::Color::Black, sf::Color::Cyan, sf::Color::Green, sf::Color::Magenta, sf::Color::Yellow};
    size_t iterKolorow = 0;
    std::unordered_set<int> wierzcholkiPoczatkowe;
    int poczatkowyWierzcholek = rand() % graf.size();

    // Inicjalizowanie agentow
    for (int i = 0; i < iloscAgentowDFS; i++, idAgentow++) {
        while (wierzcholkiPoczatkowe.find(poczatkowyWierzcholek) != wierzcholkiPoczatkowe.end()) {
            poczatkowyWierzcholek = rand() % graf.size();
        }
        // Animacja
        sf::Vector2f startPos = pozycjeWierzcholkow[poczatkowyWierzcholek];
        agenci.emplace_back(idAgentow, poczatkowyWierzcholek, kolory[iterKolorow], dfs(graf, poczatkowyWierzcholek), rand() % 100, startPos);
        //--------

        wierzcholkiPoczatkowe.insert(poczatkowyWierzcholek);
        iterKolorow++;
    }

    for (int i = 0; i < iloscAgentowLosowych; i++, idAgentow++) {
        while (wierzcholkiPoczatkowe.find(poczatkowyWierzcholek) != wierzcholkiPoczatkowe.end()) {
            poczatkowyWierzcholek = rand() % graf.size();
        }
        // Animacja
        sf::Vector2f startPos = pozycjeWierzcholkow[poczatkowyWierzcholek];
        agenci.emplace_back(idAgentow, poczatkowyWierzcholek, kolory[iterKolorow], algorytmlosowy(graf, poczatkowyWierzcholek), rand() % 100, startPos);

        wierzcholkiPoczatkowe.insert(poczatkowyWierzcholek);
        iterKolorow++;
    }

    // Dodanie efektow dzwiekowych
    sf::Music bonk("res/bonk.mp3");
    sf::Music YodaDeath("res/YodaDeath.mp3");
    sf::Music jackpot("res/jackpot.mp3");

    sf::RenderWindow window(sf::VideoMode({1000, 600}), "Graf");
    window.setFramerateLimit(0);

    // Wizualizacja wierzchołków
    std::vector<sf::CircleShape> wezly;
    for (const sf::Vector2f &pos : pozycjeWierzcholkow) {
        sf::CircleShape shape(10.f);
        shape.setFillColor(sf::Color::White);
        shape.setOrigin({shape.getRadius(), shape.getRadius()});
        shape.setPosition(pos);
        wezly.push_back(shape);
    }

    sf::VertexArray krawedzie(sf::PrimitiveType::Lines);

    const sf::Color kolorLinii(100, 100, 100);
    for (int i = 0; i < graf.size(); ++i) {
        sf::Vector2f pozycjaStartowa = pozycjeWierzcholkow[i];

        for (int sasiadId : graf[i].pobierzSasiadow()) {
            if (i < sasiadId) {
                sf::Vector2f pozycjaKoncowa = pozycjeWierzcholkow[sasiadId];

                krawedzie.append(sf::Vertex{pozycjaStartowa, kolorLinii});
                krawedzie.append(sf::Vertex{pozycjaKoncowa, kolorLinii});
            }
        }
    }

    bool czySkonczone = false;
    bool czyTura = false;
    vector<size_t> przegrani;
    int idZwyciezcy = -1;
    float licznikCzasu = 0.f;
    int czyJackpot = -1;

    sf::Clock deltaClock;

    while (window.isOpen()) {

        sf::Time dtTime = deltaClock.restart();
        float dt = dtTime.asSeconds();

        bool czyKtosSieRusza = false;
        for (auto &a : agenci) {
            if (a.czyZywy == false)
                continue;

            a.aktualizujAnimacje(dt); // To przesuwa kółka
            if (a.wTrakcieRuchu)
                czyKtosSieRusza = true;
        }

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {

                if (czyKtosSieRusza)
                    continue;
                if (czySkonczone == false) {

                    if (czyTura == false) {

                        if (keyPressed->code == sf::Keyboard::Key::Right) {

                            for (int i = 0; i < agenci.size(); i++) {
                                if (agenci[i].czyZywy == false)
                                    continue;

                                if (!agenci[i].kolejka.empty()) {

                                    if (agenci[i].odwiedzone.size() < graf.size()) {
                                        // Animacja
                                        int idCelu = agenci[i].kolejka.front();

                                        agenci[i].agent.przemiescAgenta(agenci[i].kolejka.front());
                                        agenci[i].odwiedzone.insert(agenci[i].kolejka.front());
                                        agenci[i].poprzednieKroki.emplace_back(agenci[i].kolejka.front());
                                        agenci[i].kolejka.pop_front();

                                        agenci[i].idzDo(pozycjeWierzcholkow[idCelu]);
                                    }
                                    if (agenci[i].odwiedzone.size() == graf.size() - 1) {
                                        (czySkonczone = true);
                                        idZwyciezcy = i;
                                    }
                                }
                            }
                            std::vector<std::vector<int>> agenciNaWierzcholku(graf.size());

                            for (int i = 0; i < agenci.size(); i++) {
                                if (agenci[i].czyZywy == false)
                                    continue;

                                agenciNaWierzcholku[agenci[i].agent.pozycjaAgenta()].push_back(i);
                            }

                            for (int i = 0; i < agenciNaWierzcholku.size(); i++) {
                                if (agenciNaWierzcholku[i].size() > 1) {
                                    bonk.play();
                                    int najwiekszaMoc = -1;
                                    int idNajsilniejszego = agenciNaWierzcholku[i][0];

                                    for (int j : agenciNaWierzcholku[i]) {
                                        if (agenci[j].agent.moc > najwiekszaMoc) {
                                            najwiekszaMoc = agenci[j].agent.moc;
                                            idNajsilniejszego = j;
                                        }
                                    }

                                    for (int idPrzegranego : agenciNaWierzcholku[i]) {
                                        if (idPrzegranego != idNajsilniejszego) {
                                            przegrani.emplace_back(idPrzegranego);
                                        }
                                    }

                                    czyTura = true;
                                }
                            }
                            agenciNaWierzcholku.clear();
                        }
                    }
                    // Animacja
                    else if (czyTura && !czyKtosSieRusza) {
                        czyTura = false;

                        for (size_t i : przegrani) {
                            bool doZabicia = false;
                            if (agenci[i].czyZywy == false)
                                continue;

                            if (agenci[i].poprzednieKroki.size() >= 2) {
                                int idCeluCofniecia = agenci[i].poprzednieKroki[agenci[i].poprzednieKroki.size() - 2];

                                for (auto &j : agenci) {
                                    if (j.czyZywy == false)
                                        continue;
                                    if (j.agent.pozycjaAgenta() == idCeluCofniecia)
                                        doZabicia = true;
                                }

                                if (doZabicia) {
                                    agenci[i].czyUmiera = true;
                                    YodaDeath.play();
                                    agenci[i].smierc();
                                } else {
                                    agenci[i].cofnij(pozycjeWierzcholkow[idCeluCofniecia]);
                                }
                            }
                        }
                        przegrani.clear();
                    }
                    if (keyPressed->code == sf::Keyboard::Key::Left) {
                    }
                }
            }
        }
        licznikCzasu += dt;

        if (czySkonczone == 1 && idZwyciezcy != -1) {
            
            if (agenci[idZwyciezcy].wTrakcieRuchu == false) {
                agenci[idZwyciezcy].zmienRozmiar(50.f);

                if (czyJackpot == -1){
                    jackpot.play();
                    czyJackpot = 1;
                }

                if (licznikCzasu <= 0.5f) {
                    agenci[idZwyciezcy].koloAgenta.setFillColor(sf::Color::Red);
                } else if (licznikCzasu <= 1.f) {
                    agenci[idZwyciezcy].koloAgenta.setFillColor(sf::Color::Green);
                } else if (licznikCzasu <= 1.5f) {
                    agenci[idZwyciezcy].koloAgenta.setFillColor(sf::Color::Blue);
                } else licznikCzasu = 0.f;
            }
        }
        // Kolor backgroundu
        window.clear(sf::Color::Black);

        // Rysuj w kolejności:
        window.draw(krawedzie);
        for (const auto &wezel : wezly) {
            window.draw(wezel);
        }

        for (auto &i : agenci) {
            if (i.czyZywy == false)
                continue;

            window.draw(i.koloAgenta);
        }

        window.display();
    }
}