#include "disease.hpp"
#include <SFML/Graphics.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// evolution of the disease
////////////////////////////////////////////////////////////////////////////////////////////////////////////
auto disease::Disease::evolve_(State const &begin) {
  auto end = State();
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
  // I make sure the data is correct
  assert(end.s > 0 || end.s == 0);
  assert(end.i > 0 || end.i == 0);
  assert(end.r > 0 || end.r == 0);
  return end;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

disease::Disease::Disease(std::string p, int n, double b, double y) : name_{p} {
  assert(b > 0 && b < 1);
  assert(y > 0 && y < 1);
  assert(n > 1);

  tot_ = n;
  beta_ = b / tot_;
  gamma_ = y * 0.1;
  State state_0{(double)n - 1., 1., 0.};
  state_.push_back(state_0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void disease::Disease::setBeta(double b) { beta_ = b / tot_; }
void disease::Disease::setGamma(double g) { gamma_ = g; }
void disease::Disease::evolve(int n) {
  for (int i = 0; i < n; ++i) {
    state_.push_back(evolve_(state_[i]));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void disease::Disease::print() {
  int i = 0;
  auto tab = std::setw(20);
  std::cout << name_ << '\n';
  std::cout << "Current value of R0: " << (tot_ * beta_) / gamma_ << '\n';
  std::cout << tab << "Day" << tab << "Susceptible" << tab << "Infectuos" << tab
            << "Recovered" << '\n';
  for (auto const it : state_) {
    std::cout << std::setprecision(10) << tab << i << tab << (int)it.s << tab
              << (int)it.i << tab << (int)it.r << '\n';
    ++i;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void disease::Disease::f_print() {
  // Sposto l'output su file
  std::ofstream fp;
  fp.open("report.txt");
  std::cout.rdbuf(fp.rdbuf());

  print();
  fp.close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

double disease::Disease::get_state_s(int i) { return (state_[i].s); }
double disease::Disease::get_state_i(int i) { return (state_[i].i); }
double disease::Disease::get_state_r(int i) { return (state_[i].r); }
double disease::Disease::get_beta() { return beta_; }
double disease::Disease::get_gamma() { return gamma_; }
//////////////////////////////////////////////////////////////////////////////////////////////
// DRAW THE GRAPH USING SFML
//////////////////////////////////////////////////////////////////////////////////////////////

void disease::Disease::draw(int lenght, int height, const char &c) {
  sf::RenderWindow window(sf::VideoMode(lenght, height), name_,
                          sf::Style::Close | sf::Style::Resize);
  window.setVerticalSyncEnabled(true);
  sf::RectangleShape x_axis(sf::Vector2f(static_cast<float>(lenght), 1.f));
  sf::RectangleShape y_axis(sf::Vector2f(1.f, static_cast<float>(height)));
  x_axis.setPosition(0.f, static_cast<float>(height - EDGE));
  y_axis.setPosition(EDGE, 0);
  x_axis.setFillColor(sf::Color::White);
  y_axis.setFillColor(sf::Color::White);
  sf::RectangleShape bit(sf::Vector2f(2., 2.));
  auto x_up = (lenght - EDGE) / state_.size();
  auto y_scale = (height - (2 * EDGE)) / tot_;
  if (x_up < 1) {
    x_up = 1;
  }
  while (window.isOpen()) {
    window.clear(sf::Color::Black);
    window.draw(x_axis);
    window.draw(y_axis);

    sf::Event evnt;
    while (window.pollEvent(evnt)) {
      if (evnt.type == sf::Event::Closed) {
        window.close();
      }
    }
    double j;
    switch (c) {
    case 's':
    case 'S':
      j = EDGE;
      bit.setFillColor(sf::Color::Green);
      for (auto const it : state_) {
        bit.setPosition(static_cast<float>(j),
                        static_cast<float>(height - EDGE - it.s * y_scale));
        window.draw(bit);
        j += x_up;
      }
      break;
    case 'i':
    case 'I':
      j = EDGE;
      bit.setFillColor(sf::Color::Red);
      for (auto const it : state_) {
        bit.setPosition(static_cast<float>(j),
                        static_cast<float>(height - EDGE - it.i * y_scale));
        window.draw(bit);
        j += x_up;
      }
      break;
    case 'r':
    case 'R':
      j = EDGE;
      bit.setFillColor(sf::Color::Blue);
      for (auto const it : state_) {
        bit.setPosition(static_cast<float>(j),
                        static_cast<float>(height - EDGE - it.r * y_scale));
        window.draw(bit);
        j += x_up;
      }
      break;
    case 'a':
    case 'A':
      j = EDGE;
      bit.setFillColor(sf::Color::Green);
      for (auto const it : state_) {
        bit.setPosition(static_cast<float>(j),
                        static_cast<float>(height - EDGE - it.s * y_scale));
        window.draw(bit);
        j += x_up;
      }
      j = EDGE;
      bit.setFillColor(sf::Color::Red);
      for (auto const it : state_) {
        bit.setPosition(static_cast<float>(j),
                        static_cast<float>(height - EDGE - it.i * y_scale));
        window.draw(bit);
        j += x_up;
      }
      j = EDGE;
      bit.setFillColor(sf::Color::Blue);
      for (auto const it : state_) {
        bit.setPosition(static_cast<float>(j),
                        static_cast<float>(height - EDGE - it.r * y_scale));
        window.draw(bit);
        j += x_up;
      }
      break;
    default:
      throw std::runtime_error(
          "Warning: invalid character passed at the draw function\n");
    }
    window.display();
  }
}