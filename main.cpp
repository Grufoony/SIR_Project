//*******************************************************************************************
// Autori : Francesco Pivi , Gregorio Berselli, Matteo Fusconi.   
//
//
// Questo programma illustra l'evoluzione di una ipotetica pandemia secondo
// il modello Sir. Tale modello è stato poi implementato con quarantenati ed altre modalità                                                                        
// descritte nella relazione allegata.                                                                                                                                                                                                             
//*******************************************************************************************
#include "board.hpp"
#include "disease.hpp"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        /***********************************************************
        Ultima opzione del progetto d'esame. varie opzioni possibili.
        Possibilità di attivare varie opzioni avanzate.
        ***********************************************************/
        std::string name{"Covid-19"};
        int dim = 400;
        double beta_model = 0.2;
        double gamma_model = 0.5;
        Quarantine_parameters Quarantine;
        Quarantine.first_day = 20;
        Quarantine.last_day = 120;
        Sir_model::Board b(name, dim,beta_model, gamma_model, 0.09, 3, Quarantine, Mode::Quarantene_1_and_2);
        int refresh_rate = 10; //ms
        b.draw(refresh_rate);
     

        /***********************************************************
        Di seguito è riportata l'opzione 1 richiesta per l'esame.
        Si è deciso di utilizzarla per portare un confronto teorico.
        ***********************************************************/
        double gamma_sir = 0.01;
        double beta_sir = 0.18;
        int days = 1600;
        disease::Disease d("COVID-19", dim * dim,beta_sir , gamma_sir);
        d.evolve(days);
        d.f_print();
        d.draw(1280, 720, 'A');

    }
    catch (std::runtime_error & e) {
        std::cerr << e.what();
    }    
}