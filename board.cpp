#include"board.hpp"
#include <algorithm>
#include<cassert>
#include<random>
#include<ctime>
#include<stdexcept>
#include <SFML/Graphics.hpp>

// q_prob_ è la probabilità che lo stato trovi un malato e lo metta in quarantena 
    Board::Board(std::string c,int n, double b, double y,double q_prob,int inf, Quarantene_parameters quarantene,Mode adv_opt) : grid_(n + 2, std::vector<Cell>(n + 2)){
        //Verifico la coerenza dei dati
        if (n < 100) {
            throw std::runtime_error{ "Error: the dimension is too tiny for see any simulation  effect."
                "The minim dimension is 100" };
        }
        assert(b > 0 && b < 1);
        assert(y > 0 && y < 1);
        assert(q_prob > 0 && q_prob < 1);
        assert(inf > 0);
        name_ = c;
        dimension_ = n + 2;
        q_prob_ = q_prob;
        beta_ = b;
        gamma_ = y;
        advanced_opt_ = adv_opt;
        for (int i = 0; i < inf; ++i) {
            int ran1 = (std::rand() + time(0)) % dimension_;
            int ran2 = (std::rand() + time(0)) % dimension_;
            grid_[ran1+1][ran2+1].state = Sir::i;
        }
        quaranten.len_line = dimension_ / 2;
        quaranten.first_day = quarantene.first_day;
        quaranten.last_day = quarantene.last_day;
        day_ = 0;
    };
    Sir& Board::operator()(int riga, int column) {
        return (grid_[riga + 1][column + 1].state);
    }
    //Nuovo algortimo, genero un numero casuale tra 0 e 100, se questo numero è minore della probabilità attesa (beta, gamma, q), allora applico il cambiamento alla cella
    void Board::evolve_() {
        auto temp = grid_;
        for(int l = 1; l < dimension_-1; ++l) {
            for(int c = 1; c < dimension_-1; ++c) {
                if(temp[l][c].state == Sir::s) {
                    for (int j = -1; j <= 1; ++j) {
                        for (int i = -1; i <= 1; ++i) {
                            if (temp[l + j][c + i].state == Sir::i) {
                                int ran = ((rand() + time(0)) % 101) -1;
                                if(ran < (int)beta_*100) {
                                    ++grid_[l][c].state;
                                }                               
                            }
                        }
                    }
                } else if(temp[l][c].state == Sir::i) {
                    int ran = ((rand() + time(0)) % 101) -1;
                    if(ran < (int)gamma_*100) {
                        ++grid_[l][c].state;
                    } else {
                        ran = ((rand() + time(0)) % 101) -1;
                        if(ran < (int)q_prob_*100) {
                            grid_[l][c].state = Sir::q;
                        }
                    }
                } else if(temp[l][c].state == Sir::q) {
                    if(temp[l][c].clock == 40) {
                        ++grid_[l][c].state;
                    } else {
                        ++grid_[l][c].clock;
                    }
                }
            }
        }
        counter_infected();
        counter_quarantene_infected();
        ++day_;
        //copy_(end);
    }
    // funzione che definisce una sola volta i bordi per la quarantena.
    //LI dichiara visibili in modo tale che draw stampi il bordo nero dell'opzione di pivi.
    void Board::quarantene_() {
        for (int l = 1; l < dimension_; ++l) {
            for (int c = 1; c < dimension_; ++c) {
                if ((((l == quaranten.len_line || l == quaranten.len_line * 2) || 
                    (c == quaranten.len_line * 2 || c == quaranten.len_line)) &&
                    (l >= quaranten.len_line && l <= quaranten.len_line * 2) && 
                    (c >= quaranten.len_line && c <= quaranten.len_line * 2)))
                    grid_[l][c].state = Sir::q_edge;
            }

        }
    }
    // avevo questa sul mio pc, se volete usare la vostra a me va bene.Non la commento perchè l avete fatta voi.
    void Board::move_() {
        for (int line = 1; line < dimension_ - 2; ++line) {
            for (int column = 1; column < dimension_ - 2; ++column) {
                int change_line = (rand() + time(0)) % 3;
                int change_column = (rand() + time(0)) % 3;//ricorda il meno due;
                int one_of_tree = (rand() + time(0)) % 9;
                if (one_of_tree == 3 && ((grid_[line][column].state != (Sir::q_edge) &&
                    grid_[line + change_line - 1][column + change_column - 1].state != (Sir::q_edge)) &&
                    grid_[line][column].state != Sir::q && grid_[line + change_line - 1][column + change_column - 1].state != Sir::q)) {
                    std::unique_ptr<Cell> old = std::make_unique<Cell>();
                    *old = grid_[line][column];
                    grid_[line][column] = grid_[line + change_line - 1][column + change_column - 1];
                    grid_[line + change_line - 1][column + change_column - 1] = *old;
                }
            }
        }
    }
    void Board::airplane_() {
        //creo una funzione lambda che mi creerà una cella sullo hype per permettermi di muoverle.
        auto create_cell = [&](int l, int c, int column, int line) {
            std::unique_ptr<Cell> move = std::make_unique<Cell>();
            *move = grid_[l][c];
            move->state = grid_[column][line].state;
            move->inf_prob = grid_[column][line].inf_prob;
            grid_[column][line].state = grid_[l][c].state;
            grid_[column][line].inf_prob = grid_[l][c].inf_prob;
            grid_[l][c] = *move;
        };
        //se ho l'opzione avanzata della quarantena di Pivi allora ho opzioni avanzate.Fusco da implementare l'enum deve aggiungere qui l opzione 5.
        if (day_ > quaranten.first_day && day_ < quaranten.last_day && (int)advanced_opt_ >= 4) {
            for (int l = 1; l < dimension_; ++l) {
                for (int c = 1; c < dimension_; ++c) {
                    if (((l < quaranten.len_line || l > quaranten.len_line * 2) || (c < quaranten.len_line || c > quaranten.len_line * 2))) {
                        int column = (rand() + time(0)) % dimension_;
                        int riga = (rand() + time(0)) % dimension_;
                        // ci ho messo 3 ore per fare questo if. Non toccatelo please ve lo spiego in chiamata.
                        if (((grid_[l][c].state != Sir::q_edge && grid_[column][riga].state != Sir::q_edge) ||
                            (grid_[l][c].state != Sir::q && grid_[column][riga].state != Sir::q))
                            && ((riga <  quaranten.len_line || riga > quaranten.len_line * 2) ||
                                (column < quaranten.len_line || column > quaranten.len_line * 2))) {
                            if (riga == 1) {
                                create_cell(l, c, column, riga);
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
                            create_cell(l, c, column, line);
                        }
                    }
                }
            }
        }
    }
    void Board::counter_quarantene_infected() {
        q_counter.num_s = 0;
        q_counter.num_i = 0;
        q_counter.num_r = 0;
        for (int line = quaranten.len_line; line < quaranten.len_line * 2; ++line) {
            for (int colon = quaranten.len_line; colon < quaranten.len_line * 2; ++colon) {
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
                //if((m || n) != quaranten.len_line){
                if (grid_[m][n].state == Sir::s) {
                    ++counter.num_s;
                }
                else if (grid_[m][n].state == Sir::r) {
                    ++counter.num_r;
                }
                else {
                    ++counter.num_i;
                }
                //}
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
void Board::draw(int& refresh_rate) {
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

        if (day_ >= quaranten.first_day && flag && (int)advanced_opt_>=4) {
            quarantene_();
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
        if (time.asMilliseconds() > refresh_rate) {
       
            evolve_();
            if ((int)advanced_opt_ >=1) {
                move_();
            }
            if ((int)advanced_opt_ >=2) {
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
            if ((int)advanced_opt_>=4) {
                int q_point_s = static_cast<int> (q_counter.num_s  / (quaranten.len_line));
                int q_point_i = static_cast<int> (q_counter.num_i  / (quaranten.len_line));
                int q_point_r = static_cast<int> (q_counter.num_r  / (quaranten.len_line));
                int proportion = dimension_ - ((quaranten.len_line * quaranten.len_line) / dimension_);
                int graph_point_i = static_cast<int>((counter.num_i-q_counter.num_i)*0.5 /proportion);
                int graph_point_s =static_cast<int>((counter.num_s-q_counter.num_s)*0.5/(proportion));
                int graph_point_r = static_cast<int>((counter.num_r-q_counter.num_r)*0.5 /(proportion));
                grafico_in_quarantene.push_back({ q_point_s,q_point_i,q_point_r });
                grafico_out_quarantene.push_back({ graph_point_s , graph_point_i , graph_point_r });
                //confine fra i due grafici.
                for (int i = dimension_ + 1; i < (5 * dimension_*bit_size / 3); ++i) {
                    board.setPosition(i * bit_size, static_cast<float>(dimension_ / 2));
                    window.draw(board);
                    // liberi grafico
                    for (int counter = 0; counter != (int)grafico_out_quarantene.size(); ++counter) {
                        if (grafico_out_quarantene[counter].num_i <= 1) {
                            inf_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_ / 2) - 3);
                            window.draw(inf_bit);
                        }
                        else {
                            inf_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ / 2 - grafico_out_quarantene[counter].num_i)-3);
                            window.draw(inf_bit);
                        }
                        if (grafico_out_quarantene[counter].num_s <= 1) {
                            sus_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_ / 2) - 1);
                            window.draw(sus_bit);
                        }
                        else {
                            sus_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ / 2 - grafico_out_quarantene[counter].num_s) + 1);
                            window.draw(sus_bit);
                        }
                        if (grafico_out_quarantene[counter].num_r <= 1) {
                            rec_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_ / 2) - 2);
                            window.draw(rec_bit);
                        }
                        else {
                            rec_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ / 2 - grafico_out_quarantene[counter].num_r) -2);
                            window.draw(rec_bit);
                        }
                        // quarantenati stato
                            if (grafico_in_quarantene[counter].num_i <= 1) {
                                inf_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 2);
                                window.draw(inf_bit);
                            }
                            else {
                                inf_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_)- grafico_in_quarantene[counter].num_i + 1);
                                window.draw(inf_bit);
                            }
                            if (grafico_in_quarantene[counter].num_s <= 1) {
                                sus_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 1);
                                window.draw(sus_bit);
                            }
                            else {
                                sus_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - grafico_in_quarantene[counter].num_s+2));
                                window.draw(sus_bit);
                            }
                            if (grafico_in_quarantene[counter].num_r <= 1) {
                                rec_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 1);
                                window.draw(rec_bit);
                            }
                            else {
                                rec_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - grafico_in_quarantene[counter].num_r) + 1);
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
                    grafico_out_quarantene.push_back({ graph_point_s , graph_point_i , graph_point_r });
                    for (int counter = 0; counter != (int)grafico_out_quarantene.size(); ++counter) {
                        if (grafico_out_quarantene[counter].num_i <= 1) {
                            inf_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 2);
                            window.draw(inf_bit);
                        }
                        else {
                            inf_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - grafico_out_quarantene[counter].num_i) + 1);
                            window.draw(inf_bit);
                        }
                        if (grafico_out_quarantene[counter].num_s <= 1) {
                            sus_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 1);
                            window.draw(sus_bit);
                        }
                        else {
                            sus_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - grafico_out_quarantene[counter].num_s) + 1);
                            window.draw(sus_bit);
                        }
                        if (grafico_out_quarantene[counter].num_r <= 1) {
                            rec_bit.setPosition(static_cast<float>(dimension_ + counter), static_cast<float>(dimension_) - 1);
                            window.draw(rec_bit);
                        }
                        else {
                            rec_bit.setPosition(static_cast<float>(dimension_ + counter + 1), static_cast<float>(dimension_ - grafico_out_quarantene[counter].num_r) + 1);
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