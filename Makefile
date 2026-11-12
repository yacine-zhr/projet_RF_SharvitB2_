# Makefile du projet SharvitB2.
# Les deux membres du binôme compilent avec les mêmes options, définies ici.

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude
BUILD    := build

# Code commun à tous les programmes (le parser).
COMMUN   := $(BUILD)/Dataset.o

.PHONY: all run clean

all: load

# Le programme de test du chargement.
load: $(BUILD)/main_load.o $(COMMUN)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Chaque src/X.cpp est compilé en build/X.o. Ainsi, quand on modifie un seul
# fichier, make ne recompile que celui-là.
$(BUILD)/%.o: src/%.cpp include/Dataset.hpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

# Compile et lance le test sur les 6 premières classes.
run: all
	./load data/SharvitB2 6

clean:
	rm -rf $(BUILD) load
