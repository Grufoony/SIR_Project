//*******************************************************************************************
// Autori : Francesco Pivi , Gregorio Bertiselli, Matteo Fusconi.   
//
//
// Questo programma illustra l'evoluzione di una ipotetica pandemia secondo
// il modello Sir. Tale modello è stato poi implementato con quarantenati ed altre modalità                                                                        
// descritte nella relazione allegata.                                                                                                                                                                                                             
//*******************************************************************************************
#include "board.h"
#include"desease.hpp"
#include<iostream>
#include<stdexcept>
int main(int argc, char* argv[]) {
    try {
        /***********************************************************
        Ultima opzione del progetto d'esame. varie opzioni possibili.
        Possibilità di attivare varie opzioni avanzate.
        ***********************************************************/
        int dim = 400;
        double beta_model = 0.2;
        double gamma_model = 0.5;
        Quarantene_parameters Quarantena;
        Quarantena.first_day = 20;
        Quarantena.last_day = 120;
        Sir_model::Board b(dim,beta_model, gamma_model, 0.09, Quarantena,Mode::Quarantene_1_and_2);
        b(1, 1) = Sir::i;
        int millisecondi = 1000;
        //b.draw(millisecondi);
     

        /***********************************************************
        qui di seguito è riportata l'opzione 1 richiesta per l'esame.
        Si è deciso di utilizzarla per portare un confronto teorico.
        ***********************************************************/
        double gamma_sir = 0.001;
        double beta_sir = 0.018;
        int days = 600;
        desease::Disease d("COVID-19", dim * dim,beta_sir , gamma_sir);
        d.evolve(days);
        d.f_print();
        d.draw(1280, 720, 'A');


    }
    catch (std::runtime_error & e) {
        std::cerr << e.what();
    }

    
}

