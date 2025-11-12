#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Agent.h"
#include "AlgorytmLosowy.h"
#include "DFS.h"
#include "Wierzcholek.h"
#include "bonk.h"

int main() {
    srand(unsigned int(time(NULL)));

    std::string line;

    std::ifstream plik("res/graf.txt");
    std::vector<Wierzcholek> graf;
    std::vector<sf::Vector2f> pozycjeWierzcholkow;
    std::stringstream ss;

    int idWierzcholka;
    int idSasiada;
    int pozycjaX;
    int pozycjaY;

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
    Agent a1(0, 0);
    Agent a2(1, 17);

    // Dodanie efektow dzwiekowych
    sf::Music bonk("res/bonk.mp3");

    sf::RenderWindow window(sf::VideoMode({1000, 600}), "Graf");
    window.setFramerateLimit(1000);

    // Wizualizacja wierzchołków
    std::vector<sf::CircleShape> wezly;
    for (const sf::Vector2f &pos : pozycjeWierzcholkow) {
        sf::CircleShape shape(10.f);
        shape.setFillColor(sf::Color::White);
        shape.setOrigin({shape.getRadius(), shape.getRadius()});
        shape.setPosition(pos);
        wezly.push_back(shape);
    }

    // Wizualizacja agentów
    sf::CircleShape agentCircle1(8.f);
    agentCircle1.setFillColor(sf::Color::Red);
    agentCircle1.setOrigin({agentCircle1.getRadius(), agentCircle1.getRadius()});

    sf::CircleShape agentCircle2(8.f);
    agentCircle2.setFillColor(sf::Color::Blue);
    agentCircle2.setOrigin(
        {agentCircle2.getRadius(), agentCircle2.getRadius()});

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

    std::deque<int> order1 = dfs(graf, a1.pozycjaAgenta());
    std::deque<int> order2 = dfs(graf, a2.pozycjaAgenta());

    order1.erase(order1.begin());
    order2.erase(order2.begin());

    std::deque<int> const backupOrder = order1;
    std::deque<int> const backupOrder2 = order2;

    std::set<int> odwiedzone1;
    std::set<int> odwiedzone2;

    std::deque<int> poprzednieKroki1;
    std::deque<int> poprzednieKroki2;

    poprzednieKroki1.emplace_back(a1.pozycjaAgenta());
    poprzednieKroki2.emplace_back(a2.pozycjaAgenta());

    a1.moc = 50;
    a2.moc = 75;

    std::vector<int> sasiedzi1;
    std::vector<int> sasiedzi2;

    std::vector<int> dostepniSasiedzi1;
    std::vector<int> dostepniSasiedzi2;

    int nastepnyKrok1;
    int nastepnyKrok2;

    bool czySkonczone = false;
    BonkDane daneBonka;

    while (window.isOpen()) {

        // Ustawienie wizualizacji agenta na jego pozycje
        agentCircle1.setPosition(pozycjeWierzcholkow[a1.pozycjaAgenta()]);
        agentCircle2.setPosition(pozycjeWierzcholkow[a2.pozycjaAgenta()]);

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {

                if (daneBonka.aktywny)
                    continue;
                if (czySkonczone == false) {

                    if (daneBonka.tura == 0) {

                        if (keyPressed->code == sf::Keyboard::Key::Right) {

                            if (!order1.empty() && !order2.empty()) {

                                if (odwiedzone1.size() < graf.size()) {
                                    a1.przemiescAgenta(order1.front());
                                    odwiedzone1.insert(order1.front());
                                }
                                if (odwiedzone1.size() == graf.size() - 1) {
                                    (czySkonczone = true);
                                }
                                if (odwiedzone2.size() < graf.size()) {
                                    a2.przemiescAgenta(order2.front());
                                    odwiedzone2.insert(order2.front());
                                }
                                if (odwiedzone2.size() == graf.size() - 1) {
                                    (czySkonczone = true);
                                }
                            }

                            nastepnyKrok1 = order1.front();
                            nastepnyKrok2 = order2.front();

                            if (a1.pozycjaAgenta() == a2.pozycjaAgenta()) {
                                bonk.play();
                                daneBonka.aktywny = true;
                                daneBonka.czyRazem = true;
                                daneBonka.ktoWygra = (a1.moc > a2.moc ? 1 : 2);

                            } else if (!poprzednieKroki1.empty() && !poprzednieKroki2.empty() && nastepnyKrok1 == poprzednieKroki2.back() && nastepnyKrok2 == poprzednieKroki1.back()) {
                                bonk.play();
                                daneBonka.aktywny = true;
                                daneBonka.czyRazem = false;
                                daneBonka.ktoWygra = (a1.moc > a2.moc ? 1 : 2);
                            }

                            if (odwiedzone1.size() < graf.size() || odwiedzone2.size() < graf.size()) {
                                order1.erase(order1.begin());
                                order2.erase(order2.begin());
                            }

                            poprzednieKroki1.emplace_back(a1.pozycjaAgenta());
                            poprzednieKroki2.emplace_back(a2.pozycjaAgenta());

                            agentCircle2.setFillColor(sf::Color::Blue);

                            agentCircle1.setPosition(pozycjeWierzcholkow[a1.pozycjaAgenta()]);
                            agentCircle2.setPosition(pozycjeWierzcholkow[a2.pozycjaAgenta()]);

                            window.clear(sf::Color::Black);
                            window.draw(krawedzie);
                            for (const auto &wezel : wezly)
                                window.draw(wezel);
                            window.draw(agentCircle1);
                            window.draw(agentCircle2);
                            window.display();
                        }

                    } else if (daneBonka.ktoWygra == 1) {
                        agentCircle2.setFillColor(sf::Color::Blue);
                        a2.przemiescAgenta(poprzednieKroki1.back());
                        poprzednieKroki2.pop_back();
                    } else {
                        agentCircle2.setFillColor(sf::Color::Blue);
                        order1.emplace_front(a1.pozycjaAgenta());
                        a1.przemiescAgenta(poprzednieKroki1.back());
                        daneBonka.tura = 0;
                    }
                }
                if (keyPressed->code == sf::Keyboard::Key::Left) {

                    agentCircle2.setFillColor(sf::Color::Blue);

                    a1.przemiescAgenta(0);
                    order1 = backupOrder;
                    odwiedzone1.clear();

                    poprzednieKroki1.clear();
                    poprzednieKroki2.clear();

                    a2.przemiescAgenta(17);
                    order2 = backupOrder2;
                    odwiedzone2.clear();

                    czySkonczone = false;
                }
            }

            // Kolor backgroundu
            window.clear(sf::Color::Black);

            // Rysuj w kolejności:
            window.draw(krawedzie);
            for (const auto &wezel : wezly) {
                window.draw(wezel);
            }

            window.draw(agentCircle1);
            window.draw(agentCircle2);

            window.display();
        }

        if (daneBonka.aktywny) {
            wykonajBonk(a1, a2, order1, order2, poprzednieKroki1, poprzednieKroki2, odwiedzone1, odwiedzone2, bonk, graf, czySkonczone, agentCircle1, agentCircle2, daneBonka);

            daneBonka.aktywny = false;
            daneBonka.czyRazem = false;
            continue;
        }
    }
}