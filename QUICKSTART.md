# MyOS64 - Guide de Démarrage Rapide

## 🚀 Compilation sur GitHub

### Configuration automatique

1. **Créer un nouveau dépôt GitHub**
   - Allez sur github.com
   - Cliquez sur "New repository"
   - Nommez-le "MyOS64"
   - Initialisez-le (ou pas)

2. **Pousser le code**
   ```bash
   git init
   git add .
   git commit -m "Initial commit - MyOS64"
   git branch -M main
   git remote add origin https://github.com/VOTRE-USERNAME/MyOS64.git
   git push -u origin main
   ```

3. **GitHub Actions se lance automatiquement**
   - Allez dans l'onglet "Actions" de votre dépôt
   - La compilation démarre automatiquement
   - Attendez 2-3 minutes

4. **Télécharger l'ISO**
   - Dans "Actions", cliquez sur le workflow terminé
   - Téléchargez l'artifact "MyOS64-ISO"
   - Décompressez le fichier ZIP
   - Vous avez maintenant `MyOS64.iso` !

### Releases automatiques

À chaque push sur `main`, une nouvelle release est créée avec l'ISO attaché.

## 💻 Compilation Locale (Windows)

### Option 1: WSL2 (Recommandé)

```bash
# Dans WSL2 Ubuntu
sudo apt-get update
sudo apt-get install -y nasm gcc make xorriso grub-pc-bin grub-common mtools

# Compiler
cd MyOS64
make
```

### Option 2: Docker

```bash
# Créer un conteneur de build
docker run -it --rm -v ${PWD}:/work ubuntu:22.04 bash

# Dans le conteneur
apt-get update
apt-get install -y nasm gcc make xorriso grub-pc-bin grub-common mtools
cd /work
make
```

### Option 3: Cygwin ou MSYS2

Installer les outils nécessaires via Cygwin/MSYS2 puis compiler normalement.

## 🖥️ Tester l'OS

### Avec QEMU (Rapide)

```bash
# Linux/WSL2/Mac
qemu-system-x86_64 -cdrom MyOS64.iso -m 512M

# Windows
qemu-system-x86_64.exe -cdrom MyOS64.iso -m 512M
```

### Avec VirtualBox

1. Ouvrir VirtualBox
2. Nouvelle VM:
   - Type: Linux
   - Version: Other Linux (64-bit)
   - Mémoire: 512 MB minimum
3. Configuration → Stockage → Contrôleur IDE → Ajouter CD
4. Sélectionner `MyOS64.iso`
5. Démarrer !

### Avec VMware

1. Créer une nouvelle VM
2. Type: Other Linux 64-bit
3. Mémoire: 512 MB
4. Utiliser ISO: Sélectionner `MyOS64.iso`
5. Démarrer !

### Sur une clé USB (Matériel réel)

**⚠️ ATTENTION: Cela effacera la clé USB!**

```bash
# Linux
sudo dd if=MyOS64.iso of=/dev/sdX bs=4M status=progress
sudo sync

# Windows (utiliser Rufus)
# Télécharger Rufus, sélectionner l'ISO et la clé USB
```

## 🎮 Utilisation

### Mode Terminal

Au démarrage, vous êtes en mode terminal:

```
Commandes disponibles:
  help   - Afficher l'aide
  clear  - Effacer l'écran
  gui    - Passer en mode graphique
  info   - Informations système
  reboot - Redémarrer
```

### Mode Graphique

Tapez `gui` pour passer en mode graphique.
- Interface style Windows 11
- Barre des tâches
- Fenêtres
- Appuyez sur **ESC** pour retourner au terminal

## 📁 Structure du Projet

```
MyOS64/
├── .github/
│   └── workflows/
│       └── build.yml          # ← Configuration GitHub Actions
├── boot/
│   └── boot.asm               # Bootloader 64-bit
├── kernel/
│   ├── kernel_entry.asm       # Point d'entrée
│   ├── kernel.c               # Kernel principal
│   ├── terminal.c             # Terminal VGA
│   ├── graphics.c             # Graphiques
│   └── gui.c                  # Interface GUI
├── linker.ld                  # Script de liaison
├── grub.cfg                   # Config GRUB
├── Makefile                   # Build local
├── build.sh                   # Script de build
└── README.md
```

## 🔧 Dépendances

### Pour compiler:
- NASM (assembleur)
- GCC (compilateur C)
- LD (linker)
- GRUB tools (grub-mkrescue)
- xorriso (création ISO)

### Pour tester:
- QEMU (émulateur - recommandé)
- VirtualBox ou VMware (machines virtuelles)

## 🐛 Dépannage

### "GRUB not found"
```bash
sudo apt-get install grub-pc-bin grub-common
```

### "NASM not found"
```bash
sudo apt-get install nasm
```

### "GCC not found"
```bash
sudo apt-get install gcc build-essential
```

### L'OS ne démarre pas
- Vérifiez que la VM est en mode 64-bit
- Augmentez la RAM à 512MB minimum
- Vérifiez que la virtualisation est activée dans le BIOS

### Écran noir après GRUB
- Normal si pas de framebuffer dans QEMU
- Essayez avec VirtualBox/VMware
- Le mode terminal devrait toujours fonctionner

## 📚 Ressources

- [OSDev Wiki](https://wiki.osdev.org/)
- [Documentation GRUB](https://www.gnu.org/software/grub/manual/)
- [Intel x86-64 Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)

## 🎯 Prochaines Étapes

1. Implémenter un système de fichiers
2. Ajouter le support du réseau
3. Créer un gestionnaire de mémoire avancé
4. Implémenter le multitâche
5. Développer des applications utilisateur

## ❓ Questions Fréquentes

**Q: Puis-je utiliser cet OS en production?**
R: Non, c'est un projet éducatif. Ne l'utilisez pas pour des tâches critiques.

**Q: Sur quel matériel puis-je le tester?**
R: N'importe quel PC 64-bit, mais il est recommandé de tester d'abord dans une VM.

**Q: Puis-je contribuer?**
R: Absolument! Forkez le projet et créez une pull request.

**Q: Ça fonctionne sur ARM?**
R: Non, uniquement x86-64 pour le moment.

---

🎉 **Bon coding!**
