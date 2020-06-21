#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
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
    s = 45, i = 43, r = -80, b = 81 //b = boundary
};


Sir& operator++(Sir& hs) {
    switch (hs) {
    case Sir::s: hs = Sir::i; return hs; //++s = i
    case Sir::i: hs = Sir::r; return hs; //++i = r
    default: return hs;
    }
};

struct Cell {
    Sir state;
    double inf_prob = 0; //Probabilità di infettarsi
    double rec_prob = 0; //Probabilità di guarire
    Cell() {
        state = Sir::s;
        inf_prob = 0;
        rec_prob = 0;
    }
};

class Board {
    std::vector<std::vector<Cell>> grid_;
    int dimension_;
    double beta_;
    double gamma_;
    int day;
    int number_infected = 4;
    /*int num_s1, num_s2, num_s3, num_s4;     // S, I, R dei singoli stati per grafico
    int num_i1, num_i2, num_i3, num_i4;
    int num_r1, num_r2, num_r3, num_r4;*/
    
public:
    Board(int country_dim, double b, double y) : grid_(2*country_dim + 3, std::vector<Cell>(2*country_dim + 3)){
        //Verifico la coerenza dei dati
        assert(b > 0 && b < 1);
        assert(y > 0 && y < 1);
        assert(country_dim > 3);
                
        beta_ = b;
        gamma_ = y;
        dimension_ = 2*country_dim + 3;
        day = 0;

        //Setto il bordo e i confini
        for (int l = 0; l < dimension_; ++l) {
            for (int c = 0; c < dimension_; ++c) {
                if (l == 0 || c == 0 || l == dimension_ - 1 || c == dimension_ - 1 || l == country_dim + 1 || c == country_dim + 1)
                    grid_[l][c].state = Sir::b;
            }
        }
    };
    
    Sir& operator()(int riga, int colonna) {
        return (grid_[riga + 1][colonna + 1].state);
    }


    void copy_(std::vector<std::vector<Sir>>& end) {
        for (int l = 1; l <= dimension_ - 1; ++l) {
            for (int c = 0; c <= dimension_ - 1; ++c) {
                if (l == 0 || c == 0 || l == dimension_ - 1 || c == dimension_ - 1 || l == (dimension_-3)/2+1 || c == (dimension_ - 3) / 2 + 1)
                {
                    grid_[l][c].state = Sir::b;
                }
                else {
                    grid_[l][c].state = end[l - 1][c - 1];
                }
            }
        }
    }

