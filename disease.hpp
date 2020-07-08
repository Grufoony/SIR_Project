
#ifndef DiSEASE_HPP
#define DISEASE_HPP

#include <vector>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <limits>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "structures.hpp"

namespace disease {
    int constexpr EDGE = 20;
    class Disease {
    private:
        std::string name_;
        std::vector<State> state_;
        double tot_;
        double gamma_;
        double beta_;
    public:
        auto evolve_(State const& begin);
        Disease(std::string p, int n, double  b, double y);
        void setBeta(double b);
        void setGamma(double g);
        void evolve(int n);
        void print();
        void f_print();
        double get_state_s(int i);
        double get_state_i(int i);
        double get_state_r(int i);
        double get_beta();
        double get_gamma();
        void draw(int lenght, int height, const char& c);
    };
}

#endif
