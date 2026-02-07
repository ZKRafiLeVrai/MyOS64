#!/bin/bash

# Script de vérification de la structure du projet

echo "Vérification de la structure du projet MyOS64..."
echo ""

# Couleurs
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

errors=0
warnings=0

# Fonction de vérification
check_file() {
    if [ -f "$1" ]; then
        echo -e "${GREEN}✓${NC} $1"
    else
        echo -e "${RED}✗${NC} $1 - MANQUANT"
        ((errors++))
    fi
}

check_dir() {
    if [ -d "$1" ]; then
        echo -e "${GREEN}✓${NC} $1/"
    else
        echo -e "${RED}✗${NC} $1/ - MANQUANT"
        ((errors++))
    fi
}

# Vérification de la structure
echo "=== Structure des répertoires ==="
check_dir ".github/workflows"
check_dir "boot"
check_dir "kernel"
echo ""

echo "=== Fichiers de configuration ==="
check_file ".github/workflows/build.yml"
check_file "grub.cfg"
check_file "linker.ld"
check_file "Makefile"
check_file ".gitignore"
echo ""

echo "=== Fichiers sources ==="
check_file "boot/boot.asm"
check_file "kernel/kernel_entry.asm"
check_file "kernel/kernel.c"
check_file "kernel/terminal.c"
check_file "kernel/graphics.c"
check_file "kernel/gui.c"
echo ""

echo "=== Documentation ==="
check_file "README.md"
check_file "QUICKSTART.md"
echo ""

echo "=== Scripts ==="
check_file "build.sh"
if [ -f "build.sh" ]; then
    if [ -x "build.sh" ]; then
        echo -e "${GREEN}  → Exécutable${NC}"
    else
        echo -e "${YELLOW}  → Non exécutable (chmod +x build.sh)${NC}"
        ((warnings++))
    fi
fi
echo ""

# Résumé
echo "================================"
if [ $errors -eq 0 ]; then
    echo -e "${GREEN}✓ Structure du projet: OK${NC}"
    if [ $warnings -eq 0 ]; then
        echo -e "${GREEN}✓ Aucun avertissement${NC}"
    else
        echo -e "${YELLOW}⚠ $warnings avertissement(s)${NC}"
    fi
else
    echo -e "${RED}✗ $errors erreur(s) détectée(s)${NC}"
    if [ $warnings -gt 0 ]; then
        echo -e "${YELLOW}⚠ $warnings avertissement(s)${NC}"
    fi
fi
echo "================================"
echo ""

if [ $errors -eq 0 ]; then
    echo "Le projet est prêt à être compilé!"
    echo "Exécutez './build.sh' ou 'make' pour compiler."
    exit 0
else
    echo "Veuillez corriger les erreurs avant de compiler."
    exit 1
fi
