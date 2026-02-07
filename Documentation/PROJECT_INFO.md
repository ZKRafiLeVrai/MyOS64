# MyOS64 - Système d'Exploitation 64-bit Complet

[![Build Status](https://github.com/VOTRE-USERNAME/MyOS64/workflows/Build%20MyOS64/badge.svg)](https://github.com/VOTRE-USERNAME/MyOS64/actions)
[![License](https://img.shields.io/badge/license-Educational-blue.svg)](LICENSE)

## 🎯 Aperçu

MyOS64 est un système d'exploitation complet 64-bit construit de zéro, incluant :

- ✅ **Bootloader GRUB** - Démarrage fiable
- ✅ **Mode Terminal** - Ligne de commande interactive
- ✅ **Interface Graphique** - Style Windows 11 moderne
- ✅ **Architecture x86-64** - Support processeurs 64-bit
- ✅ **Compilation GitHub Actions** - Build automatique dans le cloud

## 📸 Captures d'écran

### Mode Terminal
```
====================================
     Bienvenue dans MyOS64!        
====================================

Système d'exploitation 64-bit
Version 1.0.0

Tapez 'help' pour voir les commandes disponibles.
Tapez 'gui' pour passer en mode graphique.

MyOS64> _
```

### Mode Graphique
Interface moderne style Windows 11 avec :
- Barre des tâches
- Bouton démarrer
- Fenêtres avec décorations
- Système de couleurs moderne

## 🚀 Démarrage Rapide

### Option 1: Compilation sur GitHub (Recommandé pour Windows)

1. **Forkez ou clonez ce dépôt**
2. **Poussez sur GitHub** - La compilation démarre automatiquement
3. **Téléchargez l'ISO** depuis l'onglet Actions ou Releases
4. **Testez** avec QEMU, VirtualBox, ou VMware

### Option 2: Compilation Locale (Linux/WSL)

```bash
# Installer les dépendances
sudo apt-get update
sudo apt-get install -y nasm gcc make xorriso grub-pc-bin grub-common mtools

# Compiler
make

# Tester avec QEMU
make run
```

## 📦 Contenu du Projet

```
MyOS64/
├── .github/workflows/build.yml    # GitHub Actions configuration
├── boot/boot.asm                  # Bootloader 64-bit
├── kernel/
│   ├── kernel_entry.asm           # Kernel entry point
│   ├── kernel.c                   # Main kernel
│   ├── terminal.c                 # VGA text mode
│   ├── graphics.c                 # Graphics primitives
│   └── gui.c                      # Windows 11 style GUI
├── linker.ld                      # Linker script
├── grub.cfg                       # GRUB configuration
├── Makefile                       # Local build system
└── README.md                      # This file
```

## 💻 Fonctionnalités

### Mode Terminal
- Ligne de commande interactive
- Commandes système : `help`, `clear`, `gui`, `info`, `reboot`
- Support du clavier complet
- Gestion de l'historique de commandes

### Mode Graphique
- Interface utilisateur moderne
- Barre des tâches style Windows 11
- Fenêtres avec décorations
- Système de couleurs cohérent
- Transitions entre modes

### Architecture
- Mode long 64-bit (x86-64)
- Pagination complète
- GDT (Global Descriptor Table)
- Gestion du clavier
- Framebuffer graphique

## 🎮 Utilisation

Au démarrage, vous verrez le menu GRUB. Sélectionnez "MyOS64" pour démarrer.

### Commandes Terminal

```bash
MyOS64> help              # Affiche l'aide
MyOS64> clear             # Efface l'écran
MyOS64> gui               # Passe en mode graphique
MyOS64> info              # Informations système
MyOS64> reboot            # Redémarre le système
```

### Mode GUI

- Tapez `gui` dans le terminal pour passer en mode graphique
- Appuyez sur **ESC** pour retourner au terminal
- Interface complète avec fenêtres et barre des tâches

## 🔧 Configuration GitHub Actions

Le fichier `.github/workflows/build.yml` configure la compilation automatique :

- ✅ Build sur chaque push
- ✅ Création d'artifacts (ISO téléchargeable)
- ✅ Releases automatiques avec tags
- ✅ Support Ubuntu latest

## 🧪 Tests

### QEMU (Émulateur - Recommandé)
```bash
qemu-system-x86_64 -cdrom MyOS64.iso -m 512M
```

### VirtualBox
1. Nouvelle VM → Type: Linux, Version: Other 64-bit
2. RAM: 512 MB minimum
3. Monter MyOS64.iso comme CD
4. Démarrer

### VMware
1. Nouvelle VM → Linux 64-bit
2. RAM: 512 MB
3. Utiliser MyOS64.iso
4. Démarrer

### USB Bootable (Matériel réel)
```bash
# Linux
sudo dd if=MyOS64.iso of=/dev/sdX bs=4M status=progress

# Windows - Utiliser Rufus
```

## 📚 Détails Techniques

### Bootloader (boot.asm)
- Vérification du support 64-bit (CPUID)
- Activation de la ligne A20
- Configuration de la pagination (PML4, PDP, PD)
- Transition vers le mode long

### Kernel (kernel.c)
- Point d'entrée principal
- Boucle d'événements
- Gestion du clavier
- Commandes système
- Basculement entre modes

### Terminal (terminal.c)
- Mode texte VGA 80x25
- Buffer vidéo en 0xB8000
- Support du défilement
- Gestion des couleurs

### Graphics (graphics.c)
- Dessin de pixels
- Primitives : rectangles, cercles, lignes
- Framebuffer VESA
- Résolution 1024x768

### GUI (gui.c)
- Interface Windows 11
- Barre des tâches
- Fenêtres avec décorations
- Police bitmap 8x8
- Gestion des couleurs

## 🛠️ Développement Futur

- [ ] Système de fichiers (FAT32/ext2)
- [ ] Gestionnaire de mémoire avancé (heap, malloc)
- [ ] Multitâche préemptif
- [ ] Support du réseau (TCP/IP)
- [ ] Pilotes de périphériques (USB, SATA)
- [ ] Shell amélioré avec pipes
- [ ] Applications utilisateur
- [ ] Support multiprocesseur (SMP)

## 🤝 Contribution

Les contributions sont les bienvenues !

1. Forkez le projet
2. Créez une branche (`git checkout -b feature/AmazingFeature`)
3. Committez vos changements (`git commit -m 'Add AmazingFeature'`)
4. Poussez vers la branche (`git push origin feature/AmazingFeature`)
5. Ouvrez une Pull Request

## 📖 Ressources

- [OSDev Wiki](https://wiki.osdev.org/) - Ressource principale pour le dev OS
- [Intel Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html) - Documentation x86-64
- [GRUB Manual](https://www.gnu.org/software/grub/manual/) - Documentation GRUB
- [NASM Manual](https://www.nasm.us/doc/) - Documentation assembleur

## 📝 License

Ce projet est un projet éducatif open-source. Libre d'utilisation pour l'apprentissage.

## 👤 Auteur

Créé comme démonstration d'un OS 64-bit complet avec interface graphique moderne.

## ⭐ Support

Si ce projet vous aide, mettez une étoile ⭐ !

---

**Note**: Ceci est un projet éducatif. Ne l'utilisez pas en production.
