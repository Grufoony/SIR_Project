#include"board.hpp"
#include<cassert>
#include<random>
#include<ctime>
#include <thread>
#include<chrono>
#include<stdexcept>
#include <SFML/Graphics.hpp>
#include<algorithm>
#include<cmath>

// q_prob_ è la probabilità che lo stato trovi un malato e lo metta in quarantena 
 sir::Board::Board(std::string c, int n, double b, double y, int inf, Mode adv_opt, double q_prob, Quarantine_parameters quarantine) :
    grid_(n + 2, std::vector<Cell>(n + 2)), dimension_{ n + 2 }, q_prob_{ q_prob },
     beta_{ b }, gamma_{ y }, advanced_opt_{ adv_opt }, name_{ c }, day_{ 0 }{
    //Verifico la coerenza dei dati
    if (n < 149  || n>401) {
        throw std::runtime_error{ "Error: the dimension is too tiny to see any simulation effect."
            "The minimum dimension is 100" };
    }
    if ((int)advanced_opt_ >= 4)
        assert(quarantine.first_day != 0 && quarantine.last_day != 0);
    if ((int)advanced_opt_ == 3 || (int)advanced_opt_ == 5)
        assert(static_cast<int>(q_prob_ * 1000) != 0);
    assert(b > 0 && b < 1);
    assert(y > 0 && y < 1);
    assert(q_prob >= 0 && q_prob <= 1);
    assert(inf > 0);
    for (int i = 0; i < inf; ++i) {
        int ran1 = gen_unif_rand_number(dimension_-1);
        int ran2 = gen_unif_rand_number(dimension_-1);
        grid_[ran1][ran2].state = Sir::i;
    }
    quarantin_.len_line = dimension_ / 2;
    quarantin_.first_day = quarantine.first_day;
    quarantin_.last_day = quarantine.last_day;
    counter_.num_i = inf;
};
    Sir& sir::Board::operator()(int riga, int column) {
        return (grid_[riga + 1][column + 1].state);
    }
    void sir::Board::evolve_() {
        counter_.num_i = 0;
        counter_.num_s = 0;
        counter_.num_r = 0;
        counter_.num_q = 0;
        std::vector<std::vector<Sir>> grid_support(dimension_ - 2, std::vector<Sir>(dimension_ - 2));
        int teoretical_ill = static_cast<int>(dimension_ * beta_);
        int malati_trovati = static_cast<int>(teoretical_ill * q_prob_);
        for (int l = 1; l < dimension_ - 1; ++l) {
            for (int c = 1; c < dimension_ - 1; ++c) {
                if (grid_[l][c].state == Sir::s) {
                    for (int j = -1; j <= 1; ++j) {
                        for (int i = -1; i <= 1; ++i) {
                            if (grid_[l + j][c + i].state == Sir::i && grid_[l + j][c + i].state != Sir::s) {
                                //counter += 1;
                                grid_[l][c].inf_prob +=beta_; //somma delle probabilita di infettarsi attorno alla cella.
                                //quando tale probabilità diventa uno la cella si ammala
                            }
                            else {
                                grid_[l][c].inf_prob += 0;
                            }
                        }
                    }
                    //grid_[l][c].inf_prob += 1-static_cast<double>(Newton_bin(8,counter) * pow(beta_,counter) * pow((1 - beta_),8-counter));
                    if (grid_[l][c].inf_prob >= 1) {
                        ++grid_support[l - 1][c - 1];
                        grid_[l][c].inf_prob = 0;
                        ///counter = 0;
                    }
                    else {
                        grid_support[l - 1][c - 1]=Sir::s;
                        //counter = 0;
                    }
                }
                //metodo di greg corretto: quando una persona è malata o guarisce però dopo 14 giorni ,oppure viene messa in quarantena prima di guarire.
                //definisco un intervallo( teoretical ill), genero un numero fra 0 e il  sup di tale intervallo.
                //matati trovati è un numero dipendete da q_prob_ compreso fra zero ed il precedente sup. da qui si comprende l if.
                // deve introdurre un opzione avanzata il fatto che ci si possa quarantenare. Fusco deve agire qui
                else if (grid_[l][c].state == Sir::i) {
                    int n = gen_unif_rand_number(teoretical_ill);
                    if (n < malati_trovati && day_>15 && ((int)advanced_opt_== 3 || (int)advanced_opt_== 5)) {
                        grid_support[l - 1][c - 1] = Sir::q;
                    }
                    else {
                        grid_[l][c].clock += 1;
                        if (grid_[l][c].clock > 14) {
                            ++(++grid_support[l - 1][c - 1]);
                            grid_[l][c].clock = 0;
                        }
                        else {
                            ++grid_support[l - 1][c - 1];
                        }
                    }
                }
                else if (grid_[l][c].state == Sir::r) {
                    ++(++grid_support[l - 1][c - 1]);
                }
                //questo if è per l ultima opzione avanzata,dove ho il borod della quarantena.
                else if (grid_[l][c].state == Sir::q_edge) {
                    if (day_ < quarantin_.last_day)
                        grid_support[l - 1][c - 1] = Sir::q_edge;
                    else
                        grid_support[l - 1][c - 1]=Sir::i;
                }
                // quando un malatom viene trovato malato viene messo in quarantena. Ci sta per 40 giorni poi è libero. dve essere un opzione avanzata di fusco.
                else if (grid_[l][c].state == Sir::q) {
                    if (grid_[l][c].clock == 40) {
                        grid_support[l - 1][c - 1] = Sir::r;
                        grid_[l][c].clock = 0;
                    }
                    else {
                        ++grid_[l][c].clock;
                        grid_support[l - 1][c - 1] = Sir::q;
                    }
                }

            }
            counter_.num_s += std::count(grid_support[l - 1].begin(), grid_support[l - 1].end(), Sir::s);
            counter_.num_i += std::count(grid_support[l - 1].begin(), grid_support[l - 1].end(), Sir::i);
            counter_.num_r += std::count(grid_support[l - 1].begin(), grid_support[l - 1].end(), Sir::r);
            counter_.num_q += std::count(grid_support[l - 1].begin(),grid_support[l - 1].end(), Sir::q);
        }
        counter_quarantene_infected();
        ++day_;
        copy_(grid_support);
    }
    int sir::Board::gen_unif_rand_number(int num) const{
        int x;
        std::random_device dev{};
        std::mt19937 gen{ dev() };
        std::uniform_int_distribution<int> dis(0, num);
        x = dis(gen);
        return x;
    }
    void sir::Board::counter_quarantene_infected() {
        q_counter_.num_s = 0;
        q_counter_.num_i = 0;
        q_counter_.num_r = 0;
        for (int line = quarantin_.len_line; line < quarantin_.len_line * 2; ++line) {
            for (int colon = quarantin_.len_line; colon < quarantin_.len_line * 2; ++colon) {
                if (grid_[line][colon].state == Sir::s) {
                    ++q_counter_.num_s;
                }
                else if (grid_[line][colon].state == Sir::r) {
                    ++q_counter_.num_r;
                }
                else {
                    ++q_counter_.num_i;
                }
            }
        }
    }
    // funzione che definisce una sola volta i bordi per la quarantena.
