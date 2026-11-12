// Dataset.hpp
// Lecture des signatures de la base SharvitB2.
//
// Chaque forme de la base est décrite par 5 fichiers texte (un par méthode),
// avec une valeur par ligne. Ce fichier déclare de quoi lire ces fichiers et
// ranger le tout dans une classe Dataset, prête pour les classifieurs.

#ifndef RF_DATASET_HPP
#define RF_DATASET_HPP

#include <array>
#include <string>
#include <vector>

namespace rf {

// Les 5 méthodes fournies par le prof. L'ordre compte : il sert d'indice
// dans le tableau de la classe Dataset (ART = 0, GFD = 1, etc.).
enum class Method { ART, GFD, SA, YNG, ZRK };
constexpr int NB_METHODES = 5;

// Pour pouvoir écrire simplement : for (Method m : ALL_METHODS) { ... }
constexpr std::array<Method, NB_METHODES> ALL_METHODS = {
    Method::ART, Method::GFD, Method::SA, Method::YNG, Method::ZRK};

// Nom lisible d'une méthode, pour les affichages ("ART", "ZRK"...).
const char* method_name(Method m);

// Nom du dossier qui contient les fichiers de la méthode. Attention, il ne
// correspond pas toujours au nom court : YNG est dans "Yang", ZRK dans "Zernike7".
const char* method_folder(Method m);

// Lit un fichier de signature et renvoie ses valeurs.
// Les lignes vides sont sautées (il y en a souvent une à la fin du fichier).
// Lève une exception si le fichier n'existe pas ou contient autre chose que des nombres.
std::vector<double> read_descriptor_file(const std::string& path);

// Récupère la classe et le numéro d'échantillon dans un nom de fichier.
// Exemple : "s01n005.art" donne classe = 1 et echantillon = 5.
// Renvoie false si le nom n'a pas la forme sCCnNNN (ex : un .DS_Store).
bool parse_filename(const std::string& filename, int& classe, int& echantillon);

// Toutes les données d'une méthode. La forme numéro i correspond à
// X[i] (son vecteur), y[i] (sa classe) et names[i] (son nom).
struct MethodData {
    std::vector<std::vector<double>> X;
    std::vector<int> y;
    std::vector<std::string> names;
    int dim = 0;  // longueur des vecteurs, la même pour toutes les formes

    std::size_t size() const { return y.size(); }
};

// Le jeu de données complet : les 5 méthodes chargées sur les mêmes formes,
// rangées dans le même ordre. Ainsi, la forme i est la même pour les 5 méthodes,
// ce dont on aura besoin pour le vote majoritaire.
class Dataset {
public:
    // Charge les classes 1 à nb_classes (6, 12 ou 18) depuis le dossier racine.
    // S'arrête avec une exception si quelque chose cloche : dossier absent,
    // vecteurs de longueurs différentes, forme manquante dans une méthode...
    void load(const std::string& racine, int nb_classes);

    const MethodData& method(Method m) const { return data_[static_cast<int>(m)]; }
    int nb_classes() const { return nb_classes_; }
    int nb_formes() const { return static_cast<int>(data_[0].size()); }

    // Affiche un petit bilan pour vérifier que le chargement s'est bien passé.
    void print_summary() const;

private:
    std::array<MethodData, NB_METHODES> data_;  // une case par méthode
    int nb_classes_ = 0;
};

}  // namespace rf

#endif
