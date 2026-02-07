@echo off
REM Script pour pousser MyOS64 sur GitHub depuis Windows
REM Assurez-vous d'avoir Git installé : https://git-scm.com/download/win

echo ========================================
echo    MyOS64 - GitHub Upload Script
echo ========================================
echo.

REM Vérifier si Git est installé
where git >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [ERREUR] Git n'est pas installe ou n'est pas dans le PATH
    echo Telechargez Git depuis : https://git-scm.com/download/win
    pause
    exit /b 1
)

echo [OK] Git est installe
echo.

REM Demander le nom d'utilisateur GitHub
set /p GITHUB_USER="Entrez votre nom d'utilisateur GitHub: "

if "%GITHUB_USER%"=="" (
    echo [ERREUR] Le nom d'utilisateur ne peut pas etre vide
    pause
    exit /b 1
)

echo.
echo Configuration du repository...
echo Repository URL: https://github.com/%GITHUB_USER%/MyOS64.git
echo.

REM Initialiser Git si nécessaire
if not exist ".git" (
    echo Initialisation du depot Git...
    git init
    echo [OK] Depot Git initialise
    echo.
)

REM Configurer le remote
echo Configuration du remote GitHub...
git remote remove origin >nul 2>nul
git remote add origin https://github.com/%GITHUB_USER%/MyOS64.git
echo [OK] Remote configure
echo.

REM Ajouter tous les fichiers
echo Ajout des fichiers...
git add .
echo [OK] Fichiers ajoutes
echo.

REM Créer le commit
echo Creation du commit...
git commit -m "Initial commit: MyOS64 - Systeme d'exploitation 64-bit complet"
if %ERRORLEVEL% NEQ 0 (
    echo [INFO] Pas de nouveaux changements a committer ou commit deja existant
)
echo.

REM Renommer la branche en main
echo Renommage de la branche en 'main'...
git branch -M main
echo [OK] Branche renommee
echo.

REM Pousser vers GitHub
echo ========================================
echo Push vers GitHub...
echo ========================================
echo.
echo IMPORTANT: Utilisez votre Personal Access Token comme mot de passe
echo (PAS votre mot de passe GitHub)
echo.
echo Pour creer un token:
echo 1. Allez sur GitHub.com
echo 2. Settings ^> Developer settings ^> Personal access tokens ^> Tokens (classic)
echo 3. Generate new token
echo 4. Selectionnez 'repo' scope
echo 5. Copiez le token genere
echo.
pause

git push -u origin main

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo [SUCCES] Code pousse sur GitHub !
    echo ========================================
    echo.
    echo Prochaines etapes:
    echo 1. Allez sur https://github.com/%GITHUB_USER%/MyOS64
    echo 2. Cliquez sur l'onglet 'Actions'
    echo 3. Attendez que la compilation se termine (2-3 minutes)
    echo 4. Telechargez l'ISO depuis 'Actions' ou 'Releases'
    echo.
    echo Repository: https://github.com/%GITHUB_USER%/MyOS64
    echo.
) else (
    echo.
    echo ========================================
    echo [ERREUR] Echec du push
    echo ========================================
    echo.
    echo Verifiez:
    echo - Que le repository existe sur GitHub
    echo - Que vous avez utilise le bon username
    echo - Que vous avez utilise un Personal Access Token
    echo.
)

pause
