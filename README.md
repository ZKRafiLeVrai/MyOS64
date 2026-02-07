# MyOS64 - Système d'exploitation 64-bit

## Description

MyOS64 est un système d'exploitation 64-bit avec :
- **Bootloader GRUB** pour le démarrage
- **Mode terminal** avec ligne de commande interactive
- **Mode graphique** avec interface style Windows 11
- **Architecture x86-64** complète

## Fonctionnalités

### Mode Terminal
- Ligne de commande interactive
- Commandes disponibles :
  - `help` - Afficher l'aide
  - `clear` - Effacer l'écran
  - `gui` - Passer en mode graphique
  - `info` - Informations système
  - `reboot` - Redémarrer le système

### Mode Graphique
- Interface utilisateur style Windows 11
- Barre des tâches avec bouton démarrer
- Fenêtres avec bordures arrondies
- Système de couleurs moderne
- Appuyez sur ESC pour retourner au terminal

## Compilation

### Sur Linux

```bash
# Installer les dépendances
sudo apt-get install nasm gcc make xorriso grub-pc-bin grub-common mtools

# Compiler
make
```

### Sur GitHub Actions

Le projet se compile automatiquement via GitHub Actions :
1. Poussez votre code sur GitHub
2. GitHub Actions compilera automatiquement l'OS
3. Un fichier ISO sera créé en tant qu'artifact
4. L'ISO sera également attaché aux releases

## Structure du projet

```
MyOS64/
├── .github/
│   └── workflows/
│       └── build.yml          # Configuration GitHub Actions
├── boot/
│   └── boot.asm               # Bootloader 64-bit
├── kernel/
│   ├── kernel_entry.asm       # Point d'entrée du kernel
│   ├── kernel.c               # Kernel principal
│   ├── terminal.c             # Mode terminal VGA
│   ├── graphics.c             # Routines graphiques
│   └── gui.c                  # Interface graphique
├── linker.ld                  # Script de liaison
├── grub.cfg                   # Configuration GRUB
├── Makefile                   # Script de compilation
└── README.md                  # Ce fichier
```

## Tester l'OS

### Avec QEMU

```bash
qemu-system-x86_64 -cdrom MyOS64.iso -m 512M
```

### Avec VirtualBox

1. Créer une nouvelle VM 64-bit
2. Monter MyOS64.iso comme CD-ROM
3. Démarrer la VM

### Avec VMware

1. Créer une nouvelle VM 64-bit
2. Utiliser MyOS64.iso comme image de démarrage
3. Démarrer la VM

## Architecture

### Bootloader (boot.asm)
- Détection du support 64-bit
- Activation de la ligne A20
- Configuration de la pagination
- Passage en mode long (64-bit)

### Kernel (kernel.c)
- Gestion du terminal
- Gestion du clavier
- Commandes système
- Basculement terminal ↔ GUI

### Terminal (terminal.c)
- Mode texte VGA 80x25
- Support du défilement
- Gestion des couleurs
- Support backspace, tab, etc.

### Graphics (graphics.c)
- Dessin de pixels
- Rectangles et cercles
- Gestion du framebuffer
- Primitives graphiques

### GUI (gui.c)
- Interface Windows 11
- Barre des tâches
- Fenêtres avec décorations
- Système de couleurs moderne

## Développement futur

- [ ] Système de fichiers (FAT32)
- [ ] Gestionnaire de mémoire avancé
- [ ] Multi-threading
- [ ] Pilotes de périphériques
- [ ] Support réseau
- [ ] Applications utilisateur
- [ ] Shell amélioré
- [ ] Support USB

## Licence

Projet éducatif open-source.

## Auteur

Créé comme démonstration d'un OS 64-bit moderne.
