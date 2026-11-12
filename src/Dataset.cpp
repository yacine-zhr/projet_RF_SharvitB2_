// Dataset.cpp
// Lecture des fichiers de signatures et construction du Dataset.

#include "Dataset.hpp"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

namespace rf {

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
        case Method::YNG: return "Yang";
        case Method::ZRK: return "Zernike7";
    }
    return "?";
}

std::vector<double> read_descriptor_file(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Fichier introuvable : " + path);
    }

    std::vector<double> valeurs;
    std::string ligne;
    int numero = 0;  // uniquement pour dire où est l'erreur, s'il y en a une

    while (std::getline(in, ligne)) {
        ++numero;

        // On enlève les espaces autour de la valeur. Le \r est là pour les
        // fichiers enregistrés sous Windows, qui finissent leurs lignes par \r\n.
        auto debut = ligne.find_first_not_of(" \t\r\n");
        if (debut == std::string::npos) continue;  // ligne vide, on passe
        auto fin = ligne.find_last_not_of(" \t\r\n");
        std::string texte = ligne.substr(debut, fin - debut + 1);

        // Conversion du texte en nombre. Si ça échoue, le fichier est abîmé
        // et on préfère s'arrêter tout de suite plutôt que de continuer avec
        // des données fausses.
        std::istringstream iss(texte);
        double x;
        if (!(iss >> x)) {
            throw std::runtime_error("Valeur non numérique dans " + path + ", ligne " +
                                     std::to_string(numero) + " : '" + texte + "'");
        }
        valeurs.push_back(x);
    }

    if (valeurs.empty()) {
        throw std::runtime_error("Fichier vide : " + path);
    }
    return valeurs;
}

bool parse_filename(const std::string& filename, int& classe, int& echantillon) {
    // stem() retire le chemin et l'extension : "data/ART/s01n005.art" -> "s01n005".
    // On coupe aussi au premier point restant au cas où il y aurait une double
    // extension, comme les ".zrk.txt" de l'archive d'origine.
    std::string nom = fs::path(filename).stem().string();
    auto point = nom.find('.');
    if (point != std::string::npos) nom = nom.substr(0, point);

    // On attend exactement 7 caractères : s + 2 chiffres + n + 3 chiffres.
    // Le passage en minuscules permet d'accepter aussi "S01N005".
    if (nom.size() != 7) return false;
    std::transform(nom.begin(), nom.end(), nom.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (nom[0] != 's' || nom[3] != 'n') return false;
    for (int i : {1, 2, 4, 5, 6}) {
        if (!std::isdigit(static_cast<unsigned char>(nom[i]))) return false;
    }

    classe = std::stoi(nom.substr(1, 2));       // "01"  -> 1
    echantillon = std::stoi(nom.substr(4, 3));  // "005" -> 5
    return true;
}

// Charge toutes les formes d'une méthode (fonction interne à ce fichier).
static MethodData load_method(const std::string& racine, Method m, int nb_classes) {
    fs::path dossier = fs::path(racine) / method_folder(m);
    if (!fs::exists(dossier)) {
        throw std::runtime_error("Dossier manquant : " + dossier.string());
    }

    // Le dossier ne rend pas les fichiers dans un ordre fiable. On les garde
    // donc d'abord dans une liste temporaire, qu'on triera ensuite.
    struct Forme {
        int classe;
        int echantillon;
        std::string nom;
        std::vector<double> vecteur;
    };
    std::vector<Forme> formes;

    for (const auto& entree : fs::directory_iterator(dossier)) {
        if (!entree.is_regular_file()) continue;

        int classe, echantillon;
        if (!parse_filename(entree.path().filename().string(), classe, echantillon)) {
            continue;  // fichier qui n'est pas une signature (.DS_Store par exemple)
        }
        // C'est ici qu'on ne garde que les 6, 12 ou 18 premières classes.
        if (classe < 1 || classe > nb_classes) continue;

        // On reconstruit un nom propre "sCCnNNN", identique pour les 5 méthodes,
        // quelle que soit la façon dont le fichier était écrit sur le disque.
        char nom[16];
        std::snprintf(nom, sizeof(nom), "s%02dn%03d", classe, echantillon);

        formes.push_back({classe, echantillon, nom,
                          read_descriptor_file(entree.path().string())});
    }

    if (formes.empty()) {
        throw std::runtime_error(std::string("Aucune forme trouvée pour ") + method_name(m));
    }

    // Tri par classe, puis par numéro d'échantillon. Comme les 5 méthodes sont
    // triées de la même façon, la forme i est la même partout.
    std::sort(formes.begin(), formes.end(), [](const Forme& a, const Forme& b) {
        if (a.classe != b.classe) return a.classe < b.classe;
        return a.echantillon < b.echantillon;
    });

    // On range les formes dans la structure finale, en vérifiant au passage
    // que tous les vecteurs ont la même longueur que le premier.
    MethodData md;
    md.dim = static_cast<int>(formes.front().vecteur.size());
    for (auto& f : formes) {
        if (static_cast<int>(f.vecteur.size()) != md.dim) {
            throw std::runtime_error(std::string("Longueur différente pour ") + method_name(m) +
                                     " : " + f.nom + " a " + std::to_string(f.vecteur.size()) +
                                     " valeurs au lieu de " + std::to_string(md.dim));
        }
        md.X.push_back(std::move(f.vecteur));  // déplacé plutôt que copié
        md.y.push_back(f.classe);
        md.names.push_back(f.nom);
    }
    return md;
}

void Dataset::load(const std::string& racine, int nb_classes) {
    if (nb_classes != 6 && nb_classes != 12 && nb_classes != 18) {
        throw std::runtime_error("nb_classes doit valoir 6, 12 ou 18 (reçu " +
                                 std::to_string(nb_classes) + ")");
    }
    nb_classes_ = nb_classes;

    for (Method m : ALL_METHODS) {
        data_[static_cast<int>(m)] = load_method(racine, m, nb_classes);
    }

    // Dernière vérification : les 5 méthodes doivent décrire exactement les
    // mêmes formes. Si une liste de noms diffère, il manque un fichier quelque part.
    const auto& reference = data_[0].names;
    for (int i = 1; i < NB_METHODES; ++i) {
        if (data_[i].names != reference) {
            throw std::runtime_error(std::string("Les formes de ") + method_name(ALL_METHODS[i]) +
                                     " ne sont pas les mêmes que celles de ART");
        }
    }
}

void Dataset::print_summary() const {
    std::cout << "Chargement : " << nb_classes_ << " classes, " << nb_formes() << " formes\n";

    for (Method m : ALL_METHODS) {
        std::cout << "  " << method_name(m) << " : vecteurs de " << method(m).dim
                  << " valeurs\n";
    }

    // On compte les formes de chaque classe : on en attend 12 partout.
    std::map<int, int> par_classe;
    for (int c : data_[0].y) par_classe[c]++;

    bool ok = true;
    std::cout << "Formes par classe :";
    for (const auto& [classe, n] : par_classe) {
        std::cout << " c" << classe << "=" << n;
        if (n != 12) ok = false;
    }
    std::cout << "\n" << (ok ? "OK, 12 formes dans chaque classe.\n"
                             : "Problème : certaines classes n'ont pas 12 formes.\n");
}

}  // namespace rf
