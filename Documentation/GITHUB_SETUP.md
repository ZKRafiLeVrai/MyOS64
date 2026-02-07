# 🚀 Guide de Mise en Place sur GitHub

## Étape 1: Créer le dépôt GitHub

1. Allez sur https://github.com
2. Cliquez sur le bouton **"New repository"** (ou "New" → "Repository")
3. Remplissez les informations :
   - **Repository name**: `MyOS64`
   - **Description**: "Système d'exploitation 64-bit avec interface graphique style Windows 11"
   - **Public** ou **Private** : à votre choix
   - **N'initialisez PAS** avec README, .gitignore, ou license (on a déjà tout)
4. Cliquez sur **"Create repository"**

## Étape 2: Préparer le projet localement

### Sur Windows (avec Git Bash ou WSL)

```bash
# Aller dans le dossier MyOS64
cd MyOS64

# Initialiser git
git init

# Ajouter tous les fichiers
git add .

# Premier commit
git commit -m "Initial commit: MyOS64 - Système d'exploitation 64-bit complet"

# Renommer la branche en main (GitHub standard)
git branch -M main
```

## Étape 3: Pousser vers GitHub

Remplacez `VOTRE-USERNAME` par votre nom d'utilisateur GitHub :

```bash
# Ajouter le remote
git remote add origin https://github.com/VOTRE-USERNAME/MyOS64.git

# Pousser le code
git push -u origin main
```

Si on vous demande vos identifiants :
- **Username**: votre nom d'utilisateur GitHub
- **Password**: utilisez un **Personal Access Token** (pas votre mot de passe)
  - Pour créer un token : GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic) → Generate new token

## Étape 4: Vérifier la compilation automatique

1. Allez sur votre dépôt GitHub
2. Cliquez sur l'onglet **"Actions"**
3. Vous devriez voir un workflow "Build MyOS64" en cours d'exécution
4. Attendez 2-3 minutes que la compilation se termine
5. ✅ Si tout est vert, c'est réussi !

## Étape 5: Télécharger l'ISO compilé

### Méthode 1: Artifacts (temporaire)

1. Dans **Actions**, cliquez sur le workflow terminé
2. Descendez jusqu'à **"Artifacts"**
3. Téléchargez **"MyOS64-ISO"**
4. Décompressez le fichier ZIP
5. Vous avez maintenant `MyOS64.iso` !

### Méthode 2: Releases (permanent)

1. Allez dans l'onglet **"Releases"** de votre dépôt
2. Vous verrez une release automatique créée
3. Téléchargez `MyOS64.iso` directement depuis la release
4. Cette ISO reste disponible de façon permanente

## Étape 6: Tester l'OS

### Option A: QEMU (Rapide)

**Windows** :
1. Télécharger QEMU : https://qemu.weilnetz.de/w64/
2. Installer QEMU
3. Ouvrir un terminal dans le dossier de l'ISO
4. Exécuter :
```cmd
"C:\Program Files\qemu\qemu-system-x86_64.exe" -cdrom MyOS64.iso -m 512M
```

**Linux/WSL** :
```bash
sudo apt-get install qemu-system-x86
qemu-system-x86_64 -cdrom MyOS64.iso -m 512M
```

### Option B: VirtualBox

1. Télécharger VirtualBox : https://www.virtualbox.org/
2. Installer VirtualBox
3. Créer une nouvelle VM :
   - **Nom** : MyOS64
   - **Type** : Linux
   - **Version** : Other Linux (64-bit)
   - **Mémoire** : 512 MB (minimum)
   - **Disque** : Ne pas créer de disque (on utilise l'ISO)
4. Configuration → Stockage → Ajouter CD → Sélectionner `MyOS64.iso`
5. Démarrer la VM

### Option C: VMware

1. Télécharger VMware Workstation Player (gratuit)
2. Créer une nouvelle VM
3. Type : Other Linux 64-bit
4. Monter `MyOS64.iso` comme CD-ROM
5. Démarrer

## 🎯 Utilisation de l'OS

### Au démarrage

Vous verrez le menu GRUB :
```
MyOS64
MyOS64 (Mode texte)
Redémarrer
Éteindre
```

Sélectionnez "MyOS64" et appuyez sur Entrée.

### Mode Terminal

```
====================================
     Bienvenue dans MyOS64!        
====================================

Système d'exploitation 64-bit
Version 1.0.0

MyOS64> _
```

**Commandes disponibles** :
- `help` - Afficher l'aide
- `clear` - Effacer l'écran
- `gui` - Passer en mode graphique
- `info` - Informations système
- `reboot` - Redémarrer

### Mode Graphique

1. Tapez `gui` dans le terminal
2. Appuyez sur Entrée
3. L'interface graphique style Windows 11 s'affiche
4. Appuyez sur **ESC** pour retourner au terminal

## 🔄 Modifications et recompilation

Pour modifier le code et recompiler :

```bash
# Modifier les fichiers (kernel.c, gui.c, etc.)

# Ajouter les changements
git add .

# Commit
git commit -m "Description de vos modifications"

# Pousser
git push

# GitHub Actions recompilera automatiquement !
```

## 📊 Badge de statut

Ajoutez ce badge à votre README pour montrer le statut de compilation :

```markdown
[![Build Status](https://github.com/VOTRE-USERNAME/MyOS64/workflows/Build%20MyOS64/badge.svg)](https://github.com/VOTRE-USERNAME/MyOS64/actions)
```

## 🐛 Problèmes courants

### "Permission denied" sur Git Push

Solution :
1. Utilisez un Personal Access Token au lieu d'un mot de passe
2. Ou configurez SSH : https://docs.github.com/en/authentication/connecting-to-github-with-ssh

### GitHub Actions échoue

1. Vérifiez les logs dans l'onglet Actions
2. Assurez-vous que tous les fichiers sont bien poussés
3. Vérifiez que build.yml est bien dans `.github/workflows/`

### L'OS ne démarre pas

1. Vérifiez que votre VM est configurée en 64-bit
2. Augmentez la RAM à au moins 512 MB
3. Essayez avec un autre émulateur (QEMU vs VirtualBox)

### Écran noir après GRUB

- C'est normal avec certaines configurations QEMU
- Essayez VirtualBox
- Le mode terminal devrait fonctionner dans tous les cas

## 🎉 Félicitations !

Vous avez maintenant :
- ✅ Un OS 64-bit complet
- ✅ Compilé automatiquement sur GitHub
- ✅ Disponible en téléchargement (ISO)
- ✅ Testable sur n'importe quelle machine

## 📚 Prochaines étapes

1. **Personnalisez** l'interface (couleurs, messages)
2. **Ajoutez** de nouvelles commandes
3. **Implémentez** de nouvelles fonctionnalités
4. **Partagez** votre projet !

## 🤝 Partage

Votre OS est maintenant public sur GitHub. Vous pouvez :
- Le partager avec des amis
- Le montrer dans votre portfolio
- Recevoir des contributions
- Apprendre et expérimenter

---

**Questions ?** Créez une issue sur GitHub ou consultez la documentation dans README.md

**Bon codage !** 🚀
