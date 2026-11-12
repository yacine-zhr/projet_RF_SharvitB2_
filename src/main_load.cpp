// main_load.cpp
// Petit programme pour vérifier que les données se chargent bien.
//
// Utilisation : ./load [dossier des données] [nombre de classes]
// Exemple     : ./load data/SharvitB2 6

#include "Dataset.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
    // Valeurs par défaut si on lance ./load sans argument.
    std::string racine = (argc > 1) ? argv[1] : "data/SharvitB2";
    int nb_classes = (argc > 2) ? std::atoi(argv[2]) : 6;

    try {
        rf::Dataset ds;
        ds.load(racine, nb_classes);
        ds.print_summary();

        // Pour voir à quoi ressemblent les données : les 5 premières valeurs
        // de la première forme, pour chaque méthode.
        std::cout << "\nPremière forme (" << ds.method(rf::Method::ART).names[0] << ") :\n";
        for (rf::Method m : rf::ALL_METHODS) {
            const auto& v = ds.method(m).X[0];
            std::cout << "  " << rf::method_name(m) << " :";
            for (std::size_t i = 0; i < v.size() && i < 5; ++i) std::cout << " " << v[i];
            std::cout << " ...\n";
        }
    } catch (const std::exception& e) {
        // Le chargement lève une exception au moindre problème : on affiche
        // le message et on quitte avec un code d'erreur.
        std::cerr << "Erreur : " << e.what() << "\n";
        return 1;
    }
    return 0;
}
