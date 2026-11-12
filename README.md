# Projet Reconnaissance des Formes — Base SharvitB2

M1 Informatique 2026/2027 — projet en binôme, en C++.

On compare 5 descripteurs de formes (ART, ZRK, GFD, YNG, SA) associés à des
classifieurs (k-PPV, k-means, vote majoritaire) sur la base SharvitB2 :
18 classes de 12 formes, soit 216 formes. Les vecteurs sont déjà calculés,
on ne fait donc aucun traitement d'image : on lit des fichiers texte.

## Organisation

```
include/Dataset.hpp        déclaration du parser et de la classe Dataset
src/Dataset.cpp            lecture des fichiers et chargement des données
src/main_load.cpp          programme de test du chargement
scripts/verif_donnees.sh   vérification des données (étape 2)
data/SharvitB2/            les signatures : ART/ GFD/ SA/ Yang/ Zernike7/
results/                   résultats générés par nos programmes
report/                    dossier d'analyse
```

## Compiler et lancer

```bash
make                            # compile le programme load
make run                        # compile et charge les 6 premières classes
make clean                      # efface ce qui a été compilé

./load data/SharvitB2 12        # charge 12 classes (144 formes)
bash scripts/verif_donnees.sh   # vérifie le format de tous les fichiers
```

Options de compilation communes au binôme : `g++ -std=c++17 -O2 -Wall -Wextra`.

## Les données

| Méthode | Dossier    | Extension | Longueur du vecteur |
|---------|------------|-----------|---------------------|
| ART     | `ART`      | `.art`    | 36                  |
| GFD     | `GFD`      | `.gfd`    | 100                 |
| SA      | `SA`       | `.sa`     | 20                  |
| YNG     | `Yang`     | `.yng`    | 29                  |
| ZRK     | `Zernike7` | `.zrk`    | 18                  |

Une valeur par ligne, sans en-tête. Les fichiers s'appellent `sCCnNNN.ext` :
`CC` est la classe (01 à 18) et `NNN` le numéro de l'échantillon (001 à 012).

Ce que la vérification a montré :
- les 216 formes ont bien leurs 5 descripteurs, et chaque classe a 12 formes ;
- la longueur des vecteurs est fixe pour chaque méthode, y compris pour YNG ;
- les échelles sont très différentes (jusqu'à 13 000 pour ZRK, moins de 1
  ailleurs), ce qui imposera une normalisation ;
- la dernière valeur d'ART vaut toujours 0.

Deux erreurs de nommage de l'archive d'origine ont été corrigées (le contenu
des fichiers n'a pas changé) : les fichiers Zernike avaient une double
extension `.zrk.txt`, et l'un d'eux s'appelait `s05n011.zrkt.txt`. Tous sont
maintenant en `.zrk`.

## Prochaines étapes

Normalisation des vecteurs, distances L1/L2, protocole leave-one-out, puis
k plus proches voisins sur les 6 premières classes.
