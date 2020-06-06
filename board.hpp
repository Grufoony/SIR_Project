//Definiamo qui la classe board, ripresa dall'esercizio Game of Life con le opportune modifiche
//greg sei un bastardo muhihiahaha

#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <string>
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <random>
#include <ctime>
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>

//Definisco una variabile per pulire la console a seconda del sistema operativo in uso
#if defined _WIN32
#define cls "cls"
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__) || defined (__APPLE__)
#define cls "clear"
#endif


enum class Sir :char {
    s = 45, i = 43, r = -80, q = 81
};

Sir& operator++(Sir& hs) {
    switch (hs) {
    case Sir::s: hs = Sir::i; return hs; //++s = i
    case Sir::i: hs = Sir::r; return hs; //++i = r
    case Sir::q: hs = Sir::r; return hs; //++q = r
    default: return hs;
    }
};

struct Cell {
    Sir state;
    double inf_prob; //Probabilità di infettarsi
    double rec_prob; //Probabilità di guarire
    int clock;
    Cell() {
        state = Sir::s;
        clock = 0;
    }
};

class Board {
    std::string name_;
    std::vector<std::vector<Cell>> grid_;
    int dimension_;
    double beta_;
    double gamma_;
    double q_prob_;
    int day_;
    bool adv_opt; //Nuova variabile per attivare opzioni avanzate

    //Funzione per stampare la tabella
    void print_() {
        std::cout << "Day " << day_ << '\n';
        for (int l = 0; l <= dimension_ - 1; ++l) {
            for (int c = 0; c <= dimension_ - 1; ++c) {
                if (c == 0 || l == 0 || l == dimension_ - 1 || c == dimension_ - 1) {
                    std::cout << '#';       //stampa il bordo
                }
                else {
                    std::cout << static_cast<char>(grid_[l][c].state);
                }
            }
            std::cout << '\n';
        }
    }

    //Funzione per muovere le celle
    void move_() {
        for (int l = 1; l < dimension_ - 1; ++l) {
            for (int c = 1; c < dimension_ - 1; ++c) {
                if (grid_[l][c].state != Sir::q) {
                    int swap; // indicatore, se ha valore da 0 a 7 fa scambiare la cella con una delle 8 adiacenti
                    int newc, newl; //indici della cella da scambiare
                    if (day_ < 8)
                        swap = (rand() + time(0)) % 8;
                    else
                        swap = (rand() + time(0)) % day_; //col passare dei giorni diminuisce la probabilit� che una cella si muova( non mi piace da migliorare).
                    switch (swap) {
                    case 0: newc = c - 1;   newl = l - 1;   break;
                    case 1: newc = c;       newl = l - 1;   break;
                    case 2: newc = c + 1;   newl = l - 1;   break;
                    case 3: newc = c - 1;   newl = l;       break;
                    case 4: newc = c + 1;   newl = l;       break;
                    case 5: newc = c - 1;   newl = l + 1;   break;
                    case 6: newc = c;       newl = l + 1;   break;
                    case 7: newc = c + 1;   newl = l + 1;   break;
                    default: return;
                    }
                    if (newc == 0)
                        ++newc;
                    if (newl == 0)
                        ++newl;
                    if (newc == dimension_ - 1)
                        --newc;
                    if (newl == dimension_ - 1)
                        --newl;
                    assert(newc > 0 && newc < dimension_ - 1 && newl>0 && newl < dimension_ - 1);
                    if(grid_[newl][newc].state!=Sir::q){
                        auto old = new Cell;
                        *old = grid_[l][c];
                        grid_[l][c].state = grid_[newl][newc].state;
                        grid_[newl][newc] = *old;
                        delete old;
                    }
                  }
            }
        
        }
    }

    //Funzione per evolvere di un giorno la tabella. Questo è il cuore del programma
    void evolve_() {
        for(int l = 1; l < dimension_-1; ++l) {
            for(int c = 1; c < dimension_-1; ++c) {
                if(grid_[l][c].state == Sir::s) {
                    for (int j = -1; j <= 1; ++j) {
                        for (int i = -1; i <= 1; ++i) {
                            if (grid_[l + j][c + i].state == Sir::i) {
                                int ran = ((rand() + time(0)) % 101) -1;
                                if(ran < (int)beta_*100) {
                                    ++grid_[l][c].state;
                                }                               
                            }
                        }
                    }
                } else if(grid_[l][c].state == Sir::i) {
                    int ran = ((rand() + time(0)) % 101) -1;
                    if(ran < (int)gamma_*100) {
                        ++grid_[l][c].state;
                    } else {
                        int ran2 = ((rand() + time(0)) % 101) -1;
                        if(ran2 < (int)q_prob_*100) {
                            grid_[l][c].state = Sir::q;
                        }
                    }
                } else if(grid_[l][c].state == Sir::q) {
                    if(grid_[l][c].clock == 40) {
                        ++grid_[l][c].state;
                    } else {
                        ++grid_[l][c].clock;
                    }
                }
            }
        }
        ++day_;
    }
    
