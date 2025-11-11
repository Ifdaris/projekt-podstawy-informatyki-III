#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Agent.h"
#include "DFS_2.h"
#include "Wierzcholek.h"

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
    sf::CircleShape agentCircle(8.f);
    agentCircle.setFillColor(sf::Color::Red);
    agentCircle.setOrigin({agentCircle.getRadius(), agentCircle.getRadius()});

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

    std::vector<int> order = dfs(graf, a1.pozycjaAgenta());
    std::vector<int> order2 = dfs(graf, a2.pozycjaAgenta());

    std::vector<int> backupOrder = order;
    std::vector<int> backupOrder2 = order2;

    std::set<int> odwiedzone;
    std::set<int> odwiedzone2;

    std::vector<int> poprzedniKrok;
    std::vector<int> poprzedniKrok2;

    a1.moc = 50;
    a2.moc = 75;

    sf::Vector2f pozycjaStartowa;
    sf::Vector2f pozycjaStartowa2;
    sf::Vector2f pozycjaKoncowa;
    sf::Vector2f pozycjaKoncowa2;

    std::vector<int> sasiedzi;
    std::vector<int> sasiedzi2;

    int przepchniecie;
    int przepchniecie2;

    bool czySkonczone = false;

    while (window.isOpen()) {

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto *keyPressed =
                    event->getIf<sf::Event::KeyPressed>()) {

                if (czySkonczone == false) {

                    if (keyPressed->code == sf::Keyboard::Key::Right) {

                        poprzedniKrok.emplace_back(a1.pozycjaAgenta());
                        poprzedniKrok2.emplace_back(a2.pozycjaAgenta());

                        pozycjaStartowa =
                            graf[a1.pobierzIdAktualnegoWierzcholka()]
                                .pozycjaWierzcholka();
                        pozycjaStartowa2 =
                            graf[a2.pobierzIdAktualnegoWierzcholka()]
                                .pozycjaWierzcholka();

                        if (!order.empty()) {
                            pozycjaKoncowa = pozycjeWierzcholkow[order.front()];

                            if (odwiedzone.size() < graf.size()) {
                                a1.przemiescAgenta(order.front());
                                odwiedzone.insert(order.front());
                            }
                            if (odwiedzone.size() - 1 == graf.size() - 1) {
                                (czySkonczone = true);
                            }
                        }

                        if (!order2.empty()) {
                            pozycjaKoncowa2 =
                                pozycjeWierzcholkow[order2.front()];

                            if (odwiedzone2.size() < graf.size()) {
                                a2.przemiescAgenta(order2.front());
                                odwiedzone2.insert(order2.front());
                            }
                            if (odwiedzone.size() - 1 == graf.size() - 1) {
                                (czySkonczone = true);
                            }
                        }

                        if (a2.pozycjaAgenta() == a1.pozycjaAgenta() ||
                            order.front() == poprzedniKrok2.back()) {
                            bonk.play();
                            if (a1.moc > a2.moc) {

                                sasiedzi = graf[a1.pobierzIdAktualnegoWierzcholka()].pobierzSasiadow();
                                przepchniecie = rand() % sasiedzi.size();
                                a1.przemiescAgenta(sasiedzi[przepchniecie]);

                                //a2.przemiescAgenta(
                                //    poprzedniKrok2[poprzedniKrok2.size() - 2]);
                                order.erase(order.begin());

                            } else if (a1.moc < a2.moc) {
                                
                                sasiedzi2 = graf[a2.pobierzIdAktualnegoWierzcholka()].pobierzSasiadow();
                                przepchniecie2 = rand() % sasiedzi2.size();
                                a2.przemiescAgenta(sasiedzi2[przepchniecie2]);
                                
                                //a1.przemiescAgenta(
                                //    poprzedniKrok[poprzedniKrok.size() - 2]);
                                order2.erase(order2.begin());
                            }
                        } else {

                            if (odwiedzone.size() < graf.size()) {
                                order.erase(order.begin());
                            }
                            if (odwiedzone2.size() < graf.size()) {
                                order2.erase(order2.begin());
                            }
                        }
                    }
                }

                if (keyPressed->code == sf::Keyboard::Key::Left) {
                    a1.przemiescAgenta(0);
                    order = backupOrder;
                    odwiedzone.clear();

                    poprzedniKrok.clear();
                    poprzedniKrok2.clear();

                    a2.przemiescAgenta(17);
                    order2 = backupOrder2;
                    odwiedzone2.clear();

                    czySkonczone = false;
                }
            }
        }
        // Ustawienie wizualizacji agenta na jego pozycje
        int pozycjaAgentaId = a1.pozycjaAgenta();
        int pozycjaAgentaId2 = a2.pozycjaAgenta();
        agentCircle.setPosition(pozycjeWierzcholkow[pozycjaAgentaId]);
        agentCircle2.setPosition(pozycjeWierzcholkow[pozycjaAgentaId2]);

        // Kolor backgroundu
        window.clear(sf::Color::Black);

        // Rysuj w kolejności:
        window.draw(krawedzie);
        for (const auto &wezel : wezly) {
            window.draw(wezel);
        }
        window.draw(agentCircle);
        window.draw(agentCircle2);

        window.display();
    }
}