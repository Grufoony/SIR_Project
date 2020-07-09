#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include"disease.hpp"
#include"doctest.h"
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
    auto d = disease::Disease("nome", 500, 0.3, 0.2);
    CHECK(d.get_state_s(0) == 499);
    CHECK(d.get_state_i(0) == 1);
    CHECK(d.get_state_r(0) == 0);

   CHECK(d.get_beta() == 6e-4);
    CHECK(d.get_gamma() == 0.2);
    //Test delle funzioni setBeta e setGamma
    d.setBeta(0.26);
    d.setGamma(0.43);
    //Per comparare due double uso la definizione standard di epsilon (numero che tende a 0)
    CHECK(abs(d.get_beta() - 5.2e-4) < std::numeric_limits<double>::epsilon());
    CHECK(d.get_gamma() == 0.43);
    //Test della funzione evolve
    d.evolve(1);
    CHECK((int)d.get_state_s(1) == 498);
    CHECK((int)d.get_state_i(1) == 0);
    CHECK((int)d.get_state_r(1) == 0);
    d.evolve(2);
    CHECK((int)d.get_state_s(1) == 498);
    CHECK((int)d.get_state_i(1) == 0);
    CHECK((int)d.get_state_r(1) == 0);
}
