#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Agent.h"
#include "DFS_2.h"
#include "Wierzcholek.h"

int main() {
    std::string line;
    std::ifstream plik("D:\\Programming\\cmake-sfml-project\\res\\graf.txt");
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

    sf::RenderWindow window(sf::VideoMode({1000, 600}), "Graf");
    window.setFramerateLimit(60);

    // Wizualizacja wierzchołków
    std::vector<sf::CircleShape> wezly;
    for (const sf::Vector2f &pos : pozycjeWierzcholkow) {
        sf::CircleShape shape(10.f);
        shape.setFillColor(sf::Color::White);
        shape.setOrigin({shape.getRadius(), shape.getRadius()});
        shape.setPosition(pos);
        wezly.push_back(shape);
    }

    // Wizualizacja agenta
    sf::CircleShape agentCircle(8.f);
    agentCircle.setFillColor(sf::Color::Red);
    agentCircle.setOrigin({agentCircle.getRadius(), agentCircle.getRadius()});

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
    std::vector<int> backupOrder = order;

    int licznik = 0;

    while (window.isOpen()) {

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto *keyPressed =
                    event->getIf<sf::Event::KeyPressed>()) {

                if (keyPressed->code == sf::Keyboard::Key::Right) {

                    if (!order.empty()) {
                        a1.przemiescAgenta(order.front());
                        order.erase(order.begin());
                    }
                }

                if (keyPressed->code == sf::Keyboard::Key::Left) {
                    a1.przemiescAgenta(0);
                    order = backupOrder;
                }
            }
        }
        // Ustawienie wizualizacji agenta na jego pozycje
        int pozycjaAgentaId = a1.pozycjaAgenta();
        agentCircle.setPosition(pozycjeWierzcholkow[pozycjaAgentaId]);

        // Kolor backgroundu
        window.clear(sf::Color::Black);

        // Rysuj w kolejności:
        window.draw(krawedzie);
        for (const auto &wezel : wezly) {
            window.draw(wezel);
        }
        window.draw(agentCircle);

        window.display();
    }
}