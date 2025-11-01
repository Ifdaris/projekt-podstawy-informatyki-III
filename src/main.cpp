#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Agent.h"
#include "Wierzcholek.h"

int main() {
    std::string line;
    std::ifstream plik("D:\\Programming\\cmake-sfml-project\\res\\graf.txt");
    std::vector<Wierzcholek> graf;
    std::stringstream ss;

    sf::RenderWindow window(sf::VideoMode({800, 600}), "My window");

    int idWierzcholka;
    int idSasiada;

    while (getline(plik, line)) {
        ss.clear();

        ss.str(line);

        ss >> idWierzcholka;
        Wierzcholek wierzcholek(idWierzcholka);
        while (ss >> idSasiada) {
            wierzcholek.dodajSasiada(idSasiada);
        }
        graf.push_back(wierzcholek);
        ss.clear();
    }

    /*
    for (Wierzcholek wierzcholek : graf) {
        wierzcholek.wyswietlDane();
    }*/

    Agent a1(0, 0);
    a1.pozycjaAgenta();

    for (int a : graf[a1.pozycjaAgenta()].pobierzSasiadow()) {
        std::cout << "Sasiad: " << a << std::endl;
    }

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>()) window.close();
        }
        window.clear(sf::Color::Black);
        window.display();
    }
}