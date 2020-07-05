#include "board.hpp"
// le devo ancora implementare per bene per i grafici.lo faccio oggi
void Sir_model::Board::counter_quarantene_infected() {
    q_counter.num_s = 0;
    q_counter.num_i = 0;
    q_counter.num_r = 0;
    for (int line = quaranten.len_line; line < quaranten.len_line * 2; ++line) {
        for (int column = quaranten.len_line; column < quaranten.len_line * 2; ++column) {
            if (grid_[line][column].state == Sir::s) {
                ++q_counter.num_s;
            }
            else if (grid_[line][column].state == Sir::r) {
                ++q_counter.num_r;
            }
            else {
                ++q_counter.num_i;
            }
        }
    }
}
//da implementare meglio.possono essere unite ad evolve.Oggi lo faccio.
void Sir_model::Board::counter_infected() {
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
void Sir_model::Board::copy_(std::vector<std::vector<Sir>>& end) {
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