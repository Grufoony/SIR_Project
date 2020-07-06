#ifndef BOARD_HPP
#define Board_HPP
#include"structures.hpp"
#include<vector>
#include<iostream>

class Board {
private:
    std::string name_;
    std::vector<std::vector<Cell>> grid_;
    int dimension_;
    double beta_;
    double gamma_;
    int day_;
    Counter counter;
    Counter q_counter;
    double q_prob_;
    Mode advanced_opt;
    Quarantene_parameters quaranten;
    std::vector<Counter> grafico_out_quarantene;
    std::vector<Counter> grafico_in_quarantene;
public:
    Board(std::string c,int n, double b, double y, double q_prob,int inf, Quarantene_parameters quarantene,Mode mode=Mode::Still);
    void copy_(std::vector<std::vector<Sir>>& end);
    void counter_quarantene_infected();
    void counter_infected();
    Sir& operator()(int riga, int colonna);
    void move_();
    void evolve_();
    void quarantene_();
    void airplane_();
    void draw(int& secondi);
   
};
#endif