    //Funzione per muovere le celle
    void move_() {
        int maxc = dimension_ - 1;
        for (int l = 1; l < dimension_ - 1; ++l) {
            if (l == (dimension_ - 3) / 2 + 1)  //fa in modo che nella country 4 non avvengano spostamenti
                maxc = (dimension_ - 3) / 2 + 1;
            for (int c = 1; c < maxc; ++c) {
                if (grid_[l][c].state != Sir::b) {
                    int swap; // indicatore, se ha valore da 0 a 7 fa scambiare la cella con una delle 8 adiacenti
                    int newc, newl; //indici della cella da scambiare
                    swap = (rand() + time(0)) % 8; //probabilità di circa il 60% che si sposti
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

                    if (grid_[newl][newc].state != Sir::b && grid_[l][c].state != Sir::b) {
                        auto old = new Cell;
                        *old = grid_[l][c];
                        grid_[l][c] = grid_[newl][newc];
                        grid_[newl][newc] = *old;
                        delete old;
                    }
                }
            }

        }
    }
    //Funzione per evolvere di un giorno la tabella.Questo è il cuore del programma
    void evolve_() {
        std::vector<std::vector<Sir>> end(dimension_ - 2, std::vector<Sir>(dimension_ - 2));
        number_infected = 0;
        for (int m = 1; m < dimension_ - 1; ++m) {//conta il numero di infetti in ogni vettore grid_
            for (int n = 1; n < dimension_ - 1; ++n) {
                if (grid_[m][n].state == Sir::i || grid_[m][n].state == Sir::r) {
                    ++number_infected;
                }
            }
        }
        for (int l = 1; l < dimension_ - 1; ++l) {
            for (int c = 1; c < dimension_ - 1; ++c) {
                if (grid_[l][c].state == Sir::s) {
                    for (int j = -1; j <= 1; ++j) {
                        for (int i = -1; i <= 1; ++i) {
                            if (grid_[l + j][c + i].state == Sir::i && grid_[l + j][c + i].state != Sir::s) {
                                grid_[l][c].inf_prob += beta_; //somma delle probabilita di infettarsi attorno alla cella.
                            }
                            else {
                                grid_[l][c].inf_prob += 0;
                            }
                        }
                    }
                    if (grid_[l][c].inf_prob >= 1) {
                        end[l - 1][c - 1] = Sir::i;
                        grid_[l][c].inf_prob = 0;
                    }
                    else {
                        end[l - 1][c - 1] = Sir::s;
                    }
                }
                else if (grid_[l][c].state == Sir::i) {
                    int teoretical_rescued = (dimension_ - 2) * (dimension_ - 2) * beta_ * gamma_;
                    //int num = (dimension_*dimension_* gamma_);
                    int num = 9;
                    int factor = (rand() + time(0)) % num;
                    if (factor == num - 1 && day > 6) { //factor e days li ho messi liberamente uguali a questi valori && (number_infected * gamma_ < teoretical_rescued)
                        end[l - 1][c - 1] = Sir::r;
                    }
                    else {
                        end[l - 1][c - 1] = Sir::i;
                    }
                }
                else if (grid_[l][c].state == Sir::r) {
                    end[l - 1][c - 1] = Sir::r;
                }
            }
        }
        ++day;
        copy_(end);
    }
    //Funzione per spostamenti a lungo raggio
    void airplane_() {
        for (int l = 1; l <= (dimension_-3)/2; ++l) {
            for (int c = 1; c < dimension_-1; ++c) {
                int chance = (rand() + time(0)) % 1000; //probabilità del 0.1% di spostarsi a lungo raggio
                if (chance == 0) {
                    int newl = 1 + (rand() + time(0)) % ((dimension_ - 3) / 2);     // 1 <= newl <= coun_dim
                    int newc = 1 + (rand() + time(0)) % (dimension_ - 2);           // 1 <= newc <= dim_-2
                    
                    if (grid_[newl][newc].state != Sir::b && grid_[l][c].state != Sir::b) {
                        
                        auto old = new Cell;
                        *old = grid_[l][c];
                        grid_[l][c] = grid_[newl][newc];
                        grid_[newl][newc] = *old;
                        delete old;
                    }
                }
            }
        }
    }
    
    //Funzione per avare la grafica
    void draw(int refresh_time = 1) {
        float bit_size = 1.;
        //L'if di seguito setta di fatto un fattore di scala nel caso la board sia troppo piccola
        if (dimension_ < 100) {
            bit_size = 10.;
        }
        auto win_size = bit_size * dimension_;
        assert(win_size > 99 && win_size < 1001);
        sf::RenderWindow window(sf::VideoMode(win_size, win_size), "Disease", sf::Style::Close | sf::Style::Resize); //Creo una finestra (ogni cella = BIT_SIZE px)
        //Attivo il VSync (Max framerate = framerate del monitor utilizzato)
        window.setVerticalSyncEnabled(true);
        //Setto i bit dei quadratini e i rispettivi colori
        sf::RectangleShape sus_bit(sf::Vector2f(bit_size, bit_size));
        sf::RectangleShape inf_bit(sf::Vector2f(bit_size, bit_size));
        sf::RectangleShape rec_bit(sf::Vector2f(bit_size, bit_size));
        sf::RectangleShape bound_bit(sf::Vector2f(bit_size, bit_size));
        //sf::RenderWindow window2(sf::VideoMode(win_size, win_size), "Disease2", sf::Style::Close | sf::Style::Resize);
        sus_bit.setFillColor(sf::Color::Green);
        inf_bit.setFillColor(sf::Color::Red);
        rec_bit.setFillColor(sf::Color::Blue);
        bound_bit.setFillColor(sf::Color::Black);

        sf::Clock clock;
        sf::Time time;
        while (window.isOpen()) {
            //Gestione eventi
            sf::Event evnt;
            while (window.pollEvent(evnt)) {
                //Se clicco la X chiude la finestra
                if (evnt.type == sf::Event::Closed) {
                    window.close();
                }
            }
            
            time = clock.getElapsedTime();
            if (time.asSeconds() > refresh_time)
            {
            std::cout << "Day " << day << "  number of infected = " << number_infected << '\n';
            evolve_();
            move_();
            airplane_();
            for (int l = 0; l < dimension_; ++l) { 
                for (int c = 0; c < dimension_; ++c) {
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
                    else if (grid_[l][c].state == Sir::b) {
                        bound_bit.setPosition(bit_size * c, bit_size * l);
                        window.draw(bound_bit);
                    }
                }
            }   
            clock.restart();
            }
            
            window.display();
        }
    }
};
#endif
