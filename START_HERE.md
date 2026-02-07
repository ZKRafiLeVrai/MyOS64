# 🎯 COMMENCEZ ICI - MyOS64

Bienvenue dans MyOS64, votre système d'exploitation 64-bit complet !

## 🚀 Démarrage Ultra-Rapide (3 étapes)

### ✅ Étape 1: Créer le dépôt GitHub
1. Allez sur https://github.com/new
2. Nom du repository: `MyOS64`
3. Cochez "Public"
4. **NE PAS** initialiser avec README
5. Cliquez "Create repository"

### ✅ Étape 2: Pousser le code

**Sur Windows** (double-cliquez sur le fichier) :
```
push_to_github.bat
```

**Sur Linux/Mac** (dans le terminal) :
```bash
./push_to_github.sh
```

**Ou manuellement** :
```bash
git init
git add .
git commit -m "Initial commit: MyOS64"
git branch -M main
git remote add origin https://github.com/VOTRE-USERNAME/MyOS64.git
git push -u origin main
```

### ✅ Étape 3: Récupérer l'ISO
1. Allez sur votre dépôt GitHub
2. Onglet **"Actions"** → Attendez la fin (2-3 min)
3. Téléchargez l'artifact **"MyOS64-ISO"**
4. **OU** Onglet **"Releases"** → Téléchargez `MyOS64.iso`

## 🎮 Tester l'OS

### Option 1: QEMU (Rapide)

**Windows** :
```cmd
"C:\Program Files\qemu\qemu-system-x86_64.exe" -cdrom MyOS64.iso -m 512M
```

**Linux/Mac** :
```bash
qemu-system-x86_64 -cdrom MyOS64.iso -m 512M
```

### Option 2: VirtualBox
1. Nouvelle VM → Linux → Other 64-bit
2. RAM: 512 MB
3. Monter `MyOS64.iso`
4. Démarrer

## 📁 Structure du Projet

```
MyOS64/
├── 📄 START_HERE.md              ← VOUS ÊTES ICI
├── 📄 GITHUB_SETUP.md            ← Guide détaillé GitHub
├── 📄 QUICKSTART.md              ← Guide de démarrage rapide
├── 📄 README.md                  ← Documentation principale
│
├── 🔧 push_to_github.bat         ← Script Windows pour GitHub
├── 🔧 push_to_github.sh          ← Script Linux/Mac pour GitHub
├── 🔧 build.sh                   ← Script de compilation local
├── 🔧 check_structure.sh         ← Vérification de la structure
│
├── 📦 .github/workflows/
│   └── build.yml                 ← Configuration GitHub Actions
│
├── 💾 boot/
│   └── boot.asm                  ← Bootloader 64-bit
│
├── 🖥️ kernel/
│   ├── kernel_entry.asm          ← Point d'entrée
│   ├── kernel.c                  ← Kernel principal
│   ├── terminal.c                ← Mode terminal
│   ├── graphics.c                ← Primitives graphiques
│   └── gui.c                     ← Interface Windows 11
│
├── ⚙️ linker.ld                  ← Script de liaison
├── ⚙️ grub.cfg                   ← Configuration GRUB
└── ⚙️ Makefile                   ← Système de build
```

## 🎯 Que faire ensuite ?

### Niveau 1: Découverte
- [ ] Pousser le code sur GitHub
- [ ] Voir la compilation automatique dans Actions
- [ ] Télécharger et tester l'ISO
- [ ] Explorer le mode terminal
- [ ] Passer en mode GUI

### Niveau 2: Personnalisation
- [ ] Changer les couleurs dans `kernel/gui.c`
- [ ] Ajouter une commande dans `kernel/kernel.c`
- [ ] Modifier le message de bienvenue
- [ ] Changer le logo dans la barre des tâches

### Niveau 3: Développement
- [ ] Implémenter une nouvelle fonctionnalité
- [ ] Ajouter une application en mode GUI
- [ ] Créer un système de fichiers simple
- [ ] Améliorer la gestion du clavier

## 📚 Documentation

| Fichier | Description |
|---------|-------------|
| `README.md` | Documentation technique complète |
| `QUICKSTART.md` | Guide de démarrage rapide |
| `GITHUB_SETUP.md` | Guide détaillé pour GitHub |
| `PROJECT_INFO.md` | Informations sur le projet |

## 🔧 Compilation Locale (Optionnel)

Si vous voulez compiler localement au lieu d'utiliser GitHub Actions :

**Linux/WSL** :
```bash
sudo apt-get update
sudo apt-get install -y nasm gcc make xorriso grub-pc-bin grub-common mtools
make
make run
```

**Windows** : Utilisez WSL2 ou Docker

## 💡 Astuces

### Token GitHub
Quand vous poussez vers GitHub, utilisez un **Personal Access Token** comme mot de passe :
1. GitHub.com → Settings → Developer settings
2. Personal access tokens → Tokens (classic)
3. Generate new token
4. Sélectionnez "repo"
5. Copiez le token et utilisez-le comme mot de passe

### Commandes Git Utiles
```bash
git status              # Voir l'état du repo
git add .               # Ajouter tous les changements
git commit -m "msg"     # Créer un commit
git push                # Pousser vers GitHub
git pull                # Récupérer depuis GitHub
```

### Commandes Terminal MyOS64
```
help     - Affiche l'aide
clear    - Efface l'écran
gui      - Passe en mode graphique
info     - Informations système
reboot   - Redémarre le système
```

### Raccourcis Mode GUI
```
ESC      - Retour au terminal
```

## ❓ Problèmes Courants

**"Git command not found"**
→ Installez Git : https://git-scm.com/

**"Permission denied"**
→ Utilisez un Personal Access Token, pas votre mot de passe

**"GitHub Actions failed"**
→ Vérifiez que tous les fichiers sont bien dans le repo

**"OS ne démarre pas"**
→ Vérifiez que la VM est en 64-bit avec 512 MB de RAM minimum

**"Écran noir après GRUB"**
→ Essayez VirtualBox au lieu de QEMU

## 🎉 C'est Parti !

Vous avez tout ce qu'il faut pour :
- ✅ Créer votre OS
- ✅ Le compiler sur GitHub
- ✅ Le tester
- ✅ Le personnaliser
- ✅ L'améliorer

**Prochaine étape** : Exécutez `push_to_github.bat` (Windows) ou `./push_to_github.sh` (Linux/Mac) !

---

**Besoin d'aide ?** Consultez les fichiers de documentation :
- Questions GitHub → `GITHUB_SETUP.md`
- Questions techniques → `README.md`
- Guide rapide → `QUICKSTART.md`

**Bon codage !** 🚀
