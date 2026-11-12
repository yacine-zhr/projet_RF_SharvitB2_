# Projet Reconnaissance des Formes — Base SharvitB2

M1 Informatique 2026/2027 — Implémentation C++ — Travail en binôme.

Évaluation de 5 descripteurs de formes binaires (ART, ZRK, GFD, YNG, SA) associés
à des classifieurs (k-PPV, k-means, vote majoritaire) sur la base SharvitB2
(18 classes × 12 échantillons = 216 formes). Les vecteurs caractéristiques sont
déjà calculés : **aucun traitement d'image**, on lit des fichiers texte.

## Structure du dépôt

```
projet_RF_SharvitB2/
├── include/        en-têtes (.hpp)
│   └── Dataset.hpp
├── src/            sources (.cpp)
│   ├── Dataset.cpp      parser + classe Dataset (tâches 1, 2)
│   ├── main_load.cpp    programme "load"  : test de chargement
│   └── check_format.cpp programme "check" : vérification complète du format
├── data/
│   └── SharvitB2/  signatures (ART/ GFD/ SA/ Yang/ Zernike7/)
├── results/        sorties générées (CSV, figures) — ignoré par git
├── report/         dossier d'analyse (PDF)
├── Makefile
└── README.md
```

## Compilation

```bash
make          # compile les exécutables "load" et "check"
make run      # compile puis lance check (18 classes) et load (6 classes)
make clean    # supprime build/ et les binaires
```

Compilateur : `g++ -std=c++17 -O2 -Wall -Wextra`. **Vérifiez que les deux membres
du binôme compilent avec la même commande** (c'est le `CXXFLAGS` du Makefile).

## Utilisation

```bash
bash scripts/verif_donnees.sh  # analyse du format (étape 2)
./check data/SharvitB2         # même vérification via le parser C++
./load  data/SharvitB2 6       # charge les 6 premières classes (72 formes)
./load  data/SharvitB2 12      # 12 classes (144 formes)
./load  data/SharvitB2 18      # 18 classes (216 formes)
```

## Format des données

| Méthode | Dossier     | Extension | Dimension | Remarque |
|---------|-------------|-----------|-----------|----------|
| ART     | `ART`       | `.art`    | 36        | dernière composante constante = 0 |
| GFD     | `GFD`       | `.gfd`    | 100       | valeurs ~ [8e-5 ; 0,89] |
| SA      | `SA`        | `.sa`     | 20        | 1ère valeur ≈ 1 (0,99999–1,00001) |
| YNG     | `Yang`      | `.yng`    | 29        | **longueur fixe confirmée** |
| ZRK     | `Zernike7`  | `.zrk`    | 18        | valeurs ~ [0,007 ; 13211] → grande échelle |

- Fichier texte, **une valeur par ligne**, sans en-tête ni indice, ligne vide
  finale ignorée.
- Nom : `sCCnNNN.ext` — `CC` = classe (01..18), `NNN` = échantillon (001..012).

### Vérifications faites (résultat de `./check`)

- ✅ 216 formes, chaque forme possède bien ses **5 descripteurs** ;
- ✅ dimension **constante** par méthode (valeurs du tableau ci-dessus) ;
- ✅ **12 échantillons par classe** pour les 18 classes ;
- ✅ **YNG a une longueur fixe de 29** (la question de la tâche 1 est réglée) ;
- ℹ️ une seule composante constante détectée (dernière valeur de ART) → à gérer
  dans la normalisation z-score (écart-type nul : ignorer la composante ou
  ajouter un epsilon).

### Nettoyage appliqué aux données d'origine

L'archive fournie contenait deux anomalies de nommage, corrigées dans ce dépôt :

1. Les fichiers Zernike étaient en **double extension** `.zrk.txt` → renommés `.zrk`.
2. Un fichier était mal nommé : `s05n011.zrkt.txt` (faute de frappe `zrkt`) →
   renommé `s05n011.zrk`. Son vecteur a bien 18 valeurs.

Les dossiers ont aussi été renommés sans le préfixe `=` présent dans l'archive.
Le parser (`parse_filename`) reste néanmoins tolérant (casse, extension, chemin).

## Suite (tâches 2 à 4)

Le socle de lecture est en place. Prochaines briques à poser sur `Dataset` :
normalisation z-score (tâche 2), distances L1/L2/Minkowski (tâche 3),
protocole leave-one-out (tâche 4), puis le k-PPV de base.

