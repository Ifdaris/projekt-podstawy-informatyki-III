#pragma once
#include <vector>
#include <deque>
#include <set>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "Agent.h"
#include "Wierzcholek.h"


struct BonkDane {
    bool aktywny = false;
    bool czyRazem = false;
    int ktoWygra = -1;
    int tura = 0;
};

void wykonajBonk(
    Agent &a1,
    Agent &a2,
    std::deque<int> &kolejka1,
    std::deque<int> &kolejka2,
    std::deque<int> &poprzednieKroki1,
    std::deque<int> &poprzednieKroki2,
    std::set<int> &odwiedzone1,
    std::set<int> &odwiedzone2,
    sf::Music &bonk,
    std::vector<Wierzcholek> &graf,
    bool &czySkonczone,
    sf::CircleShape &agentCircle1,
    sf::CircleShape &agentCircle2,
    BonkDane &daneBonka
);