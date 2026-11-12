#!/bin/bash
# Étape 2 : vérification des données avant d'écrire le parser.
# On contrôle tous les fichiers, pas seulement quelques exemples :
#   - chaque classe a bien 12 échantillons ;
#   - chaque forme a bien ses 5 descripteurs ;
#   - les vecteurs ont la même longueur dans une même méthode ;
#   - les valeurs min et max de chaque méthode (pour voir les écarts d'échelle).
#
# Utilisation : bash scripts/verif_donnees.sh [dossier des données]

D=${1:-data/SharvitB2}
METHODES="ART GFD SA Yang Zernike7"

# On extrait le numéro de classe de chaque nom de fichier, puis on compte.
echo "Échantillons par classe :"
for m in $METHODES; do
  echo -n "  $m : "
  ls "$D/$m" | sed -E 's/^s([0-9]{2})n.*/\1/' | sort | uniq -c \
    | awk '{printf "c%s=%s ", $2, $1} END {print ""}'
done

# On compare la liste des formes (nom sans extension) de chaque méthode à
# celle d'ART. Zéro différence veut dire que les mêmes formes sont présentes.
echo "Formes présentes dans chaque méthode :"
for m in $METHODES; do ls "$D/$m" | cut -c1-7 | sort > /tmp/formes_$m.txt; done
for m in GFD SA Yang Zernike7; do
  n=$(comm -3 /tmp/formes_ART.txt /tmp/formes_$m.txt | wc -l)
  echo "  ART vs $m : $n différence(s)"
done

# On compte les lignes non vides de chaque fichier (= longueur du vecteur),
# puis on regroupe : une seule longueur par méthode veut dire qu'elle est fixe.
echo "Longueur des vecteurs :"
for m in $METHODES; do
  echo -n "  $m : "
  for f in "$D/$m"/*; do awk 'NF' "$f" | wc -l; done | sort | uniq -c \
    | awk '{printf "%s valeurs dans %s fichiers  ", $2, $1} END {print ""}'
done

# On trie toutes les valeurs d'une méthode et on garde la première et la dernière.
echo "Valeurs min et max :"
for m in $METHODES; do
  echo -n "  $m : "
  cat "$D/$m"/* | awk 'NF' | sort -g | sed -n '1p;$p' | tr '\n' ' '; echo
done
