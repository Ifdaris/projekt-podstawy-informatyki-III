#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <chrono> 
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <random> 
#include <ctime>  

#include "AlgorytmLosowy.h"
#include "DFS.h"
#include "DaneAgenta.h"
#include "Wierzcholek.h"

using namespace std;

struct Statystyki {
    int wygraneDFS = 0;
    int wygraneLosowe = 0;
};

// Funkcja do rysowania paska postępu w konsoli (BEZ ZMIAN)
void rysujPasekPostepu(int obecny, int calkowity) {
    const int szerokoscPaska = 50;
    float procent = (float)obecny / calkowity;
    int wypelnienie = (int)(szerokoscPaska * procent);

    cout << "\r["; 
    for (int i = 0; i < szerokoscPaska; ++i) {
        if (i < wypelnienie)
            cout << "=";
        else if (i == wypelnienie)
            cout << ">";
        else
            cout << " ";
    }
    cout << "] " << int(procent * 100.0) << "% (" << obecny << "/" << calkowity << ")" << flush;
}

int main() {
    
    // 💡 ZASTĄPIENIE srand/rand PRZEZ C++11 GENERATOR
    std::mt19937 generator(time(NULL));
    
    string line;
    ifstream plik("res/graf3.txt");

    if (!plik.is_open()) {
        cout << "Blad: Nie mozna otworzyc res/graf3.txt" << endl;
        return 1;
    }

    // --- Wczytywanie pliku (fragment usunięty dla zwięzłości, zakładamy, że działa) ---
    vector<Wierzcholek> graf;
    vector<sf::Vector2f> pozycjeWierzcholkow;
    stringstream ss;
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
    // Przywracamy 1000, ale w realnych testach lepiej zostawić dużą liczbę (np. 100000)
    const int LICZBA_SYMULACJI = 10000; 
    
    Statystyki statystyki;

    cout << "Start symulacji jednowatkowych (" << LICZBA_SYMULACJI << " prob)..." << endl;

    // 1. ZACZNIJ POMIAR CZASU ⏱️
    auto start = chrono::high_resolution_clock::now();
    
    // 💡 OPTYMALIZACJA RECYKLINGU
    vector<DaneAgenta> agenci;
    unordered_set<int> wierzcholkiPoczatkowe;
    vector<size_t> przegrani;
    vector<vector<int>> agenciNaWierzcholku(graf.size()); 
    
    std::uniform_int_distribution<int> distWierzcholek(0, graf.size() - 1);
    std::uniform_int_distribution<int> distProcent(0, 99);
    
    // Główna pętla symulacji
    for (int symulacja = 0; symulacja < LICZBA_SYMULACJI; symulacja++) {

        // --- RECYKLING: Czyścimy zamiast tworzyć nowe ---
        agenci.clear();
        wierzcholkiPoczatkowe.clear();
        przegrani.clear();

        int poczatkowyWierzcholek = distWierzcholek(generator);
        int idAgentow = 0;

        // Inicjalizacja agentów (DFS i Losowych, z użyciem generatora)
        for (int i = 0; i < iloscAgentowDFS; i++, idAgentow++) {
            while (wierzcholkiPoczatkowe.count(poczatkowyWierzcholek)) { 
                poczatkowyWierzcholek = distWierzcholek(generator);
            }
            sf::Vector2f startPos = pozycjeWierzcholkow[poczatkowyWierzcholek];
            agenci.emplace_back(idAgentow, poczatkowyWierzcholek, sf::Color::Red, dfs(graf, poczatkowyWierzcholek), distProcent(generator), startPos);
            wierzcholkiPoczatkowe.insert(poczatkowyWierzcholek);
        }

        for (int i = 0; i < iloscAgentowLosowych; i++, idAgentow++) {
            while (wierzcholkiPoczatkowe.count(poczatkowyWierzcholek)) {
                poczatkowyWierzcholek = distWierzcholek(generator);
            }
            sf::Vector2f startPos = pozycjeWierzcholkow[poczatkowyWierzcholek];
            agenci.emplace_back(idAgentow, poczatkowyWierzcholek, sf::Color::Red, algorytmlosowy(graf, poczatkowyWierzcholek, generator), distProcent(generator), startPos);
            wierzcholkiPoczatkowe.insert(poczatkowyWierzcholek);
        }

        bool czySkonczone = false;
        bool czyTura = false;
        int limitTur = 0;

        // Główna pętla symulacji pojedynczego scenariusza (logika ruchu/walki bez zmian)
        while (!czySkonczone && limitTur < 2000) {
            limitTur++;

            if (czyTura == false) {
                // LOGIKA RUCHU I ZLICZANIA WYGRANYCH
                for (int i = 0; i < agenci.size(); i++) {
                    if (agenci[i].czyZywy == false) continue;
                    if (!agenci[i].kolejka.empty()) {
                        if (agenci[i].odwiedzone.size() < graf.size()) {
                            agenci[i].agent.przemiescAgenta(agenci[i].kolejka.front());
                            agenci[i].odwiedzone.insert(agenci[i].kolejka.front());
                            agenci[i].poprzednieKroki.emplace_back(agenci[i].kolejka.front());
                            agenci[i].kolejka.pop_front();
                        }
                        if (agenci[i].odwiedzone.size() == graf.size() - 1) {
                            czySkonczone = true;
                            if (i < iloscAgentowDFS) {
                                statystyki.wygraneDFS++;
                            } else {
                                statystyki.wygraneLosowe++;
                            }
                            break;
                        }
                    }
                }
                if (czySkonczone) break;

                // LOGIKA KOLIZJI
                for (auto& vec : agenciNaWierzcholku) {
                    vec.clear();
                }

                for (int i = 0; i < agenci.size(); i++) {
                    if (agenci[i].czyZywy == false) continue;
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
            } else {
                // LOGIKA COFANIA SIĘ PRZEGRANYCH
                czyTura = false;
                for (size_t i : przegrani) {
                    bool doZabicia = false;
                    if (agenci[i].czyZywy == false) continue;

                    if (agenci[i].poprzednieKroki.size() >= 2) {
                        int idCeluCofniecia = agenci[i].poprzednieKroki[agenci[i].poprzednieKroki.size() - 2];

                        for (auto &j : agenci) {
                            if (j.czyZywy == false) continue;
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

        // 🛑 POPRAWKA: ZMIENIONO WARUNEK DLA PASKA POSTĘPU
        // Aktualizacja co 100 symulacji.
        if ((symulacja + 1) % 100 == 0 || symulacja == LICZBA_SYMULACJI - 1) {
            rysujPasekPostepu(symulacja + 1, LICZBA_SYMULACJI);
        }
    }
    cout << endl; // Dodajemy nową linię po pasku 100%

    // 2. ZAKOŃCZ POMIAR CZASU 🏁
    auto koniec = chrono::high_resolution_clock::now();
    auto czasTrwania = chrono::duration_cast<chrono::milliseconds>(koniec - start);

    cout << endl
         << "=== WYNIKI KONCOWE ===" << endl;
    cout << "DFS Wygral: " << statystyki.wygraneDFS << " razy (" << (float)statystyki.wygraneDFS / LICZBA_SYMULACJI * 100.0f << "%)" << endl;
    cout << "Losowy Wygral: " << statystyki.wygraneLosowe << " razy (" << (float)statystyki.wygraneLosowe / LICZBA_SYMULACJI * 100.0f << "%)" << endl;
    cout << "Remisy/Bledy: " << (LICZBA_SYMULACJI - statystyki.wygraneDFS - statystyki.wygraneLosowe) << endl;

    // 4. WYDRUKUJ CZAS
    cout << endl
         << "Czas wykonania (single-threaded, zoptymalizowany): " << czasTrwania.count() << " ms" << endl;

    cin.get();
    return 0;
}