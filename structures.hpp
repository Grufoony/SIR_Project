
#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP
#include<iostream>
#include<cassert>

enum class Sir :char {
    s, i, r, q, q_edge
};

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

enum class Mode : int {
    Still, Move, Move_Plus, Quarantine_1, Quarantine_2, Quarantine_1_and_2
};

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

struct Cell {
    Sir state;
    double inf_prob = 0; 
    int clock;
    inline Cell() {
        state = Sir::s;
        inf_prob = 0;
        clock = 0;
    }
};

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

struct Quarantine_parameters {
    int first_day = 0;
    int last_day = 0;
    int len_line = 0;
};

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
struct Counter {
    int num_s = 0;
    int num_i = 0;
    int num_r = 0;
    int num_q = 0;
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

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

///////////////////////////////////////////////////
///////////////////////////////////////////////////

inline Sir& operator++(Sir& hs) {
    switch (hs) {
    case Sir::s: hs = Sir::i; return hs; //++s = i
    case Sir::i: hs = Sir::r; return hs; //++i = r
    case Sir::q: hs = Sir::r; return hs; //++q = r
    default: return hs;
    }
};
