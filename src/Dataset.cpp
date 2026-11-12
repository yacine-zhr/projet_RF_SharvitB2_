#include <cstdio>
// =============================================================================
//  Projet Reconnaissance des Formes - Base SharvitB2
//  Implementation du parser et de la classe Dataset (taches 1 et 2).
// =============================================================================

#include "Dataset.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

namespace rf {

// -----------------------------------------------------------------------------
//  Tables de correspondance methode -> nom / dossier / extension / dimension.
// -----------------------------------------------------------------------------
const char* method_name(Method m) {
    switch (m) {
        case Method::ART: return "ART";
        case Method::GFD: return "GFD";
        case Method::SA:  return "SA";
        case Method::YNG: return "YNG";
        case Method::ZRK: return "ZRK";
    }
    return "?";
}

const char* method_folder(Method m) {
    switch (m) {
        case Method::ART: return "ART";
        case Method::GFD: return "GFD";
        case Method::SA:  return "SA";
        case Method::YNG: return "Yang";       // le dossier s'appelle "Yang"
        case Method::ZRK: return "Zernike7";   // le dossier s'appelle "Zernike7"
    }
    return "?";
}

const char* method_ext(Method m) {
    switch (m) {
        case Method::ART: return ".art";
        case Method::GFD: return ".gfd";
        case Method::SA:  return ".sa";
        case Method::YNG: return ".yng";
        case Method::ZRK: return ".zrk";
    }
    return "";
}

int method_expected_dim(Method m) {
    switch (m) {
        case Method::ART: return 36;
        case Method::GFD: return 100;
        case Method::SA:  return 20;
        case Method::YNG: return 29;
        case Method::ZRK: return 18;
    }
    return 0;
}

// -----------------------------------------------------------------------------
//  Lecture d'un fichier descripteur : une valeur par ligne.
// -----------------------------------------------------------------------------
std::vector<double> read_descriptor_file(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Fichier introuvable : " + path);
    }
    std::vector<double> v;
    std::string ligne;
    int numero = 0;
    while (std::getline(in, ligne)) {
        ++numero;
        // On retire les espaces en debut/fin pour tolerer " 0.12 " ou "\r".
        auto debut = ligne.find_first_not_of(" \t\r\n");
        if (debut == std::string::npos) continue;          // ligne vide : ignoree
        auto fin = ligne.find_last_not_of(" \t\r\n");
        std::string token = ligne.substr(debut, fin - debut + 1);

        std::istringstream iss(token);
        double x;
        if (!(iss >> x)) {
            throw std::runtime_error("Valeur non numerique dans " + path +
                                     " (ligne " + std::to_string(numero) + ") : '" +
                                     token + "'");
        }
        v.push_back(x);
    }
    if (v.empty()) {
        throw std::runtime_error("Fichier vide : " + path);
    }
    return v;
}

