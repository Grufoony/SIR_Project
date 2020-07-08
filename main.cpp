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

int main() {
    try {      
        std::cout << "Inserire il nome della pandemia che si ha intenzione di trattare.\n";
        std::string name;
        std::cin >> name;
        std::cout << "Inserire la dimensione della griglia.\n";
        std::cout << "Tale dimensione deve essere compresa fra 150 e 400.\nLa dimensione che lei desidera e':";
        int dim;
        std::cin>>dim;   
        double beta_model;
        double gamma_model;
        double q_prob=0;
        std::cout << "Inserire il parametro BETA del modello Sir(deve essere > di 0 e < di 1).\nBeta:";
        std::cin >> beta_model;
        std::cout << "Inserire il parametro GAMMA del modelllo Sir(deve essere > di 0 e < di 1).\nGamma:";
        std::cin >>gamma_model;
        std::cout << "Inserire il numero di quale opzione avanzata si vuole osservare il risulatoto:\n";
        std::cout << "1)Celle_ferme \n2)Celle con movimento adiacente.\n3)Celle con movimento a lungo raggio.\n"
            "4)Possibilita' di trovare alcuni malati e quarantenarli.\n5)Dopo un certo numero di giorni un quarto della griglia si quarantena.\n"
            "6)Vengono incluse tutte le opzioni precedenti\nL'opzione da lei desiderata e': ";   
        int option;
        Mode adv_opt;
        std::cin >> option;
        Quarantine_parameters Quarantine;
        Quarantine.first_day = 0;
        Quarantine.last_day =0;
        int num_initial_infected = 1;
        std::cout << "Inserire il numero di infetti che si trovera' inizialmente sulla griglia:";
            std::cin >> num_initial_infected;  
        switch(option)
        {
        case 1:adv_opt = Mode::Still; break;
        case 2:adv_opt = Mode::Move; break;;
        case 3:adv_opt = Mode::Move_Plus; break;;
        case 4:adv_opt = Mode::Quarantine_1;
            std::cout<<"Dichiara la probabilita' che una persona si trovi una persona malata\ne la si ponga in quarantena(compresa fra 0 e 0.5): ";
            std::cin >> q_prob; break;
        case 5:adv_opt = Mode::Quarantine_2;
            std::cout << "Inserire da che  giorno un quarto della griglia decidera' di quarantenarsi: ";
            std::cin >> Quarantine.first_day;
            std::cout << "\n";
            std::cout << "Inserire l'ultimo giorno della quarantena, non potendo questa durare in eterno: ";
            std::cin >> Quarantine.last_day;
            break;
        case 6:adv_opt = Mode::Quarantine_1_and_2;
            std::cout << "Dichiara la probabilita' che una persona si trovi una persona malata\n e la si ponga in quarantena(deve essere compresa fra 0 e 0.5): ";
            std::cin >> q_prob;
            std::cout << "Inserire da che  giorno un quarto della griglia decidera' di quarantenarsi: ";
            std::cin >> Quarantine.first_day;
            std::cout << "\n";
            std::cout << "Inserire l'ultimo giorno della quarantena, non potendo questa durare in eterno: ";
            std::cin >> Quarantine.last_day;
            break;
        default:
            adv_opt = Mode::Quarantine_1_and_2;
        }
        /***********************************************************
       Ultima opzione del progetto d'esame. varie opzioni possibili.
       Possibilità di attivare varie opzioni avanzate.
       ***********************************************************/

        sir::Board b(name, dim, beta_model, gamma_model,num_initial_infected,adv_opt,q_prob,Quarantine);
        int refresh_rate = 200; //ms
        b.draw(refresh_rate);


        /***********************************************************
        Di seguito è riportata l'opzione 1 richiesta per l'esame.
        Si è deciso di utilizzarla per portare un confronto teorico.
        ***********************************************************/

       double gamma_sir = 0.01;
        double beta_sir = 0.18;
        int days = 600;
        disease::Disease d(name, dim * dim, beta_sir, gamma_sir);
        d.evolve(days);
        d.f_print();
        d.draw(700, 500, 'A');
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what();
    }
}
