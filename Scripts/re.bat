@echo off
echo Reorganisation des fichiers...

REM Creer les dossiers
mkdir boot
mkdir kernel

REM Deplacer boot.asm
move boot.asm boot\

REM Deplacer les fichiers kernel
move kernel_entry.asm kernel\
move kernel.c kernel\
move terminal.c kernel\
move graphics.c kernel\
move gui.c kernel\

echo.
echo [OK] Fichiers reorganises !
echo.
pause