    //Funzione tipo move ma a lungo range
    void aeroplane_() {
        for (int l = 1; l < dimension_; ++l) {
            for (int c = 1; c < dimension_; ++c) {
                int colonna = (rand() + time(0)) % dimension_;
                int riga = (rand() + time(0)) % dimension_;
                if(grid_[l][c].state!=Sir::q && grid_[colonna][riga].state!=Sir::q) {
                    
                    if (riga == 1) {
                        auto sposto = new Cell();
                        sposto->state = grid_[colonna][riga].state;
                        sposto->inf_prob = grid_[colonna][riga].inf_prob;
                        sposto->rec_prob = grid_[colonna][riga].rec_prob;
                        grid_[colonna][riga].state = grid_[l][c].state;
                        grid_[colonna][riga].inf_prob = grid_[l][c].inf_prob;
                        grid_[colonna][riga].rec_prob = grid_[l][c].rec_prob;
                        grid_[l][c].state = sposto->state;
                        grid_[l][c].inf_prob = sposto->inf_prob;
                        grid_[l][c].rec_prob = sposto->rec_prob;
                        delete sposto;
                    }
                }
            }
        }
    }

public:
    //Costruttore principale (nome, dimensione, prob. inf., prob. rec., prob. q., n° infetti iniziali, abilita modalità avanzata)
    Board(std::string c, int n, double b, double y, double q, int inf, bool pro) : name_{c}, grid_(n + 2, std::vector<Cell>(n + 2)), adv_opt{pro} {
        //Verifico la coerenza dei dati
        assert(b > 0 && b < 1);
        assert(y > 0 && y < 1);
        assert(q > 0 && q < 1);
        assert(n > 3);
        assert(inf > 0);

        q_prob_ = q;
        beta_ = b;
        gamma_ = y;
        dimension_ = n + 2;
        day_ = 0;
        for(int i = 0; i < inf; ++i) {
            int ran1 = (std::rand() + time(0)) % dimension_;
            int ran2 = (std::rand() + time(0)) % dimension_;
            grid_[ran1][ran2].state = Sir::i;
        }
    };
    //Costruttore secondario - disabilità di default la modalità avanzata
    Board(std::string c, int n, double b, double y, double q, int inf) : name_{c}, grid_(n + 2, std::vector<Cell>(n + 2)) {
        //Verifico la coerenza dei dati
        assert(b > 0 && b < 1);
        assert(y > 0 && y < 1);
        assert(q > 0 && q < 1);
        assert(n > 3);
        assert(inf > 0);

        q_prob_ = q;
        beta_ = b;
        gamma_ = y;
        dimension_ = n + 2;
        day_ = 0;
        adv_opt = false;
        for(int i = 0; i < inf; ++i) {
            int ran1 = (std::rand() + time(0)) % dimension_;
            int ran2 = (std::rand() + time(0)) % dimension_;
            grid_[ran1][ran2].state = Sir::i;
        }
    };

    Sir& operator()(int riga, int colonna) {
        return (grid_[riga + 1][colonna + 1].state);
    }

    //Funzione per cambiare il valore di beta
    void setBeta(double& b) {
        assert(b > 0 && b < 1);
        beta_ = b;
    }

    //Funzione per cambiare il valore di gamma
    void setGamma(double& y) {
        assert(y > 0 && y < 1);
        gamma_ = y;
    }

    //Funzione per abilitare le opzioni avanzate(move, q, ecc)
    void change_adv_opt(bool hs) {
        adv_opt = hs;
    }

    //Funzione pubblica che permette di stampare l'evoluzione in n giorni (days)
    void print(int const& days) {
        assert(days > 0);
        using namespace std::chrono_literals;
        while (day_ < days) {
            system(cls);
            print_();
            evolve_();
            if (adv_opt) {
                move_();
                aeroplane_();
            }
            std::this_thread::sleep_for(1s);
        }
    }

    //Funzione per avare la grafica
    void draw() {
        float bit_size = 1.;
        //L'if di seguito setta di fatto un fattore di scala nel caso la board sia troppo piccola
        if (dimension_ < 100) {
            bit_size = 10.;
        }
        auto win_size = bit_size * dimension_;
        assert(win_size > 99 && win_size < 1001);
        sf::RenderWindow window(sf::VideoMode(win_size, win_size), name_, sf::Style::Close | sf::Style::Resize); //Creo una finestra (ogni cella = BIT_SIZE px)
        //Attivo il VSync (Max framerate = framerate del monitor utilizzato)
        window.setVerticalSyncEnabled(true);
        //Setto i bit dei quadratini e i rispettivi colori
        sf::RectangleShape sus_bit(sf::Vector2f(bit_size, bit_size));
        sf::RectangleShape inf_bit(sf::Vector2f(bit_size, bit_size));
        sf::RectangleShape rec_bit(sf::Vector2f(bit_size, bit_size));
        sf::RectangleShape q_bit(sf::Vector2f(bit_size, bit_size));

        sus_bit.setFillColor(sf::Color::Green);
        inf_bit.setFillColor(sf::Color::Red);
        rec_bit.setFillColor(sf::Color::Blue);
        q_bit.setFillColor(sf::Color::Black);

        while (window.isOpen()) {

            //Gestione eventi
            sf::Event evnt;
            while (window.pollEvent(evnt)) {
                //Se clicco la X chiude la finestra
                if (evnt.type == sf::Event::Closed) {
                    window.close();
                }
            }

            std::cout << "Day " << day_ << '\n';
            evolve_();
            if (adv_opt) {
                move_();
                aeroplane_();
              
            }

            for (int l = 1; l < dimension_ - 1; ++l) {
                for (int c = 1; c < dimension_ - 1; ++c) {
                    //Stampa le celle
                    if (grid_[l][c].state == Sir::s) {
                        sus_bit.setPosition(bit_size * c, bit_size * l);
                        window.draw(sus_bit);
                    }
                    else if (grid_[l][c].state == Sir::i) {
                        inf_bit.setPosition(bit_size * c, bit_size * l);
                        window.draw(inf_bit);
                    }
                    else if (grid_[l][c].state == Sir::r) {
                        rec_bit.setPosition(bit_size * c, bit_size * l);
                        window.draw(rec_bit);
                    }
                    else if (grid_[l][c].state == Sir::q) {
                        q_bit.setPosition(bit_size * c, bit_size * l);
                        window.draw(q_bit);
                    }
                }
            }

            window.display();
        }
    }
};
#endif 
