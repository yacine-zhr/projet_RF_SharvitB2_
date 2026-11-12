// =============================================================================
//  Programme de test du chargement (tache 1).
//  Usage :  ./load [racine_donnees] [nb_classes]
//  Exemple : ./load data/SharvitB2 6
// =============================================================================

#include "Dataset.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
    std::string racine = (argc > 1) ? argv[1] : "data/SharvitB2";
    int nb_classes     = (argc > 2) ? std::atoi(argv[2]) : 6;

    try {
        rf::Dataset ds;
        ds.load(racine, nb_classes);
        ds.print_summary();

        // Petit apercu : premiere forme de chaque methode.
        std::cout << "\nApercu (premiere forme, " << ds.method(rf::Method::ART).names[0]
                  << ") :\n";
        for (rf::Method m : rf::ALL_METHODS) {
            const auto& v = ds.method(m).X[0];
            std::cout << "  " << rf::method_name(m) << " [" << v.size() << "] : ";
            for (std::size_t i = 0; i < v.size() && i < 5; ++i) std::cout << v[i] << " ";
            std::cout << (v.size() > 5 ? "..." : "") << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "ERREUR : " << e.what() << "\n";
        return 1;
    }
    return 0;
}
