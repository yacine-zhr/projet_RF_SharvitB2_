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
