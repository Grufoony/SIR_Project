#include "board.h"
#include<cassert>
#include<random>
#include<ctime>
#include<stdexcept>
#include<algorithm>
#include <SFML/Graphics.hpp>
// q_prob_ è la probabilità che lo stato trovi un malato e lo metta in quarantena 
Board::Board(std::string c, int n, double b, double y, int inf, Mode adv_opt, double q_prob, Quarantine_parameters quarantine ) :
    grid_(n + 2, std::vector<Cell>(n + 2)), dimension_{ n + 2 }, q_prob_{ q_prob },
    beta_{ b }, gamma_{ y }, advanced_opt{ adv_opt }, name_{ c }{
    //Verifico la coerenza dei dati
    if (n < 100) {
        throw std::runtime_error{ "Error: the dimension is too tiny to see any simulation effect."
            "The minimum dimension is 100" };
    }
    if ((int)advanced_opt >= 4)
        assert(quarantine.first_day != 0 && quarantine.last_day != 0);
    if ((int)advanced_opt == 3 || (int)advanced_opt == 5)
        assert(static_cast<int>(q_prob_ * 1000) != 0);
    assert(b > 0 && b < 1);
    assert(y > 0 && y < 1);
    assert(q_prob > 0 && q_prob < 1);
    assert(inf > 0);
    for (int i = 0; i < inf; ++i) {
        int ran1 = (std::rand() + time(0)) % dimension_;
        int ran2 = (std::rand() + time(0)) % dimension_;
        grid_[ran1 + 1][ran2 + 1].state = Sir::i;
    }
    quarantin.len_line = dimension_ / 2;
    quarantin.first_day = quarantine.first_day;
    quarantin.last_day = quarantine.last_day;
    day_ = 0;
};
Sir& Board::operator()(int line, int column) {
    return (grid_[line + 1][column + 1].state);
}
void Board::evolve_() {
    std::vector<std::vector<Sir>> end(dimension_ - 2, std::vector<Sir>(dimension_ - 2));
    int teoretical_ill = static_cast<int>(dimension_ * beta_);
    int ill_found = static_cast<int>(teoretical_ill * q_prob_);
    for (int l = 1; l < dimension_ - 1; ++l) {
        for (int c = 1; c < dimension_ - 1; ++c) {
            if (grid_[l][c].state == Sir::s) {
                for (int j = -1; j <= 1; ++j) {
                    for (int i = -1; i <= 1; ++i) {
                        if (grid_[l + j][c + i].state == Sir::i && grid_[l + j][c + i].state != Sir::s) {
                            grid_[l][c].inf_prob += beta_; //somma delle probabilita di infettarsi attorno alla cella.
                            //quando tale probabilità diventa uno la cella si ammala
                        }
                        else {
                            grid_[l][c].inf_prob += 0;
                        }
                    }
                }
                if (grid_[l][c].inf_prob >= 1) {
                    ++end[l - 1][c - 1];
                    grid_[l][c].inf_prob = 0;
                }
                else {
                    end[l - 1][c - 1];
                }
            }
            //metodo di greg corretto: quando una persona è malata o guarisce però dopo 14 giorni ,oppure viene messa in quarantena prima di guarire.
            //definisco un intervallo( teoretical ill), genero un numero fra 0 e il  sup di tale intervallo.
            //matati trovati è un numero dipendete da q_prob_ compreso fra zero ed il precedente sup. da qui si comprende l if.
            
            else if (grid_[l][c].state == Sir::i) {
                int n = (rand() + time(0)) % (teoretical_ill);
                if (n < ill_found && day_>10 && ((int)advanced_opt == 3 || (int)advanced_opt == 5)) {
                    end[l - 1][c - 1] = Sir::q;
                }
                else {
                    grid_[l][c].clock += 1;
                    if (grid_[l][c].clock > 14) {
                        ++(++end[l - 1][c - 1]);
                        grid_[l][c].clock = 0;
                    }
                    else {
                        ++end[l - 1][c - 1];
                    }
                }
            }
            else if (grid_[l][c].state == Sir::r) {
                ++(++end[l - 1][c - 1]);
            }
            //questo if è per l ultima opzione avanzata,dove ho il borod della quarantena.
            else if (grid_[l][c].state == Sir::q_edge) {
                if (day_ < quarantin.last_day)
                    end[l - 1][c - 1] = Sir::q_edge;
                else
                    end[l - 1][c - 1];
            }
            // quando un malato viene trovato malato viene messo in quarantena. Ci sta per 40 giorni poi è libero.
            else if (grid_[l][c].state == Sir::q) {
                if (grid_[l][c].clock == 40) {
                    end[l - 1][c - 1] = Sir::r;
                    grid_[l][c].clock = 0;
                }
                else {
                    ++grid_[l][c].clock;
                    end[l - 1][c - 1] = Sir::q;
                }
            }

        }
    }
    counter_infected();
    counter_quarantine_infected();
    ++day_;
    copy_(end);
}
// funzione che definisce una sola volta i bordi per la quarantena.
//LI dichiara visibili in modo tale che draw stampi il bordo nero dell'opzione di pivi.
void Board::quarantine_() {
    for (int l = 1; l < dimension_; ++l) {
        for (int c = 1; c < dimension_; ++c) {
            if ((((l == quarantin.len_line || l == quarantin.len_line * 2) ||
                (c == quarantin.len_line * 2 || c == quarantin.len_line)) &&
                (l >= quarantin.len_line && l <= quarantin.len_line * 2) &&
                (c >= quarantin.len_line && c <= quarantin.len_line * 2)))
                grid_[l][c].state = Sir::q_edge;
        }

    }
}
// avevo questa sul mio pc, se volete usare la vostra a me va bene.Non la commento perchè l avete fatta voi.
void Board::move_() {
    for (int l = 1; l < dimension_ - 2; ++l) {
        for (int c = 1; c < dimension_ - 2; ++c) {
            if (grid_[l][c].state != Sir::q && grid_[l][c].state != Sir::q_edge) {
                int swap; // indicatore, se ha valore da 0 a 7 fa scambiare la cella con una delle 8 adiacenti
                int newc, newl; //indici della cella da scambiare
                swap = (rand() + time(0)) % 13; //probabilità di circa il 60% che si sposti
                switch (swap) {
                case 0: newc = c - 1;   newl = l - 1;   break;
                case 1: newc = c;       newl = l - 1;   break;
                case 2: newc = c + 1;   newl = l - 1;   break;
                case 3: newc = c - 1;   newl = l;       break;
                case 4: newc = c + 1;   newl = l;       break;
                case 5: newc = c - 1;   newl = l + 1;   break;
                case 6: newc = c;       newl = l + 1;   break;
                case 7: newc = c + 1;   newl = l + 1;   break;
                default: break;
                }

                if (swap < 8 && grid_[newl][newc].state != Sir::q && grid_[l][c].state != Sir::q && grid_[newl][newc].state != Sir::q_edge && grid_[l][c].state != Sir::q_edge) {
                    std::unique_ptr<Cell> old = std::make_unique<Cell>();
                    *old = grid_[l][c];
                    grid_[l][c] = grid_[newl][newc];
                    grid_[newl][newc] = *old;
                }
            }
        }
    }
}
void Board::airplane_() {
    //creo una funzione lambda che mi creerà una cella sullo heap per permettermi di muoverle.
    auto swap_cell = [&](int l, int c, int column, int line) {
        std::unique_ptr<Cell> move = std::make_unique<Cell>();
        *move = grid_[l][c];
        move->state = grid_[column][line].state;
        move->inf_prob = grid_[column][line].inf_prob;
        grid_[column][line].state = grid_[l][c].state;
        grid_[column][line].inf_prob = grid_[l][c].inf_prob;
        grid_[l][c] = *move;
    };
    //se ho l'opzione avanzata della quarantena di Pivi allora ho opzioni avanzate.
    if (day_ > quarantin.first_day && day_ < quarantin.last_day && (int)advanced_opt >= 4) {
        for (int l = 1; l < dimension_; ++l) {
            for (int c = 1; c < dimension_; ++c) {
                if (((l < quarantin.len_line || l > quarantin.len_line * 2) || (c < quarantin.len_line || c > quarantin.len_line * 2))) {
                    int column = (rand() + time(0)) % dimension_;
                    int line = (rand() + time(0)) % dimension_;
                    // ci ho messo 3 ore per fare questo if. Non toccatelo please ve lo spiego in chiamata.
                    if (((grid_[l][c].state != Sir::q_edge && grid_[column][line].state != Sir::q_edge) ||
                        (grid_[l][c].state != Sir::q && grid_[column][line].state != Sir::q))
                        && ((line <  quarantin.len_line || line > quarantin.len_line * 2) ||
                            (column < quarantin.len_line || column > quarantin.len_line * 2))) {
                        if (line == 1) {
                            swap_cell(l, c, column, line);
                        }
                    }
                }
            }
        }
    }
    // se non ci sono le opzioni avanzate di PIvi.
    else {
        for (int l = 1; l < dimension_; ++l) {
            for (int c = 1; c < dimension_; ++c) {
                int column = (rand() + time(0)) % dimension_;
                int line = (rand() + time(0)) % dimension_;
                if (grid_[l][c].state != Sir::q_edge && grid_[column][line].state != Sir::q_edge) {
                    if (line == 1) {
                        swap_cell(l, c, column, line);
                    }
                }
            }
        }
    }
}
void Board::counter_quarantine_infected() {
    q_counter.num_s = 0;
    q_counter.num_i = 0;
    q_counter.num_r = 0;
    for (int line = quarantin.len_line; line < quarantin.len_line * 2; ++line) {
        for (int colon = quarantin.len_line; colon < quarantin.len_line * 2; ++colon) {
            if (grid_[line][colon].state == Sir::s) {
                ++q_counter.num_s;
            }
            else if (grid_[line][colon].state == Sir::r) {
                ++q_counter.num_r;
            }
            else {
                ++q_counter.num_i;
            }
        }
    }
}
//da implementare meglio.possono essere unite ad evolve.Oggi lo faccio.
void Board::counter_infected() {
    counter.num_i = 0;
    counter.num_s = 0;
    counter.num_r = 0;
    for (int m = 1; m < dimension_ - 1; ++m) {
        for (int n = 1; n < dimension_ - 1; ++n) {
            if (grid_[m][n].state == Sir::s) {
                ++counter.num_s;
            }
            else if (grid_[m][n].state == Sir::r) {
                ++counter.num_r;
            }
            else {
                ++counter.num_i;
            }
        }
    }
}
void Board::copy_(std::vector<std::vector<Sir>>& end) {
    for (int l = 1; l <= dimension_ - 1; ++l) {
        for (int c = 0; c <= dimension_ - 1; ++c) {
            if (l == 0 || c == 0 || l == dimension_ - 1 || c == dimension_ - 1)
            {
                grid_[l][c].state = Sir::s;
            }
            else {
                grid_[l][c].state = end[l - 1][c - 1];
            }
        }
    }
}
void Board::draw(int& milliseconds) {
    float bit_size = 1.f;
    int win_size = static_cast<int>(bit_size * dimension_);
    assert(win_size > 99 && win_size < 1001);
    sf::RenderWindow window(sf::VideoMode(win_size + static_cast<int>((2 * win_size / 3)), win_size),
        name_, sf::Style::Close | sf::Style::Resize);

    window.setVerticalSyncEnabled(true);
    //Setto i bit dei quadratini e i rispettivi colori
    sf::RectangleShape sus_bit(sf::Vector2f(bit_size, bit_size));
    sf::RectangleShape inf_bit(sf::Vector2f(bit_size, bit_size));
    sf::RectangleShape rec_bit(sf::Vector2f(bit_size, bit_size));
    sf::RectangleShape board(sf::Vector2f(bit_size, bit_size));
    sf::RectangleShape q_bit(sf::Vector2f(bit_size, bit_size));

    sus_bit.setFillColor(sf::Color::Green);
    inf_bit.setFillColor(sf::Color::Red);
    rec_bit.setFillColor(sf::Color::Blue);
    board.setFillColor(sf::Color::Magenta);
    q_bit.setFillColor(sf::Color::Black);

    std::cout << "day**********" << "number_infected" << '\n';
    sf::Clock clock;
    sf::Time time;
    bool flag = true;
    while (window.isOpen())
    {

        if (day_ >= quarantin.first_day && flag && (int)advanced_opt >= 4) {
            quarantine_();
            flag = false;
        }
        sf::Event evnt;

        while (window.pollEvent(evnt)) {
            if (evnt.type == sf::Event::Closed) {
                window.close();
            }
        }
        // questo if dovrà comprendere le funzioni avanzate di fusco.La funzione evolve,move e airplane devono attivarsi a seconde dell unum che gli passiamo.
    //scendi piu' in basso a leggere per il commento con i grafici.
        time = clock.getElapsedTime();
        if (time.asMilliseconds() > milliseconds) {

            evolve_();
            if ((int)advanced_opt >= 1) {
                move_();
            }
            if ((int)advanced_opt >= 2) {
                airplane_();
            }
            clock.restart();
            std::cout << "" << day_ + 1 << "            " << counter.num_i + q_counter.num_i << '\n';
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
                    else if (grid_[l][c].state == Sir::q_edge) {
                        board.setPosition(bit_size * c, bit_size * l);
                        window.draw(board);
                    }
                    else if (grid_[l][c].state == Sir::q) {
                        q_bit.setPosition(bit_size * c, bit_size * l);
                        window.draw(q_bit);
                    }
                }

            }

            // questo è il grafico che si attiva solo se la mia opzione( l'ultima) è attiva.
            if ((int)advanced_opt >= 4) {
                int q_point_s = static_cast<int> (q_counter.num_s / (quarantin.len_line));
                int q_point_i = static_cast<int> (q_counter.num_i / (quarantin.len_line));
                int q_point_r = static_cast<int> (q_counter.num_r / (quarantin.len_line));
                int proportion = dimension_ - ((quarantin.len_line * quarantin.len_line) / dimension_);
                int graph_point_i = static_cast<int>((counter.num_i - q_counter.num_i) * 0.5 / proportion);
                int graph_point_s = static_cast<int>((counter.num_s - q_counter.num_s) * 0.5 / (proportion));
                int graph_point_r = static_cast<int>((counter.num_r - q_counter.num_r) * 0.5 / (proportion));
                graph_in_quarantine.push_back({ q_point_s,q_point_i,q_point_r });
                graph_out_quarantine.push_back({ graph_point_s , graph_point_i , graph_point_r });
                //confine fra i due grafici.
                for (int i = dimension_ + 1; i < (5 * dimension_ * bit_size / 3); ++i) {
                    board.setPosition(i * bit_size, static_cast<float>(dimension_ / 2));
                    window.draw(board);
                    // liberi grafico
                    for (int counter = 0; counter != graph_out_quarantine.size(); ++counter) {
                        if (graph_out_quarantine[counter].num_i <= 1) {
                            inf_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_ / 2) - 3);
                            window.draw(inf_bit);
                        }
                        else {
                            inf_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ / 2 - graph_out_quarantine[counter].num_i) - 3);
                            window.draw(inf_bit);
                        }
                        if (graph_out_quarantine[counter].num_s <= 1) {
                            sus_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_ / 2) - 1);
                            window.draw(sus_bit);
                        }
                        else {
                            sus_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ / 2 - graph_out_quarantine[counter].num_s) + 1);
                            window.draw(sus_bit);
                        }
                        if (graph_out_quarantine[counter].num_r <= 1) {
                            rec_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_ / 2) - 2);
                            window.draw(rec_bit);
                        }
                        else {
                            rec_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ / 2 - graph_out_quarantine[counter].num_r) - 2);
                            window.draw(rec_bit);
                        }
                        // quarantenati stato
                        if (graph_in_quarantine[counter].num_i <= 1) {
                            inf_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 2);
                            window.draw(inf_bit);
                        }
                        else {
                            inf_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_) - graph_in_quarantine[counter].num_i + 1);
                            window.draw(inf_bit);
                        }
                        if (graph_in_quarantine[counter].num_s <= 1) {
                            sus_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 1);
                            window.draw(sus_bit);
                        }
                        else {
                            sus_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - graph_in_quarantine[counter].num_s + 2));
                            window.draw(sus_bit);
                        }
                        if (graph_in_quarantine[counter].num_r <= 1) {
                            rec_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 1);
                            window.draw(rec_bit);
                        }
                        else {
                            rec_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - graph_in_quarantine[counter].num_r) + 1);
                            window.draw(rec_bit);
                        }

                    }
                }
                window.display();
                if (day_ > 250) {
                    window.close();
                }
            }
            // questo grafico si attiva in tutti gli altri casi.
            else {
                int graph_point_i = static_cast<int>(counter.num_i / dimension_);
                int graph_point_s = static_cast<int>(counter.num_s / dimension_);
                int graph_point_r = static_cast<int>(counter.num_r / dimension_);
                // grafico_out_quarantene.num_i.push_back(punto_del_grafico);
                graph_out_quarantine.push_back({ graph_point_s , graph_point_i , graph_point_r });
                for (int counter = 0; counter != graph_out_quarantine.size(); ++counter) {
                    if (graph_out_quarantine[counter].num_i <= 1) {
                        inf_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 2);
                        window.draw(inf_bit);
                    }
                    else {
                        inf_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - graph_out_quarantine[counter].num_i) + 1);
                        window.draw(inf_bit);
                    }
                    if (graph_out_quarantine[counter].num_s <= 1) {
                        sus_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 1);
                        window.draw(sus_bit);
                    }
                    else {
                        sus_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - graph_out_quarantine[counter].num_s) + 1);
                        window.draw(sus_bit);
                    }
                    if (graph_out_quarantine[counter].num_r <= 1) {
                        rec_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 1);
                        window.draw(rec_bit);
                    }
                    else {
                        rec_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - graph_out_quarantine[counter].num_r) + 1);
                        window.draw(rec_bit);
                    }
                }
                window.display();
                if (day_ > 250) {
                    window.close();
                }
            }

        }
    }
}