//LI dichiara visibili in modo tale che draw stampi il bordo nero dell'opzione di pivi.
    void sir::Board::quarantine_() {
        for (int l = 1; l < dimension_; ++l) {
            for (int c = 1; c < dimension_; ++c) {
                if ((((l == quarantin_.len_line || l == quarantin_.len_line * 2) || 
                    (c == quarantin_.len_line * 2 || c == quarantin_.len_line)) &&
                    (l >= quarantin_.len_line && l <= quarantin_.len_line * 2) && 
                    (c >= quarantin_.len_line && c <= quarantin_.len_line * 2)))
                    grid_[l][c].state = Sir::q_edge;
            }

        }
    }
    // avevo questa sul mio pc, se volete usare la vostra a me va bene.Non la commento perchè l avete fatta voi.
    void sir::Board::move_() {
        for (int l = 1; l < dimension_ - 2; ++l) {
            for (int c = 1; c < dimension_ - 2; ++c) {
                if (grid_[l][c].state != Sir::q && grid_[l][c].state != Sir::q_edge) {
                    int swap; // indicatore, se ha valore da 0 a 7 fa scambiare la cella con una delle 8 adiacenti
                    int newc, newl; //indici della cella da scambiare
                    swap = gen_unif_rand_number(13); //probabilità di circa il 60% che si sposti
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
                    if (swap < 8 && grid_[newl][newc].state != Sir::q && grid_[l][c].state != Sir::q 
                        && grid_[newl][newc].state != Sir::q_edge && grid_[l][c].state != Sir::q_edge) {
                        std::swap(grid_[l][c], grid_[newl][newc]);
                    }
                }
            }
        }
    }
    void sir::Board::airplane_() {
        //se ho l'opzione avanzata della quarantena di Pivi allora ho opzioni avanzate.Fusco da implementare l'enum deve aggiungere qui l opzione 5.
        if (day_ > quarantin_.first_day && day_ < quarantin_.last_day && (int)advanced_opt_ >= 4) {
            for (int l = 1; l < dimension_; ++l) {
                for (int c = 1; c < dimension_; ++c) {
                    if (((l < quarantin_.len_line || l > quarantin_.len_line * 2) || (c < quarantin_.len_line || c > quarantin_.len_line * 2))) {
                        int column = gen_unif_rand_number(dimension_ - 1);
                        int line = gen_unif_rand_number(dimension_ - 1);
                        // ci ho messo 3 ore per fare questo if. Non toccatelo please ve lo spiego in chiamata.
                        if (((grid_[l][c].state != Sir::q_edge && grid_[column][line].state != Sir::q_edge) ||
                            (grid_[l][c].state != Sir::q && grid_[column][line].state != Sir::q))
                            && ((line <  quarantin_.len_line || line > quarantin_.len_line * 2) ||
                                (column < quarantin_.len_line || column > quarantin_.len_line * 2))) {
                            if (line == 1) {
                                std::swap(grid_[l][c], grid_[line][column]);
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
                    int column = gen_unif_rand_number(dimension_ - 1);
                    int line = gen_unif_rand_number(dimension_ - 1);
                    if (grid_[l][c].state != Sir::q_edge && grid_[column][line].state != Sir::q_edge) {
                        if (line == 1) {
                            std::swap(grid_[l][c], grid_[line][column]);
                        }
                    }
                }
            }
        }
    }
    void sir::Board::copy_(std::vector<std::vector<Sir>>& end) {
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

    void sir::Board::draw(int& millisecondi) {
        float bit_size = 1.;
        int win_size = static_cast<int>(bit_size * dimension_);
        assert(win_size > 99 && win_size < 1001);
        sf::RenderWindow window(sf::VideoMode(win_size + static_cast<int>((2 * win_size / 3)), win_size), name_, sf::Style::Close | sf::Style::Resize);
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

        //vettori importanti per i grafici
        Counter q_graph_points;
        Counter graph_points;
        int proportion = dimension_ - ((quarantin_.len_line * quarantin_.len_line) / dimension_);

        std::cout << "day**********" << "number_infected" << '\n';
        sf::Clock clock;
        sf::Time time;
        bool flag = true;
        while (window.isOpen())
        {           
            //window.clear(sf::Color::Black);
            if (day_ >= quarantin_.first_day && flag && (int)advanced_opt_ >= 4) {
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
            if (time.asMilliseconds() > millisecondi) {
                std::cout << "" << day_+1 << "            " << counter_.num_i + q_counter_.num_i << '\n';
                evolve_();               
                if ((int)advanced_opt_ >= 1) {
                    move_();
                }
                if ((int)advanced_opt_ >= 2) {
                    airplane_();
                }
                clock.restart();
               
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
                if ((int)advanced_opt_ >= 4) {   
                    q_graph_points.num_s= static_cast<int> (q_counter_.num_s / (quarantin_.len_line));
                    q_graph_points.num_i= static_cast<int> (q_counter_.num_i / (quarantin_.len_line));
                    q_graph_points.num_r= static_cast<int> (q_counter_.num_r / (quarantin_.len_line));
                    graph_points.num_s= static_cast<int>((counter_.num_s - q_counter_.num_s) * 0.5 / (proportion));
                    graph_points.num_i= static_cast<int>((counter_.num_i + counter_.num_q - q_counter_.num_i) * 0.5 / (proportion));
                    graph_points.num_r= static_cast<int>((counter_.num_r - q_counter_.num_r) * 0.5 / (proportion));
                    graph_in_quarantine_.push_back({ q_graph_points });
                    graph_out_quarantine_.push_back({ graph_points });
                    for (int i = dimension_ + 1; i < (5 * dimension_ * bit_size / 3); ++i) {
                        board.setPosition(i * bit_size, static_cast<float>(dimension_ / 2));
                        window.draw(board);
                        // liberi grafico
                        for (int counter = 0; counter != graph_out_quarantine_.size(); ++counter) {
                            if (graph_out_quarantine_[counter].num_i <= 1) {
                                inf_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_ / 2) - 3);
                                window.draw(inf_bit);
                            }
                            else {
                                inf_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ / 2 - graph_out_quarantine_[counter].num_i) - 3);
                                window.draw(inf_bit);
                            }
                            if (graph_out_quarantine_[counter].num_s <= 1) {
                                sus_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_ / 2) - 1);
                                window.draw(sus_bit);
                            }
                            else {
                                sus_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ / 2 - graph_out_quarantine_[counter].num_s) + 1);
                                window.draw(sus_bit);
                            }
                            if (graph_out_quarantine_[counter].num_r <= 1) {
                                rec_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_ / 2) - 2);
                                window.draw(rec_bit);
                            }
                            else {
                                rec_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ / 2 - graph_out_quarantine_[counter].num_r) - 2);
                                window.draw(rec_bit);
                            }
                            // quarantenati stato
                            if (graph_in_quarantine_[counter].num_i <= 1) {
                                inf_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 2);
                                window.draw(inf_bit);
                            }
                            else {
                                inf_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_) - graph_in_quarantine_[counter].num_i + 1);
                                window.draw(inf_bit);
                            }
                            if (graph_in_quarantine_[counter].num_s <= 1) {
                                sus_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 1);
                                window.draw(sus_bit);
                            }
                            else {
                                sus_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - graph_in_quarantine_[counter].num_s + 2));
                                window.draw(sus_bit);
                            }
                            if (graph_in_quarantine_[counter].num_r <= 1) {
                                rec_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 1);
                                window.draw(rec_bit);
                            }
                            else {
                                rec_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - graph_in_quarantine_[counter].num_r) + 1);
                                window.draw(rec_bit);
                            }

                        }
                    }
                }
                // questo grafico si attiva in tutti gli altri casi.
                else {
                     graph_points.num_i = static_cast<int>((counter_.num_i+counter_.num_q) / dimension_);
                     graph_points.num_s = static_cast<int>(counter_.num_s / dimension_);
                     graph_points.num_r = static_cast<int>(counter_.num_r / dimension_);
                     graph_out_quarantine_.push_back({graph_points});
                    for (int counter = 0; counter != graph_out_quarantine_.size(); ++counter) {
                        if (graph_out_quarantine_[counter].num_i <= 1) {
                            inf_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 2);
                            window.draw(inf_bit);
                        }
                        else {
                            inf_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - graph_out_quarantine_[counter].num_i) + 1);
                            window.draw(inf_bit);
                        }
                        if (graph_out_quarantine_[counter].num_s <= 1) {
                            sus_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 1);
                            window.draw(sus_bit);
                        }
                        else {
                            sus_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - graph_out_quarantine_[counter].num_s) + 1);
                            window.draw(sus_bit);
                        }
                        if (graph_out_quarantine_[counter].num_r <= 1) {
                            rec_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 1);
                            window.draw(rec_bit);
                        }
                        else {
                            rec_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - graph_out_quarantine_[counter].num_r) + 1);
                            window.draw(rec_bit);
                        }
                    }                  
                }
                window.display();
                if (day_ > 250) {
                    window.close();
                }              
                std::this_thread::sleep_for(std::chrono::milliseconds(millisecondi));
                window.clear(sf::Color::Black);
            }
        }
    }
