#include "board.hpp"
#include <ctime>
#include <memory>

void Sir_model::Board::airplane_() {
    //creo una funzione lambda che mi creer� una cella sullo hype per permettermi di muoverle.
    auto create_hype_cell = [&](int c, int l, int column, int line){
        std::unique_ptr<Cell> move = std::make_unique<Cell>();
        *move = grid_[c][l];
        move->state = grid_[column][line].state;
        move->inf_prob = grid_[column][line].inf_prob;
        grid_[column][line].state = grid_[c][l].state;
        grid_[column][line].inf_prob = grid_[c][l].inf_prob;
        grid_[c][l] = *move;
    };
    //se ho l'opzione avanzata della quarantena di Pivi allora ho opzioni avanzate.Fusco da implementare l'enum deve aggiungere qui l opzione 5.
    if (day_ > quaranten.first_day && day_ < quaranten.last_day && (int)advanced_opt>=4) {
        for (int c = 1; c < dimension_; ++c) {
            for (int l = 1; l < dimension_; ++l) {
                if (((c < quaranten.len_line || c > quaranten.len_line * 2) || (l < quaranten.len_line || l > quaranten.len_line * 2))) {
                    int column = (rand() + time(0)) % dimension_;
                    int line = (rand() + time(0)) % dimension_;
                    // ci ho messo 3 ore per fare questo if. Non toccatelo please ve lo spiego in chiamata.
                    if (((grid_[c][l].state !=Sir::q_edge && grid_[column][line].state != Sir::q_edge)||
                        (grid_[c][l].state!=Sir::q && grid_[column][line].state != Sir::q))
                        && ((line <  quaranten.len_line || line > quaranten.len_line * 2) ||
                            (column < quaranten.len_line || column > quaranten.len_line * 2))) {
                        if (line == 1) {
                            create_hype_cell(c, l, column, line);
                        }
                    }
                }
            }
        }
    }
    // se non ci sono le opzioni avanzate di PIvi.
    else {
        for (int c = 1; c < dimension_; ++c) {
            for (int l = 1; l < dimension_; ++l) {
                int column = (rand() + time(0)) % dimension_;
                int line = (rand() + time(0)) % dimension_;
                if (grid_[c][l].state != Sir::q_edge && grid_[column][line].state != Sir::q_edge) {
                    if (line == 1) {
                        create_hype_cell(c, l, column, line);
                    }
                }
            }
        }
    }
}
//permette alle celle di muoversi una adiacente all'altra.Simula gli spostamenti
//giornalieri di prima necessit� di ognuno di noi
void Sir_model::Board::move_() {
    for (int l = 1; l < dimension_ - 2; ++l) {
        for (int c = 1; c < dimension_ - 2; ++c) {
            int change_line = (rand() + time(0)) % 3;
            int change_column = (rand() + time(0)) % 3;//ricorda il meno due;
            if (((rand() + time(0)) % 9) == 3 && ((grid_[l][c].state != (Sir::q_edge) && grid_[l + change_line - 1][c + change_column - 1].state != (Sir::q_edge)) && grid_[l][c].state != Sir::q && grid_[l + change_line - 1][c + change_column - 1].state != Sir::q)) {
                std::unique_ptr<Cell> old = std::make_unique<Cell>();
                *old = grid_[l][c];
                grid_[l][c] = grid_[l + change_line - 1][c + change_column - 1];
                grid_[l + change_line - 1][c + change_column - 1] = *old;
            }
        }
    }
}
