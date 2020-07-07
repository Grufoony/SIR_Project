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
    Quarantine_parameters quarantin;
    std::vector<Counter> graph_out_quarantine;
    std::vector<Counter> graph_in_quarantine;
public:
    Board(std::string c, int n, double b, double y,  int inf, Mode mode = Mode::Still, double q_prob = 0., Quarantine_parameters quarantene = {0, 0, 0});
    void copy_(std::vector<std::vector<Sir>>& end);
    void counter_quarantine_infected();
    void counter_infected();
    Sir& operator()(int riga, int colonna);
    void move_();
    void evolve_();
    void quarantine_();
    void airplane_();
    void draw(int& secondi);

};
#endif
