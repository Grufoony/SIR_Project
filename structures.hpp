#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include<iostream>
#include<cassert>

    enum class Sir :char {
        s, i, r, q, q_edge
    };
    enum class Mode : int {
        Still, Move, move_plus, Quarantene_1, Quarantene_2, Quarantene_1_and_2
    };
    // struttura che definisce i paramtri della quarantena
    struct Quarantene_parameters {
        int first_day = 0;
        int last_day = 0;
        int len_line = 0;
    };
    //struttura che definisce i parametri di ogni cella del vettore grid della classe board.
    struct Cell {
        Sir state;
        double inf_prob = 0; //Probabilità di infettarsi
        int clock;
        inline Cell() {
            state = Sir::s;
            inf_prob = 0;
            clock = 0;

        }
    };
    struct Counter {
        int num_s=0;
        int num_i=0;
        int num_r=0;
    };    
    inline Sir& operator++(Sir& hs) {
        switch (hs) {
        case Sir::s: hs = Sir::i; return hs; //++s = i
        case Sir::i: hs = Sir::r; return hs; //++i = r
        case Sir::q: hs = Sir::r; return hs; //++q = r
        default: return hs;
        }
    };

    //Per la classe disease
    struct State {
        double s;
        double i;
        double r;

        State(double sus, double inf, double rec) : s{ sus }, i{ inf }, r{ rec } {
            assert(s > 0 || s == 0);
            assert(i > 0 || i == 0);
            assert(r > 0 || r == 0);
        };
        State() {
            s = 0.;
            i = 0.;
            r = 0.;
        }
    };

#endif