// -----------------------------------------------------------------------------
//  Parsing du nom de fichier sCCnNNN.ext
// -----------------------------------------------------------------------------
bool parse_filename(const std::string& filename, int& classe, int& echantillon) {
    // On ne garde que le nom de base (sans chemin ni extension).
    std::string base = fs::path(filename).stem().string();
    // Attention : pour "s05n011.zrk.txt", stem() donne "s05n011.zrk".
    // On coupe donc au premier point eventuel restant.
    auto point = base.find('.');
    if (point != std::string::npos) base = base.substr(0, point);

    // Format attendu : s + 2 chiffres + n + 3 chiffres (insensible a la casse).
    if (base.size() != 7) return false;
    std::string low = base;
    std::transform(low.begin(), low.end(), low.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (low[0] != 's' || low[3] != 'n') return false;
    for (int i : {1, 2, 4, 5, 6}) {
        if (!std::isdigit(static_cast<unsigned char>(low[i]))) return false;
    }
    classe      = std::stoi(low.substr(1, 2));
    echantillon = std::stoi(low.substr(4, 3));
    return true;
}

// -----------------------------------------------------------------------------
//  Chargement d'une methode.
// -----------------------------------------------------------------------------
static MethodData load_method(const std::string& racine, Method m, int nb_classes) {
    fs::path dossier = fs::path(racine) / method_folder(m);
    if (!fs::exists(dossier)) {
        throw std::runtime_error("Dossier manquant : " + dossier.string());
    }

    MethodData md;
    // On memorise temporairement (classe, ech) -> vecteur pour trier ensuite.
    struct Entree { int classe, ech; std::string nom; std::vector<double> vec; };
    std::vector<Entree> entrees;

    for (const auto& e : fs::directory_iterator(dossier)) {
        if (!e.is_regular_file()) continue;
        const std::string fichier = e.path().filename().string();
        int classe, ech;
        if (!parse_filename(fichier, classe, ech)) continue;  // .DS_Store, etc.
        if (classe < 1 || classe > nb_classes) continue;      // filtrage 6/12/18

        std::vector<double> vec = read_descriptor_file(e.path().string());
        // Nom canonique sCCnNNN (classe sur 2 chiffres, echantillon sur 3).
        char buf[16];
        std::snprintf(buf, sizeof(buf), "s%02dn%03d", classe, ech);
        std::string nom(buf);
        entrees.push_back({classe, ech, nom, std::move(vec)});
    }

    // Tri (classe, echantillon) : alignement reproductible entre methodes.
    std::sort(entrees.begin(), entrees.end(), [](const Entree& a, const Entree& b) {
        return a.classe != b.classe ? a.classe < b.classe : a.ech < b.ech;
    });

    // Verification de la dimension constante.
    if (entrees.empty()) {
        throw std::runtime_error(std::string("Aucune forme pour la methode ") +
                                 method_name(m));
    }
    md.dim = static_cast<int>(entrees.front().vec.size());
    for (const auto& en : entrees) {
        if (static_cast<int>(en.vec.size()) != md.dim) {
            throw std::runtime_error(
                std::string("Dimension variable pour ") + method_name(m) + " : " +
                en.nom + " a " + std::to_string(en.vec.size()) +
                " valeurs au lieu de " + std::to_string(md.dim));
        }
        md.X.push_back(en.vec);
        md.y.push_back(en.classe);
        md.ech.push_back(en.ech);
        md.names.push_back(en.nom);
    }

    // Avertissement si la dimension differe de celle attendue (non bloquant).
    if (md.dim != method_expected_dim(m)) {
        std::cerr << "[AVERTISSEMENT] " << method_name(m) << " : dimension "
                  << md.dim << " (attendu " << method_expected_dim(m) << ")\n";
    }
    return md;
}

void Dataset::load(const std::string& racine, int nb_classes) {
    if (nb_classes != 6 && nb_classes != 12 && nb_classes != 18) {
        throw std::runtime_error("nb_classes doit valoir 6, 12 ou 18 (recu " +
                                 std::to_string(nb_classes) + ")");
    }
    nb_classes_ = nb_classes;

    for (Method m : ALL_METHODS) {
        data_[static_cast<int>(m)] = load_method(racine, m, nb_classes);
    }

    // Verification : les 5 methodes doivent decrire exactement les memes formes.
    const auto& ref = data_[0].names;
    for (int i = 1; i < NB_METHODES; ++i) {
        if (data_[i].names != ref) {
            throw std::runtime_error(
                std::string("Les formes de ") + method_name(ALL_METHODS[i]) +
                " ne correspondent pas a celles de " + method_name(ALL_METHODS[0]) +
                " : chaque forme doit avoir ses 5 descripteurs.");
        }
    }
}

// -----------------------------------------------------------------------------
//  Test de validite (tache 1).
// -----------------------------------------------------------------------------
void Dataset::print_summary() const {
    std::cout << "=== Chargement SharvitB2 : " << nb_classes_ << " classes, "
              << nb_formes() << " formes ===\n";

    std::cout << "Dimensions par methode :\n";
    for (Method m : ALL_METHODS) {
        std::cout << "  " << method_name(m) << " : dim = " << method(m).dim
                  << ", formes = " << method(m).size() << "\n";
    }

    // Nombre d'echantillons par classe (doit etre 12 partout).
    std::map<int, int> par_classe;
    for (int c : data_[0].y) par_classe[c]++;
    std::cout << "Echantillons par classe (doit etre 12) :\n  ";
    bool ok = true;
    for (const auto& [classe, n] : par_classe) {
        std::cout << "c" << classe << "=" << n << " ";
        if (n != 12) ok = false;
    }
    std::cout << "\n";
    std::cout << (ok ? "[OK] 12 echantillons par classe.\n"
                     : "[ERREUR] certaines classes n'ont pas 12 echantillons.\n");
}

}  // namespace rf
