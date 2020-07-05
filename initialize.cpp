#include "board.hpp"
#include <random>
#include <ctime>

// q_prob_ � la probabilit� che lo stato trovi un malato e lo metta in quarantena 
    Sir_model::Board::Board(std::string c, int n, double b, double y,double q_prob, int inf, Quarantine_parameters quarantene, Mode adv_opt) : name_{c}, grid_(n + 2, std::vector<Cell>(n + 2)), advanced_opt{adv_opt}{
        //Verifico la coerenza dei dati
        if (n < 100) {
            throw std::runtime_error{ "Error: the dimension is too tiny for see any simulation  effect."
                "The minim dimension is 100" };
        }
        assert(b > 0 && b < 1);
        assert(y > 0 && y < 1);
        assert(q_prob > 0 && q_prob < 1);
        assert(inf > 0);

        q_prob_ = q_prob;
        beta_ = b;
        gamma_ = y;
        dimension_ = n + 2;
        day_ = 0;
        quaranten.len_line = dimension_ / 2;
        quaranten.first_day = quarantene.first_day;
        quaranten.last_day = quarantene.last_day;
        for(int i = 0; i < inf; ++i) {
            int ran1 = (std::rand() + time(0)) % dimension_;
            int ran2 = (std::rand() + time(0)) % dimension_;
            grid_[ran1][ran2].state = Sir::i;
        }
    };
    Sir& Sir_model::Board::operator()(int line, int column) {
        return (grid_[line + 1][column + 1].state);
    }
