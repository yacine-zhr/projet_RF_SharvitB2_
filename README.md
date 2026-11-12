# Projet Reconnaissance des Formes — Base SharvitB2

M1 Informatique 2026/2027 — Implémentation C++ — Travail en binôme.

## Structure du dépôt

```
include/   en-têtes (.hpp)
src/       sources (.cpp)
data/      données SharvitB2
results/   sorties générées (CSV, figures)
report/    dossier d'analyse
```

## Compilation

```bash
make        # compile
make clean  # nettoie
```

Commande commune au binôme : `g++ -std=c++17 -O2 -Wall -Wextra` (voir `CXXFLAGS` du Makefile).

## Données (étape 2 : analyse du format)

| Méthode | Dossier    | Extension | Dimension |
|---------|------------|-----------|-----------|
| ART     | `ART`      | `.art`    | 36 (dernière valeur toujours 0) |
| GFD     | `GFD`      | `.gfd`    | 100 |
| SA      | `SA`       | `.sa`     | 20 |
| YNG     | `Yang`     | `.yng`    | 29 |
| ZRK     | `Zernike7` | `.zrk`    | 18 (valeurs jusqu'à ~13 000) |

- Fichier texte, une valeur par ligne, sans en-tête ; ligne vide finale possible.
- Nom : `sCCnNNN.ext` — `CC` = classe (01..18), `NNN` = échantillon (001..012).
- Vérification : `bash scripts/verif_donnees.sh` → 216 formes, 5 descripteurs
  par forme, 12 échantillons par classe, dimension constante par méthode.

Corrections apportées à l'archive d'origine (contenu inchangé) :
1. fichiers Zernike en double extension `.zrk.txt` → renommés `.zrk` ;
2. faute de frappe `s05n011.zrkt.txt` → renommé `s05n011.zrk`.
