#pragma once
#include<iostream>

enum class Sir :char {
    s, i, r, q, q_edge
};
enum class Mode : int {
    Still, Move, Move_Plus, Quarantine_1, Quarantine_2, Quarantine_1_and_2
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
    double inf_prob = 0; //Probabilità di infettarsi
    int clock;
    inline Cell() {
        state = Sir::s;
        inf_prob = 0;
        clock = 0;
    }
};
struct Counter {
    int num_s = 0;
    int num_i = 0;
    int num_r = 0;
};
inline Sir& operator++(Sir& hs) {
    switch (hs) {
    case Sir::s: hs = Sir::i; return hs; //++s = i
    case Sir::i: hs = Sir::r; return hs; //++i = r
    case Sir::q: hs = Sir::r; return hs; //++q = r
    default: return hs;
    }
};
