#ifndef BOARD_HPP
#define BOARD_HPP
#include "structures.hpp"
#include <iostream>
#include <vector>
namespace sir {
class Board {
private:
  std::string name_;
  std::vector<std::vector<Cell>> grid_;
  int dimension_;
  double beta_;
  double gamma_;
  int day_ = 0;
  Counter counter_;
  Counter q_counter_;
  double q_prob_;
  Mode advanced_opt_;
  Quarantine_parameters quarantin_;
  std::vector<Counter> graph_out_quarantine_;
  std::vector<Counter> graph_in_quarantine_;

public:
  Board(std::string c, int n, double b, double y, int inf, Mode adv_opt,
        double q_prob, Quarantine_parameters quarantine);
  void copy_(std::vector<std::vector<Sir>> &end);
  void counter_quarantine();
  Sir &operator()(int riga, int colonna);
  void move_();
  void evolve_();
  void quarantine_();
  void airplane_();
  void draw(int &secondi);
  int gen_unif_rand_number(int) const;
};
} // namespace sir
#endif
