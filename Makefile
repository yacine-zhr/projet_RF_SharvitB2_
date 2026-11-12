# =============================================================================
#  Projet Reconnaissance des Formes - Base SharvitB2
#  Compilation commune au binome : g++ -std=c++17 -O2 -Wall
# =============================================================================

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude
SRCDIR   := src
BUILDDIR := build

# Executables du projet (ajoutes au fur et a mesure).
BINS     :=

.PHONY: all clean

all: $(BINS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR) $(BINS)
