#ifndef RF_DATASET_HPP
#define RF_DATASET_HPP

// =============================================================================
//  Projet Reconnaissance des Formes - M1 2026/2027 - Base SharvitB2
//  Socle : lecture des donnees (parser + classe Dataset)
//  Taches 1 et 2 du PDF "Liste des taches".
//
//  Rappel : les vecteurs caracteristiques sont deja calcules. Aucun traitement
//  d'image. On lit simplement des fichiers texte (une valeur par ligne).
// =============================================================================

#include <string>
#include <vector>
#include <array>

namespace rf {

// -----------------------------------------------------------------------------
//  Les 5 methodes de description de formes.
// -----------------------------------------------------------------------------
enum class Method { ART = 0, GFD, SA, YNG, ZRK };
constexpr int NB_METHODES = 5;

// Toutes les methodes, pratique pour boucler : for (Method m : ALL_METHODS)
constexpr std::array<Method, NB_METHODES> ALL_METHODS = {
    Method::ART, Method::GFD, Method::SA, Method::YNG, Method::ZRK};

// Nom court de la methode, ex : "ART".
const char* method_name(Method m);
// Sous-dossier des signatures, ex : "Zernike7" pour ZRK.
const char* method_folder(Method m);
// Extension des fichiers, ex : ".zrk".
const char* method_ext(Method m);
// Dimension attendue du vecteur (verifiee au chargement) : ART=36, GFD=100,
// SA=20, YNG=29, ZRK=18.
int method_expected_dim(Method m);

// -----------------------------------------------------------------------------
//  Parser bas niveau.
// -----------------------------------------------------------------------------

// Lit un fichier descripteur : une valeur (double) par ligne. Les lignes vides
// (dont la ligne vide finale eventuelle) et les espaces sont ignores.
// Lance std::runtime_error si le fichier est introuvable ou contient une ligne
// non numerique.
std::vector<double> read_descriptor_file(const std::string& path);

// Extrait la classe (1..18) et l'indice d'echantillon (1..12) a partir d'un nom
// de type sCCnNNN(.ext). Tolere majuscules/minuscules et le chemin complet.
// Renvoie false si le nom ne respecte pas le format.
bool parse_filename(const std::string& filename, int& classe, int& echantillon);

// -----------------------------------------------------------------------------
//  Donnees d'UNE methode.
// -----------------------------------------------------------------------------
struct MethodData {
    std::vector<std::vector<double>> X;  // matrice n x dim : un vecteur par forme
    std::vector<int>                 y;  // etiquette de classe (1..nb_classes)
    std::vector<int>                 ech;// indice d'echantillon (1..12)
    std::vector<std::string>         names;  // "sCCnNNN"
    int dim = 0;                         // dimension commune des vecteurs

    std::size_t size() const { return y.size(); }
};

// -----------------------------------------------------------------------------
//  Jeu de donnees complet (les 5 methodes alignees sur les memes formes).
// -----------------------------------------------------------------------------
class Dataset {
public:
    // Charge les classes 1..nb_classes (nb_classes dans {6,12,18}) depuis
    // <racine>/<dossier methode>/. Les formes sont triees par (classe, ech)
    // pour que les 5 methodes soient alignees ligne a ligne.
    // Lance std::runtime_error en cas d'incoherence (dimension variable,
    // forme manquante dans une methode, etc.).
    void load(const std::string& racine, int nb_classes);

    const MethodData& method(Method m) const { return data_[static_cast<int>(m)]; }
    int nb_classes() const { return nb_classes_; }
    // Nombre de formes chargees (identique pour toutes les methodes).
    int nb_formes()  const { return static_cast<int>(data_[0].size()); }

    // Affiche le test de validite demande par la tache 1 :
    // nombre de formes, dimension par methode, nombre d'echantillons par classe.
    void print_summary() const;

private:
    std::array<MethodData, NB_METHODES> data_;
    int nb_classes_ = 0;
};

}  // namespace rf

#endif  // RF_DATASET_HPP
