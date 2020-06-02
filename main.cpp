#include <stdexcept>
#include "board.hpp"
#include "disease.hpp"

int main() {
    
    try {
        int days = 1600;
        int dim = 500;
        double beta = 0.018; //0.18
        double gamma = 0.001; //0.01
        double q = 0.05;

        Board b("COVID-19", dim, beta, gamma, q, 3, true);
        b.draw();

        Disease* d = new Disease("COVID-19", dim*dim, beta, gamma);
        d->evolve(days);
        d->f_print();
        d->draw(1280, 720, 'A');
        delete d;

    } catch(std::runtime_error const& e) {
        std::cerr << e.what() << '\n';
    }
    




    
}