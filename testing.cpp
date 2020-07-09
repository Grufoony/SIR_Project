#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
// Utilizziamo doctest 2.3.8, ultima versione disponibile al 24/06
#include <cassert>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

struct State {
  double s;
  double i;
  double r;

  State(double sus, double inf, double rec) : s{sus}, i{inf}, r{rec} {
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

class Disease {
public:
  std::string name_;
  std::vector<State> state_;
  double tot_;
  double gamma_;
  double beta_;

  auto evolve_(State const &begin) {
    State end = State();

    end.s = begin.s - beta_ * begin.i * begin.s;
    if (end.s < 0) {
      end.s = 0;
    }

    end.r = begin.r + gamma_ * begin.i;
    if (end.r > tot_) {
      end.r = tot_;
    }

    end.i = begin.i + beta_ * begin.i * begin.s - gamma_ * begin.i;
    if (end.i > tot_) {
      end.i = tot_ - end.r - end.s;
    }
    assert(end.s > 0 || end.s == 0);
    assert(end.i > 0 || end.i == 0);
    assert(end.r > 0 || end.r == 0);

    return end;
  };

  Disease(std::string p, int n, double b, double y) : name_{p} {
    assert(b > 0 && b < 1);
    assert(y > 0 && y < 1);
    assert(n > 1);

    tot_ = n;
    beta_ = b / tot_;
    gamma_ = y;
    State state_0{(double)n - 1., 1., 0.};
    state_.push_back(state_0);
  }

  void setBeta(double b) { beta_ = b / tot_; }

  void setGamma(double g) { gamma_ = g; }

  void evolve(int n) {
    assert(n > 0);

    for (int i = 0; i < n; ++i) {
      state_.push_back(evolve_(state_[i]));
    }
  }
};

//++++++++++++++++++++++++++++++++++++++++++++++
//             DISEASE TEST                    +
//++++++++++++++++++++++++++++++++++++++++++++++

TEST_CASE("Testing State") {
  auto a = State(1e6, 0, 1);
  CHECK(a.s == 1e6);
  CHECK(a.i == 0);
  CHECK(a.r == 1);
  auto b = State();
  CHECK(b.s == 0);
  CHECK(b.i == 0);
  CHECK(b.r == 0);
}

TEST_CASE("Testing Disease") {
  auto d = Disease("nome", 500, 0.3, 0.2);
  CHECK(d.state_[0].s == 499);
  CHECK(d.state_[0].i == 1);
  CHECK(d.state_[0].r == 0);
  CHECK(d.beta_ == 6e-4);
  CHECK(d.gamma_ == 0.2);
  // Test delle funzioni setBeta e setGamma
  d.setBeta(0.26);
  d.setGamma(0.43);
  CHECK(
      abs(d.beta_ - 5.2e-4) <
      std::numeric_limits<double>::epsilon()); // Per comparare due double uso
                                               // la definizione standard di
                                               // epsilon (numero che tende a 0)
  CHECK(d.gamma_ == 0.43);
  // Test della funzione evolve
  d.evolve(1);
  CHECK((int)d.state_[1].s == 498);
  CHECK((int)d.state_[1].i == 0);
  CHECK((int)d.state_[1].r == 0);
  d.evolve(2);
  CHECK((int)d.state_[1].s == 498);
  CHECK((int)d.state_[1].i == 0);
  CHECK((int)d.state_[1].r == 0);
}