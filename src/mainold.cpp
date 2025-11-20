#include <SFML/Graphics.hpp>
#include <atomic>
#include <chrono> // KLUCZOWE: Do pomiaru czasu
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>
#include <unordered_set>
#include <vector>
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

// Zmienna atomowa - bezpieczna dla wielu wątków
std::atomic<int> postepCalkowity(0);

// Funkcja pomocnicza do rysowania paska w konsoli (BEZ ZMIAN)
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

// Funkcja wykonajSymulacje (Poprawiona linia wywołania algorytmlosowy)
void wykonajSymulacje(int liczbaSymulacji, vector<Wierzcholek> &graf, vector<sf::Vector2f> &pozycjeWierzcholkow, int iloscAgentowDFS, int iloscAgentowLosowych, Statystyki *statystykiLokalne) {
    
    // Zmienne Losowe Thread-Local (PRZESUNIĘTE POZA PĘTLĘ GŁÓWNĄ)
    static thread_local std::mt19937 generator(
        std::hash<std::thread::id>{}(std::this_thread::get_id()) + time(NULL)
    );
    // Dystrybucje dla wierzchołków i procentów
    std::uniform_int_distribution<int> distribution(0, graf.size() - 1);
    std::uniform_int_distribution<int> percentDist(0, 99);

    // OPTYMALIZACJA RECYKLINGU: Alokacja kontenerów RAZ przed pętlą
    vector<DaneAgenta> agenci;
    unordered_set<int> wierzcholkiPoczatkowe;
    vector<size_t> przegrani;
    vector<vector<int>> agenciNaWierzcholku(graf.size());

    for (int symulacja = 0; symulacja < liczbaSymulacji; symulacja++) {

        // --- RECYKLING: Czyszczenie kontenerów ---
        agenci.clear();
        wierzcholkiPoczatkowe.clear();
        przegrani.clear(); 
        
        int poczatkowyWierzcholek = distribution(generator);
        int idAgentow = 0;

        // Inicjalizacja DFS
        for (int i = 0; i < iloscAgentowDFS; i++, idAgentow++) {
            while (wierzcholkiPoczatkowe.find(poczatkowyWierzcholek) != wierzcholkiPoczatkowe.end()) {
                poczatkowyWierzcholek = distribution(generator);
            }
            sf::Vector2f startPos = pozycjeWierzcholkow[poczatkowyWierzcholek];
            agenci.emplace_back(idAgentow, poczatkowyWierzcholek, sf::Color::Red, dfs(graf, poczatkowyWierzcholek), percentDist(generator), startPos);
            wierzcholkiPoczatkowe.insert(poczatkowyWierzcholek);
        }

        // Inicjalizacja Losowych
        for (int i = 0; i < iloscAgentowLosowych; i++, idAgentow++) {
            while (wierzcholkiPoczatkowe.find(poczatkowyWierzcholek) != wierzcholkiPoczatkowe.end()) {
                poczatkowyWierzcholek = distribution(generator);
            }
            sf::Vector2f startPos = pozycjeWierzcholkow[poczatkowyWierzcholek];
            // POPRAWIONE WYWOŁANIE: Z generator
            agenci.emplace_back(idAgentow, poczatkowyWierzcholek, sf::Color::Red, algorytmlosowy(graf, poczatkowyWierzcholek, generator), percentDist(generator), startPos);
            wierzcholkiPoczatkowe.insert(poczatkowyWierzcholek);
        }

        bool czySkonczone = false;
        bool czyTura = false;
        int limitTur = 0;

        while (!czySkonczone && limitTur < 2000) {
            limitTur++;

            if (czyTura == false) {
                // --- LOGIKA RUCHU ---
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
                            if (i < iloscAgentowDFS)
                                statystykiLokalne->wygraneDFS++;
                            else
                                statystykiLokalne->wygraneLosowe++;
                            break;
                        }
                    }
                }
                if (czySkonczone) break; 

                // --- LOGIKA KOLIZJI I WALKI ---
                for (auto& w : agenciNaWierzcholku) {
                    w.clear(); // Recykling, czyszczenie wektorów
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
                // --- LOGIKA COFANIA SIĘ PRZEGRANYCH (Po walce) ---
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

        // --- ZWIĘKSZANIE LICZNIKA (DLA PASKA POSTĘPU) ---
        postepCalkowity++; 
    }
}

