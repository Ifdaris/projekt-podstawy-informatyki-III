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

struct Statystyki {
    int wygraneDFS = 0;
    int wygraneLosowe = 0;
};

int main() {
    srand(time(NULL));

    std::string line;
    std::ifstream plik("res/graf3.txt");
    std::vector<Wierzcholek> graf;
    std::vector<sf::Vector2f> pozycjeWierzcholkow;
    std::stringstream ss;

    int idWierzcholka, idSasiada;
    float pozycjaX, pozycjaY;

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

    int iloscAgentowDFS = 4;
    int iloscAgentowLosowych = 3;
    const int LICZBA_SYMULACJI = 100000;

    Statystyki statystyki;

    std::cout << "Start symulacji" << std::endl;

    int poczatkowyWierzcholek = rand() % graf.size();

    for (int symulacja = 0; symulacja < LICZBA_SYMULACJI; symulacja++) {

        // Inicjalizowanie agentow
        std::vector<DaneAgenta> agenci;
        std::unordered_set<int> wierzcholkiPoczatkowe;
        int idAgentow = 0;

        for (int i = 0; i < iloscAgentowDFS; i++, idAgentow++) {
            while (wierzcholkiPoczatkowe.find(poczatkowyWierzcholek) != wierzcholkiPoczatkowe.end()) {
                poczatkowyWierzcholek = rand() % graf.size();
            }
            // Animacja
            sf::Vector2f startPos = pozycjeWierzcholkow[poczatkowyWierzcholek];
            agenci.emplace_back(idAgentow, poczatkowyWierzcholek, sf::Color::Red, dfs(graf, poczatkowyWierzcholek), rand() % 100, startPos);
            //--------

            wierzcholkiPoczatkowe.insert(poczatkowyWierzcholek);
        }

        for (int i = 0; i < iloscAgentowLosowych; i++, idAgentow++) {
            while (wierzcholkiPoczatkowe.find(poczatkowyWierzcholek) != wierzcholkiPoczatkowe.end()) {
                poczatkowyWierzcholek = rand() % graf.size();
            }
            // Animacja
            sf::Vector2f startPos = pozycjeWierzcholkow[poczatkowyWierzcholek];
            agenci.emplace_back(idAgentow, poczatkowyWierzcholek, sf::Color::Red, algorytmlosowy(graf, poczatkowyWierzcholek), rand() % 100, startPos);
            //---------
            wierzcholkiPoczatkowe.insert(poczatkowyWierzcholek);
        }

        bool czySkonczone = false;
        bool czyTura = false;
        vector<size_t> przegrani;
        int limitTur = 0;

        while (!czySkonczone && limitTur < 2000) {
            limitTur++;

            if (czyTura == false) {

                for (int i = 0; i < agenci.size(); i++) {
                    if (agenci[i].czyZywy == false)
                        continue;

                    if (!agenci[i].kolejka.empty()) {

                        if (agenci[i].odwiedzone.size() < graf.size()) {

                            agenci[i].agent.przemiescAgenta(agenci[i].kolejka.front());
                            agenci[i].odwiedzone.insert(agenci[i].kolejka.front());
                            agenci[i].poprzednieKroki.emplace_back(agenci[i].kolejka.front());
                            agenci[i].kolejka.pop_front();
                        }
                        if (agenci[i].odwiedzone.size() == graf.size() - 1) {
                            czySkonczone = true;
                            if (i < iloscAgentowDFS - 1) {
                                statystyki.wygraneDFS++;
                                break;
                            }
                            else {
                                statystyki.wygraneLosowe++;
                                break;
                            }
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
            } else {
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
                            agenci[i].czyZywy = false;
                        } else {
                            agenci[i].cofnij(pozycjeWierzcholkow[idCeluCofniecia]);
                        }
                    }
                }
                przegrani.clear();
            }
        }

        if ((symulacja + 1) % 100 == 0)
            std::cout << "Ukonczono " << symulacja + 1 << " symulacji." << std::endl;
    }

    std::cout << "Wyniki: " << std::endl;
    std::cout << "DFS Wygral: " << statystyki.wygraneDFS << " razy (" << (float)statystyki.wygraneDFS / LICZBA_SYMULACJI * 100.0f << "%)" << std::endl;
    std::cout << "Losowy Wygral: " << statystyki.wygraneLosowe << " razy (" << (float)statystyki.wygraneLosowe / LICZBA_SYMULACJI * 100.0f << "%)" << std::endl;
    std::cout << "Remisy/Bledy (Limit tur): " << (LICZBA_SYMULACJI - statystyki.wygraneDFS - statystyki.wygraneLosowe) << std::endl;

    std::cin.get();
    return 0;
}