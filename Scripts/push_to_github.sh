#!/bin/bash

# Script pour pousser MyOS64 sur GitHub depuis Linux/Mac
# Assurez-vous d'avoir Git installé

set -e

echo "========================================"
echo "   MyOS64 - GitHub Upload Script"
echo "========================================"
echo ""

# Vérifier si Git est installé
if ! command -v git &> /dev/null; then
    echo "❌ [ERREUR] Git n'est pas installé"
    echo "Installez Git avec:"
    echo "  Ubuntu/Debian: sudo apt-get install git"
    echo "  macOS: brew install git"
    exit 1
fi

echo "✓ Git est installé"
echo ""

# Demander le nom d'utilisateur GitHub
read -p "Entrez votre nom d'utilisateur GitHub: " GITHUB_USER

if [ -z "$GITHUB_USER" ]; then
    echo "❌ [ERREUR] Le nom d'utilisateur ne peut pas être vide"
    exit 1
fi

echo ""
echo "Configuration du repository..."
echo "Repository URL: https://github.com/$GITHUB_USER/MyOS64.git"
echo ""

# Initialiser Git si nécessaire
if [ ! -d ".git" ]; then
    echo "Initialisation du dépôt Git..."
    git init
    echo "✓ Dépôt Git initialisé"
    echo ""
fi

# Configurer le remote
echo "Configuration du remote GitHub..."
git remote remove origin 2>/dev/null || true
git remote add origin "https://github.com/$GITHUB_USER/MyOS64.git"
echo "✓ Remote configuré"
echo ""

# Ajouter tous les fichiers
echo "Ajout des fichiers..."
git add .
echo "✓ Fichiers ajoutés"
echo ""

# Créer le commit
echo "Création du commit..."
if git commit -m "Initial commit: MyOS64 - Système d'exploitation 64-bit complet"; then
    echo "✓ Commit créé"
else
    echo "ℹ Pas de nouveaux changements à commiter ou commit déjà existant"
fi
echo ""

# Renommer la branche en main
echo "Renommage de la branche en 'main'..."
git branch -M main
echo "✓ Branche renommée"
echo ""

# Pousser vers GitHub
echo "========================================"
echo "Push vers GitHub..."
echo "========================================"
echo ""
echo "IMPORTANT: Utilisez votre Personal Access Token comme mot de passe"
echo "(PAS votre mot de passe GitHub)"
echo ""
echo "Pour créer un token:"
echo "1. Allez sur GitHub.com"
echo "2. Settings > Developer settings > Personal access tokens > Tokens (classic)"
echo "3. Generate new token"
echo "4. Sélectionnez 'repo' scope"
echo "5. Copiez le token généré"
echo ""
read -p "Appuyez sur Entrée pour continuer..."
echo ""

if git push -u origin main; then
    echo ""
    echo "========================================"
    echo "✓ [SUCCÈS] Code poussé sur GitHub !"
    echo "========================================"
    echo ""
    echo "Prochaines étapes:"
    echo "1. Allez sur https://github.com/$GITHUB_USER/MyOS64"
    echo "2. Cliquez sur l'onglet 'Actions'"
    echo "3. Attendez que la compilation se termine (2-3 minutes)"
    echo "4. Téléchargez l'ISO depuis 'Actions' ou 'Releases'"
    echo ""
    echo "Repository: https://github.com/$GITHUB_USER/MyOS64"
    echo ""
else
    echo ""
    echo "========================================"
    echo "✗ [ERREUR] Échec du push"
    echo "========================================"
    echo ""
    echo "Vérifiez:"
    echo "- Que le repository existe sur GitHub"
    echo "- Que vous avez utilisé le bon username"
    echo "- Que vous avez utilisé un Personal Access Token"
    echo ""
    exit 1
fi
