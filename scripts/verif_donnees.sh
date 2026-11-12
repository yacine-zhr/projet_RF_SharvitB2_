#!/bin/bash
# =============================================================================
#  Etape 2 : analyse du format des donnees SharvitB2.
#  Verifie sur TOUS les fichiers :
#   - 12 echantillons par classe pour chaque methode ;
#   - chaque forme possede ses 5 descripteurs ;
#   - la dimension (nombre de valeurs) est constante par methode ;
#   - plage des valeurs (ecarts d'echelle entre methodes).
#  Usage : bash scripts/verif_donnees.sh [data/SharvitB2]
# =============================================================================
D=${1:-data/SharvitB2}
METHODES="ART GFD SA Yang Zernike7"

echo "=== Echantillons par classe ==="
for m in $METHODES; do
  echo -n "$m : "
  ls "$D/$m" | sed -E 's/^s([0-9]{2})n.*/\1/' | sort | uniq -c \
    | awk '{printf "c%s=%s ", $2, $1} END {print ""}'
done

echo "=== 5 descripteurs par forme ==="
for m in $METHODES; do ls "$D/$m" | cut -c1-7 | sort > /tmp/formes_$m.txt; done
for m in GFD SA Yang Zernike7; do
  n=$(comm -3 /tmp/formes_ART.txt /tmp/formes_$m.txt | wc -l)
  echo "ART vs $m : $n difference(s)"
done

echo "=== Dimension par methode (dimension : nombre de fichiers) ==="
for m in $METHODES; do
  echo -n "$m : "
  for f in "$D/$m"/*; do awk 'NF' "$f" | wc -l; done | sort | uniq -c \
    | awk '{printf "%s valeurs : %s fichiers  ", $2, $1} END {print ""}'
done

echo "=== Plage des valeurs (min / max) ==="
for m in $METHODES; do
  echo -n "$m : "
  cat "$D/$m"/* | awk 'NF' | sort -g | sed -n '1p;$p' | tr '\n' ' '; echo
done
