# =============================================================================
#  Projet Reconnaissance des Formes - Base SharvitB2
#  Makefile du socle (taches 0 a 1).
#  Compilation :  g++ -std=c++17 -O2 -Wall
# =============================================================================

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude
SRCDIR   := src
BUILDDIR := build

# Fichiers communs (la bibliotheque du projet).
COMMON   := $(BUILDDIR)/Dataset.o

# Programmes executables (un main = un binaire).
BINS     := load check

.PHONY: all clean run

all: $(BINS)

# --- Edition de liens des executables ---
load:  $(BUILDDIR)/main_load.o $(COMMON)
	$(CXX) $(CXXFLAGS) -o $@ $^

check: $(BUILDDIR)/check_format.o $(COMMON)
	$(CXX) $(CXXFLAGS) -o $@ $^

# --- Compilation des objets ---
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# --- Raccourcis ---
# Verification complete du format (18 classes) puis chargement sur 6 classes.
run: all
	./check data/SharvitB2
	@echo
	./load data/SharvitB2 6

clean:
	rm -rf $(BUILDDIR) $(BINS)
