#include "bonk.h"

void wykonajBonk(Agent &a1,
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
                 BonkDane &daneBonka) {

    agentCircle2.setFillColor(sf::Color::Green);

    if (daneBonka.czyRazem == true) {
        if (daneBonka.ktoWygra == 1) {
            if (!poprzednieKroki2.empty()) {
                int temp = a2.pozycjaAgenta();
                a2.przemiescAgenta(poprzednieKroki2.back());
                kolejka2.emplace_front(temp);
            }

        } else if (daneBonka.ktoWygra == 2) {
            if (!poprzednieKroki1.empty()) {
                poprzednieKroki1.pop_back();
                kolejka1.emplace_front(a1.pozycjaAgenta());
                kolejka1.emplace_front(poprzednieKroki1.back());
                kolejka2.emplace_front(a2.pozycjaAgenta());
                poprzednieKroki1.pop_back();
            }
        }
    } else {
        if (daneBonka.ktoWygra == 1) {
            a2.przemiescAgenta(a1.pozycjaAgenta());

        } else if (daneBonka.ktoWygra == 2) {
            
            poprzednieKroki1.pop_back();
            kolejka1.emplace_front(a1.pozycjaAgenta());
            a1.przemiescAgenta(poprzednieKroki1.back());
            a2.przemiescAgenta(a1.pozycjaAgenta());
            odwiedzone2.insert(a2.pozycjaAgenta());
            poprzednieKroki1.pop_back();
            daneBonka.tura++;
        }
    }
}