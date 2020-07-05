
#include"board.hpp"
#include<cassert>
#include<random>
#include<ctime>
#include<stdexcept>
// q_prob_ è la probabilità che lo stato trovi un malato e lo metta in quarantena 
    Board::Board(std::string c,int n, double b, double y,double q_prob,int inf, Quarantene_parameters quarantene,Mode adv_opt) :
        grid_(n + 2, std::vector<Cell>(n + 2)), dimension_{ n + 2 }, q_prob_{q_prob},
        beta_{ b }, gamma_{ y }, advanced_opt{ adv_opt }, name_{ c }{
        //Verifico la coerenza dei dati
        if (n < 100) {
            throw std::runtime_error{ "Error: the dimension is too tiny for see any simulation  effect."
                "The minim dimension is 100" };
        }
        assert(b > 0 && b < 1);
        assert(y > 0 && y < 1);
        assert(q_prob > 0 && q_prob < 1);
        assert(inf > 0);
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
    void Board::evolve_() {
        std::vector<std::vector<Sir>> end(dimension_ - 2, std::vector<Sir>(dimension_ - 2));
        int teoretical_ill = static_cast<int>(dimension_ * beta_);
        int malati_trovati = static_cast<int>(teoretical_ill * q_prob_);
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
                // deve introdurre un opzione avanzata il fatto che ci si possa quarantenare. Fusco deve agire qui
                else if (grid_[l][c].state == Sir::i) {
                    int n = (rand() + time(0)) % (teoretical_ill);
                    if (n < malati_trovati && day_>10 && ((int)advanced_opt == 3 || (int)advanced_opt == 5)) {
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
                    if (day_ < quaranten.last_day)
                        end[l - 1][c - 1] = Sir::q_edge;
                    else
                        end[l - 1][c - 1] ;
                }
                // quando un malatom viene trovato malato viene messo in quarantena. Ci sta per 40 giorni poi è libero. dve essere un opzione avanzata di fusco.
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
        counter_quarantene_infected();
        ++day_;
        copy_(end);
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
        if (day_ > quaranten.first_day && day_ < quaranten.last_day && (int)advanced_opt >= 4) {
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
