#include "board.hpp"
#include <ctime>
void Sir_model::Board::evolve_() {
    std::vector<std::vector<Sir>> end(dimension_ - 2, std::vector<Sir>(dimension_ - 2));
    int theoretical_ill = static_cast<int>(dimension_* beta_);
    int found_ill = static_cast<int>(theoretical_ill * q_prob_);
    for (int c = 1; c < dimension_ - 1; ++c) {
        for (int l = 1; l < dimension_ - 1; ++l) {
            if (grid_[c][l].state == Sir::s) {
                for (int j = -1; j <= 1; ++j) {
                    for (int i = -1; i <= 1; ++i) {
                        if (grid_[c + j][l + i].state == Sir::i && grid_[c + j][l + i].state != Sir::s) {
                            grid_[c][l].inf_prob += beta_; //somma delle probabilita di infettarsi attorno alla cella.
                            //quando tale probabilit� diventa uno la cella si ammala
                        }
                        else {
                            grid_[c][l].inf_prob += 0;
                        }
                    }
                }
                if (grid_[c][l].inf_prob >= 1) {
                    ++end[c - 1][l - 1];
                    grid_[c][l].inf_prob = 0;
                }
            }
            //metodo di greg corretto: quando una persona � malata o guarisce per� dopo 14 giorni ,oppure viene messa in quarantena prima di guarire.
            //definisco un intervallo( theoretical ill), genero un numero fra 0 e il  sup di tale intervallo.
            //matati trovati � un numero dipendete da q_prob_ compreso fra zero ed il precedente sup. da qui si comprende l if.
            // deve introdurre un opzione avanzata il fatto che ci si possa quarantenare. Fusco deve agire qui
            else if (grid_[c][l].state == Sir::i) {
                int n =(rand()+ time(0))%(theoretical_ill);             
                if (n < found_ill && day_ > 10 && ((int)advanced_opt==3||(int)advanced_opt==5)) {
                    end[c - 1][l - 1] = Sir::q;  
                }
                else {
                    grid_[c][l].clock += 1*gamma_;
                    if (grid_[c][l].clock > 14) {
                        ++end[c - 1][l - 1];
                        grid_[c][l].clock = 0;
                    }
                    else {
                        ++end[l - 1][c - 1];
                    }
                }
            }
            else if (grid_[c][l].state == Sir::r) {
                ++(++end[c - 1][l - 1]);
            }
            //questo if � per l ultima opzione avanzata,dove ho il borod della quarantena.
            else if (grid_[c][l].state == Sir::q_edge) {
                if (day_ < quaranten.last_day)
                    end[c - 1][l - 1] = Sir::q_edge;
            }
            // quando un malatom viene trovato malato viene messo in quarantena. Ci sta per 40 giorni poi � libero. dve essere un opzione avanzata di fusco.
            else if (grid_[c][l].state == Sir::q) {
                if (grid_[c][l].clock == 40) {
                    ++(++end[c - 1][l - 1]);
                    grid_[c][l].clock = 0;
                }
                else {
                    ++grid_[c][l].clock;
                    end[c - 1][l - 1] = Sir::q;
                }
            }

        }
    }
    counter_infected();
    counter_quarantene_infected();
    ++day_;
    copy_(end);
}
void Sir_model::Board::quarantene_() {
    for (int c = 1; c < dimension_; ++c) {
        for (int l = 1; l < dimension_; ++l) {
            if ((((c == quaranten.len_line || c == quaranten.len_line * 2) || (l == quaranten.len_line * 2 || l == quaranten.len_line)) && (c >= quaranten.len_line && c <= quaranten.len_line * 2) && (l >= quaranten.len_line && l <= quaranten.len_line * 2)))
                grid_[c][l].state = Sir::q_edge;
        }

    }
}