int main() {
    
    // Wczytywanie (bez zmian)
    string line;
    ifstream plik("res/graf3.txt");
    if (!plik.is_open()) {
        cout << "Blad pliku!" << endl;
        return 0;
    }

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
        while (ss >> idSasiada)
            wierzcholek.dodajSasiada(idSasiada);
        graf.push_back(wierzcholek);
    }
    plik.close();

    int iloscAgentowDFS = 4;
    int iloscAgentowLosowych = 3;
    //licznik
    const int LICZBA_SYMULACJI = 10000; // Zwiększenie dla lepszego pomiaru czasu

    // --- WIELOWĄTKOWOŚĆ ---
    const int LICZBA_WATKOW = 2;
    int symulacjiNaWatek = LICZBA_SYMULACJI / LICZBA_WATKOW;

    vector<thread> watki;
    vector<Statystyki> wyniki(LICZBA_WATKOW);
    postepCalkowity = 0; 

    cout << "Start: " << LICZBA_SYMULACJI << " symulacji (" << LICZBA_WATKOW << " watkow)." << endl;
    cout << "Przetwarzanie..." << endl;

    // 1. ZACZNIJ POMIAR CZASU ⏱️
    auto start = chrono::high_resolution_clock::now();

    // 2. Uruchomienie wątków
    for (int i = 0; i < LICZBA_WATKOW; ++i) {
        int biezaceSymulacje = symulacjiNaWatek;
        if (i == LICZBA_WATKOW - 1)
            biezaceSymulacje += LICZBA_SYMULACJI % LICZBA_WATKOW;

        watki.emplace_back([&, biezaceSymulacje, i]() {
            wykonajSymulacje(biezaceSymulacje, graf, pozycjeWierzcholkow, iloscAgentowDFS, iloscAgentowLosowych, &wyniki[i]);
        });
    }

    // 3. Pętla monitorująca postęp 
    while (postepCalkowity < LICZBA_SYMULACJI) {
        rysujPasekPostepu(postepCalkowity, LICZBA_SYMULACJI);
        this_thread::sleep_for(chrono::milliseconds(100)); 
    }
    rysujPasekPostepu(LICZBA_SYMULACJI, LICZBA_SYMULACJI);
    cout << endl;

    // 4. Złączanie wątków
    for (auto &watek : watki) {
        watek.join();
    }
    
    // 5. ZAKOŃCZ POMIAR CZASU 🏁
    auto koniec = chrono::high_resolution_clock::now();
    // 6. OBLICZ CZAS TRWANIA
    auto czasTrwania = chrono::duration_cast<chrono::milliseconds>(koniec - start);


    // --- WYNIKI ---
    Statystyki statystykiCalkowite;
    for (const auto &stat : wyniki) {
        statystykiCalkowite.wygraneDFS += stat.wygraneDFS;
        statystykiCalkowite.wygraneLosowe += stat.wygraneLosowe;
    }

    cout << endl
         << "=== WYNIKI KONCOWE ===" << endl;
    cout << "DFS Wygral: " << statystykiCalkowite.wygraneDFS << " (" << (float)statystykiCalkowite.wygraneDFS / LICZBA_SYMULACJI * 100.0f << "%)" << endl;
    cout << "Losowy Wygral: " << statystykiCalkowite.wygraneLosowe << " (" << (float)statystykiCalkowite.wygraneLosowe / LICZBA_SYMULACJI * 100.0f << "%)" << endl;
    cout << "Nierozstrzygniete: " << (LICZBA_SYMULACJI - statystykiCalkowite.wygraneDFS - statystykiCalkowite.wygraneLosowe) << endl;
    
    // 7. WYDRUKUJ CZAS
    cout << endl
         << "Czas wykonania (multi-threaded [" << LICZBA_WATKOW << "]): " << czasTrwania.count() << " ms" << endl;


    cin.get();
    return 0;
}