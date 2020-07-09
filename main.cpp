//*******************************************************************************************
// Autori : Francesco Pivi , Gregorio Berselli, Matteo Fusconi.
//
//
// Questo programma illustra l'evoluzione di una ipotetica pandemia secondo
// il modello Sir. Tale modello è stato poi implementato con quarantenati ed
// altre modalità descritte nella relazione allegata.
//*******************************************************************************************

#include "board.hpp"
#include "disease.hpp"
constexpr int NUM_INITIAL_INFECTED = 5;
constexpr int DIM = 400;
constexpr double BETA = 0.4;
constexpr double GAMMA = 0.15;
int main() {
  try {
    std::string name;
    int option;
    double q_prob = 0;
    Mode adv_opt;
    Quarantine_parameters Quarantine;
    Quarantine.first_day = 0;
    Quarantine.last_day = 0;

    std::cout << "Welcome to the simulation of an epidemic disease!\n";
    std::cout << "The name of the disease is:";
    std::cin >> name;
    std::cout << "Choose the mode:\n1) Still \n2) Move\n3) Move Plus\n4) "
                 "Quarantine 1\n5) Quarantine 2\n"
                 "6) Quarantine 1 and 2\nYour choice: ";
    std::cin >> option;

    switch (option) {
    case 1:
      adv_opt = Mode::Still;
      break;
    case 2:
      adv_opt = Mode::Move;
      break;
      ;
    case 3:
      adv_opt = Mode::Move_Plus;
      break;
    case 4:
      adv_opt = Mode::Quarantine_1;
      q_prob = 0.09;
      break;
    case 5:
      adv_opt = Mode::Quarantine_2;
      Quarantine.first_day = 10;
      Quarantine.last_day = 150;
      break;
    case 6:
      adv_opt = Mode::Quarantine_1_and_2;
      q_prob = 0.5;
      Quarantine.first_day = 10;
      Quarantine.last_day = 150;
      break;
    default:
      throw std::runtime_error{"Invalid selected mode"};
    }

    sir::Board b(name, DIM, BETA, GAMMA, NUM_INITIAL_INFECTED, adv_opt, q_prob,
                 Quarantine);
    int refresh_rate = 10; // ms
    b.draw(refresh_rate);

    /***********************************************************
    Di seguito è riportata l'opzione 1 richiesta per l'esame.
    Si è deciso di utilizzarla per portare un confronto teorico.
    ***********************************************************/

    int days = 1600;
    disease::Disease d(name, DIM * DIM, BETA, GAMMA);
    d.evolve(days);
    d.f_print();
    d.draw(700, 500, 'A');
  } catch (std::runtime_error &e) {
    std::cerr << e.what();
  }
}