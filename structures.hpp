#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include<iostream>
// struttura che definisce i vari stati possibili di una celletta.
    enum class Sir :char {
        s, i, r, q, q_edge
    };
    inline Sir& operator++(Sir& hs) {
        switch (hs) {
        case Sir::s: hs = Sir::i; return hs; //++s = i
        case Sir::i: hs = Sir::r; return hs; //++i = r
        case Sir::q: hs = Sir::r; return hs; //++q = r
        default: return hs;
        }
    };
    // struttura che definisce i paramtri della quarantena
    struct Quarantine_parameters {
        int first_day = 0;
        int last_day = 0;
        int len_line = 0;
    };
    //struttura che definisce i parametri di ogni cella del vettore grid della classe board.
    struct Cell {
        Sir state;
        double inf_prob = 0; //Probabilit� di infettarsi
        double clock;
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
    enum class Mode : int {
        Still,Move,move_plus,Quarantene_1,Quarantene_2,Quarantene_1_and_2
    };

#endif