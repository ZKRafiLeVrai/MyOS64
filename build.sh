#!/bin/bash

# Script de build et test rapide pour MyOS64

set -e

echo "======================================"
echo "  MyOS64 - Build & Test Script"
echo "======================================"
echo ""

# Vérifier les dépendances
echo "[1/5] Vérification des dépendances..."
if ! command -v nasm &> /dev/null; then
    echo "❌ NASM n'est pas installé"
    echo "   Installez avec: sudo apt-get install nasm"
    exit 1
fi

if ! command -v gcc &> /dev/null; then
    echo "❌ GCC n'est pas installé"
    echo "   Installez avec: sudo apt-get install gcc"
    exit 1
fi

if ! command -v grub-mkrescue &> /dev/null; then
    echo "❌ GRUB tools ne sont pas installés"
    echo "   Installez avec: sudo apt-get install grub-pc-bin grub-common"
    exit 1
fi

echo "✓ Toutes les dépendances sont présentes"
echo ""

# Nettoyer
echo "[2/5] Nettoyage des fichiers précédents..."
make clean &> /dev/null || true
echo "✓ Nettoyage terminé"
echo ""

# Compiler
echo "[3/5] Compilation de MyOS64..."
if make all; then
    echo "✓ Compilation réussie"
else
    echo "❌ Échec de la compilation"
    exit 1
fi
echo ""

# Vérifier l'ISO
echo "[4/5] Vérification de l'ISO..."
if [ -f "MyOS64.iso" ]; then
    SIZE=$(du -h MyOS64.iso | cut -f1)
    echo "✓ ISO créé avec succès (taille: $SIZE)"
else
    echo "❌ L'ISO n'a pas été créé"
    exit 1
fi
echo ""

# Proposer de tester
echo "[5/5] Test de l'OS..."
if command -v qemu-system-x86_64 &> /dev/null; then
    read -p "Voulez-vous tester MyOS64 avec QEMU? (y/n) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Démarrage de QEMU..."
        echo "(Appuyez sur Ctrl+Alt+G pour libérer la souris)"
        echo ""
        make run
    fi
else
    echo "⚠ QEMU n'est pas installé"
    echo "  Installez avec: sudo apt-get install qemu-system-x86"
    echo "  Ou testez l'ISO avec VirtualBox/VMware"
fi

echo ""
echo "======================================"
echo "  Build terminé avec succès!"
echo "======================================"
echo ""
echo "Fichier ISO: MyOS64.iso"
echo ""
echo "Pour tester:"
echo "  - QEMU: make run"
echo "  - VirtualBox: Créer une VM et monter MyOS64.iso"
echo "  - VMware: Créer une VM et utiliser MyOS64.iso"
echo ""
