// =============================================================================
//  Verification complete du format sur TOUS les fichiers (tache 2 / point 2).
//  Charge les 18 classes et controle :
//    - 5 descripteurs par forme (verifie par Dataset::load) ;
//    - dimension constante par methode ;
//    - 12 echantillons par classe ;
//    - composantes constantes (ecart-type nul) : utile pour la normalisation ;
//    - plage min/max des valeurs (met en evidence les ecarts d'echelle).
//  Usage : ./check [racine_donnees]
// =============================================================================

#include "Dataset.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

int main(int argc, char** argv) {
    std::string racine = (argc > 1) ? argv[1] : "data/SharvitB2";

    try {
        rf::Dataset ds;
        ds.load(racine, 18);  // controle sur l'integralite de la base
        std::cout << "[OK] 216 formes attendues, " << ds.nb_formes()
                  << " chargees. Chaque forme possede ses 5 descripteurs.\n\n";
        ds.print_summary();
        std::cout << "\n--- Analyse par methode (sur 216 formes) ---\n";

        for (rf::Method m : rf::ALL_METHODS) {
            const auto& md = ds.method(m);
            const int dim = md.dim;
            const int n   = static_cast<int>(md.size());

            double vmin =  std::numeric_limits<double>::infinity();
            double vmax = -std::numeric_limits<double>::infinity();

            // Moyenne et ecart-type par composante -> detection des constantes.
            std::vector<double> moy(dim, 0.0), var(dim, 0.0);
            for (const auto& x : md.X)
                for (int j = 0; j < dim; ++j) moy[j] += x[j];
            for (int j = 0; j < dim; ++j) moy[j] /= n;
            for (const auto& x : md.X)
                for (int j = 0; j < dim; ++j) {
                    var[j] += (x[j] - moy[j]) * (x[j] - moy[j]);
                    vmin = std::min(vmin, x[j]);
                    vmax = std::max(vmax, x[j]);
                }
            int constantes = 0;
            for (int j = 0; j < dim; ++j) {
                var[j] /= n;
                if (std::sqrt(var[j]) < 1e-9) ++constantes;
            }

            std::cout << "  " << rf::method_name(m)
                      << " : dim=" << dim
                      << ", min=" << vmin << ", max=" << vmax
                      << ", composantes constantes (ecart-type nul) = "
                      << constantes << "\n";
        }

        std::cout << "\nRemarques :\n"
                     "  - Les ecarts d'echelle (ZRK >> GFD) justifient la "
                     "normalisation z-score (tache 2).\n"
                     "  - Les composantes constantes doivent etre ignorees ou "
                     "regularisees (epsilon) lors du z-score.\n";
    } catch (const std::exception& e) {
        std::cerr << "ERREUR : " << e.what() << "\n";
        return 1;
    }
    return 0;
}
