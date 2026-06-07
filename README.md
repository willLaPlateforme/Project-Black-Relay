[README(1).md](https://github.com/user-attachments/files/28684528/README.1.md)
# Project-Black-Relay# 🎮 BLACK RELAY — Tower Defense
### DEFEND // RESIST // SURVIVE

> Projet étudiant — Tower Defense en C++ / SFML 3  
> La Plateforme — Promo 2025

---

## 📋 Sommaire

1. [Présentation du jeu](#1-présentation-du-jeu)
2. [Captures d'écran](#2-captures-décran)
3. [Prérequis et installation](#3-prérequis-et-installation)
4. [Compilation et lancement](#4-compilation-et-lancement)
5. [Comment jouer](#5-comment-jouer)
6. [Architecture du projet](#6-architecture-du-projet)
7. [Structure des fichiers](#7-structure-des-fichiers)
8. [Systèmes de jeu](#8-systèmes-de-jeu)
9. [Assets requis](#9-assets-requis)
10. [Design Patterns utilisés](#10-design-patterns-utilisés)
11. [Équipe et répartition du travail](#11-équipe-et-répartition-du-travail)
12. [Pistes d'amélioration](#12-pistes-damélioration)

---

## 1. Présentation du jeu

**Black Relay** est un jeu de Tower Defense développé en C++ avec la bibliothèque graphique **SFML 3**. L'objectif est de défendre le **Black Relay** — une structure technologique vitale — contre des vagues d'ennemis de plus en plus redoutables.

Le joueur déploie des unités spécialisées (Sniper, Guardian, Elementalist, etc.) sur des emplacements prédéfinis, gère ses ressources (**Relay Shards**) et doit survivre aux 10 vagues du mode normal, ou tenir le plus longtemps possible en mode **Endless**.

### Caractéristiques principales

- **Menu complet** avec navigation clavier/souris, musique d'ambiance, options et tableau des scores
- **10 vagues** avec composition progressive (scouts → boss final)
- **4 difficultés** : Facile, Normal, Difficile, Endless
- **9 types de tours** avec animations sprites et 3 niveaux d'upgrade
- **6 types d'ennemis** avec comportements spécifiques (armure, AoE, soutien, boss)
- **Interface HUD** complète mise à jour en temps réel
- **Architecture MVC** avec patterns Observer et Factory

---

## 2. Captures d'écran

| Menu Principal | Écran de jeu |
|---|---|
| *Menu avec background cinématique* | *Map avec tours déployées et ennemis* |

---

## 3. Prérequis et installation

### Environnement requis

- **Windows** avec **MSYS2 (UCRT64)**
- **CMake** 3.16 ou supérieur
- **Make**

### Installation des dépendances via MSYS2

Ouvrir le terminal **MSYS2 UCRT64** et exécuter :

```bash
# Mise à jour du système
pacman -Syu

# SFML 3
pacman -S mingw-w64-ucrt-x86_64-sfml

# nlohmann_json (chargement des données JSON)
pacman -S mingw-w64-ucrt-x86_64-nlohmann-json

# CMake
pacman -S mingw-w64-ucrt-x86_64-cmake

# Make
pacman -S make
```

---

## 4. Compilation et lancement

### Étapes complètes

```bash
# 1. Se placer dans le dossier racine du projet
cd /c/Users/VotreNom/chemin/vers/BlackRelay

# 2. Créer le dossier de build
mkdir build
cd build

# 3. Configurer CMake
cmake .. -G "MSYS Makefiles"

# 4. Compiler
make

# 5. Lancer le jeu
./BlackRelay.exe
```

### Recompiler après modification d'un fichier source

```bash
cd build
make
```

> **Note :** Pas besoin de refaire `cmake ..` sauf si vous modifiez `CMakeLists.txt`.

### Copier les Assets manuellement (si nécessaire)

```bash
cp -r ../Assets ./Assets
```

> Le CMakeLists copie automatiquement le dossier `Assets/` dans `build/` après chaque compilation.

---

## 5. Comment jouer

### Navigation dans les menus

| Action | Touche / Souris |
|--------|-----------------|
| Naviguer dans le menu | ↑ ↓ ou Souris |
| Sélectionner | ENTRÉE ou Clic gauche |
| Quitter | QUITTER dans le menu ou fermer la fenêtre |

### Contrôles en jeu

| Action | Touche / Souris |
|--------|-----------------|
| Déployer une tour | Clic gauche sur un emplacement (+) |
| Améliorer une tour | Clic droit sur une tour déployée |
| Lancer la prochaine vague | ESPACE |
| Pause / Reprendre | P |
| Retour au menu | ESC |
| Son on/off | M |
| Sélectionner Sniper | 1 |
| Sélectionner Heavy Gunner | 2 |
| Sélectionner Elementalist | 3 |
| Sélectionner Vanguard | 4 |
| Sélectionner Guardian | 5 |
| Sélectionner Assault | 6 |

### Déroulement d'une partie

1. **Choisir la difficulté** dans les paramètres (menu → PARAMETRES)
2. **Lancer une partie** via NOUVELLE PARTIE
3. **Phase Farm (vagues 1-3)** : déployer jusqu'à 3 tours gratuitement
4. **À partir de la vague 4** : chaque tour coûte des Relay Shards
5. **Lancer les vagues** manuellement avec ESPACE ou attendre le timer automatique (10 sec)
6. **Améliorer** vos tours avec un clic droit (coût = prix de base de la tour)
7. **Survivre** jusqu'à la vague 10 pour la victoire (ou sans fin en mode Endless)

### Système de ressources

- **Relay Shards** : gagnés en tuant des ennemis (drop selon le type)
- **Phase Farm** : déploiement gratuit, maximum 3 tours simultanées
- **Après la phase Farm** : chaque tour a un coût défini dans `Assets/Data/units.json`
- **Upgrade** : coût = prix de base × niveau actuel

---

## 6. Architecture du projet

Le projet suit rigoureusement l'architecture **MVC (Modèle-Vue-Contrôleur)** complétée par les patterns **Observer** et **Factory**.

```
┌─────────────────────────────────────────────────────┐
│                      main.cpp                        │
│         (orchestre la transition menu ↔ jeu)         │
└──────────────┬───────────────────┬──────────────────┘
               │                   │
    ┌──────────▼──────────┐   ┌────▼───────────────────┐
    │   MENU (collègue)   │   │    JEU (Melvin)         │
    │                     │   │                         │
    │  Menu (logique)     │   │  Game (Modèle)          │
    │  Options            │   │  ├── WaveManager        │
    │  Scores             │   │  ├── EnemyFactory       │
    │                     │   │  └── TowerFactory       │
    │  Affichage_Menu     │   │                         │
    │  Affichage_Options  │   │  Controller             │
    └─────────────────────┘   │                         │
                              │  GameView (Observer)    │
                              └─────────────────────────┘
```

### Le Modèle (Game)

Contient toute la logique sans jamais rien afficher :
- État du jeu, score, ressources, HP du Black Relay
- Liste des tours et des ennemis actifs
- Chemin des ennemis (waypoints) et emplacements (Slots)
- Notification des observateurs via `IObservable`

### La Vue (GameView + Affichage_Menu)

- `GameView` implémente `IObserver` et reçoit les notifications du modèle
- Dessine la map, le HUD, les tours animées, les ennemis, les menus ingame
- `Affichage_Menu` gère le rendu complet du menu principal

### Le Contrôleur (Controller)

- Capture les événements SFML (clavier, souris, fermeture)
- Traduit les actions utilisateur en appels sur le modèle
- Ne contient aucune logique de jeu

### Pattern Observer

```cpp
// Le modèle notifie sans connaître la vue
notify("SCORE",     _score);
notify("RESOURCES", _resources);
notify("BASE_HP",   _baseHp);

// La vue reçoit et met à jour uniquement ce qui a changé
void GameView::onEvent(const std::string& event, int value) {
    if (event == "SCORE")     _scoreText->setString("Score: " + ...);
    if (event == "RESOURCES") _resourceText->setString("Shards: " + ...);
    // ...
}
```

### Pattern Factory

```cpp
// EnemyFactory crée l'ennemi approprié depuis son id
auto e = EnemyFactory::create("boss", waveNumber);

// TowerFactory crée la tour depuis son id JSON
auto t = TowerFactory::create("sniper", x, y);
```

---

## 7. Structure des fichiers

```
BlackRelay/
├── main.cpp                              ← Point d'entrée
├── CMakeLists.txt                        ← Configuration build CMake
│
├── Menu/                                 ← Logique des menus (collègue)
│   ├── Includes/
│   │   ├── Menu.hpp                      ← Logique navigation menu
│   │   ├── Options.hpp                   ← Logique paramètres
│   │   └── Scores.hpp                    ← Gestion tableau des scores
│   └── Sources/
│       ├── Menu.cpp
│       ├── Options.cpp
│       └── Scores.cpp
│
├── Affichage/
│   ├── Affichage_Menu/                   ← Vue du menu (collègue)
│   │   ├── Includes/affichage_menu.hpp
│   │   └── Sources/affichage_menu.cpp
│   ├── Affichage_Options/                ← Vue des options (collègue)
│   │   ├── Includes/Affichage_Options.hpp
│   │   └── Sources/Affichage_Options.cpp
│   └── Game/                             ← Vue du jeu (Melvin)
│       ├── Includes/Game.hpp             ← GameView : IObserver
│       └── Sources/Game.cpp
│
├── Game/
│   ├── Model/                            ← Modèle du jeu (Melvin)
│   │   ├── Includes/
│   │   │   ├── Game.hpp                  ← Game + WaveManager + Slot
│   │   │   └── Observer.hpp              ← IObserver + IObservable
│   │   ├── Sources/
│   │   │   └── Game.cpp
│   │   ├── Enemy/
│   │   │   ├── Includes/Ennemy.hpp       ← Enemy + sous-classes + EnemyFactory
│   │   │   └── Sources/Ennemy.cpp
│   │   └── Player_Tower/
│   │       ├── Includes/Player_Tower.hpp ← Player_Tower + TowerFactory
│   │       └── Sources/Player_Tower.cpp
│   └── Controller/                       ← Contrôleur (Melvin)
│       ├── Includes/Controller.hpp
│       └── Sources/Controller.cpp
│
└── Assets/
    ├── Backgrounds/
    │   ├── Menu_background.png           ← Background du menu principal
    │   └── map_bg.png                    ← Image de la map de jeu
    ├── Fonts/
    │   └── font.ttf                      ← Police principale
    ├── Ost/
    │   └── Menu/
    │       └── Black Relay(Menu Ost).mp3 ← Musique du menu
    ├── Data/
    │   ├── enemies.json                  ← Stats et données des ennemis
    │   └── units.json                    ← Stats et données des tours
    ├── Sprites/
    │   ├── sniper/                       ← idle_00.png walk_00.png death_00.png ...
    │   ├── vanguard/
    │   ├── guardian/
    │   ├── assault/
    │   ├── heavy_gunner/
    │   ├── medic/
    │   ├── elementalist/
    │   ├── infiltrator/
    │   └── berserker/
    └── Icones/
        └── Ico/
            └── Black Relay.png           ← Icône de la fenêtre
```

---

## 8. Systèmes de jeu

### Vagues d'ennemis

| Vague(s) | Composition |
|----------|-------------|
| 1-3 | **Phase Farm** : scouts uniquement. Déploiement gratuit, max 3 tours |
| 4 | Scouts + Infected |
| 5 | + Destroyers (armure 20%) |
| 6 | + Blighters (dégâts AoE) |
| 7 | + Overseers (soutien) |
| 8-9 | Composition maximale : tous les types |
| 10 | **Boss** final + escorte renforcée |
| Endless | Recyclage infini, scaling HP/DPS par vague |

### Types d'ennemis

| ID | Nom | Particularité |
|----|-----|---------------|
| `scout` | Scout | Rapide, peu résistant |
| `infected` | Infected | Équilibré, unité standard |
| `destroyer` | Destroyer | Armure 20% (dégâts réduits) |
| `blighter` | Blighter | Dégâts AoE sur les tours |
| `overseer` | Overseer | Soutien des alliés |
| `boss` | Boss | Armure 30%, très puissant |

### Types de tours

| ID | Portée | Type de dégâts |
|----|--------|----------------|
| `sniper` | Très longue | Single target |
| `vanguard` | Moyenne | Single target |
| `guardian` | Courte | Single target |
| `assault` | Moyenne | Single target |
| `heavy_gunner` | Moyenne | AoE |
| `medic` | Courte | Soin (support) |
| `elementalist` | Longue | AoE |
| `infiltrator` | Longue | Single target |
| `berserker` | Courte | Single target |

### Niveaux d'upgrade

Chaque tour peut être améliorée 2 fois (niveau 1 → 2 → 3) via clic droit :

| Niveau | DPS | Portée | Cadence de tir |
|--------|-----|--------|----------------|
| 1 | Base | Base | Base |
| 2 | ×1.5 | ×1.15 | ×1.2 |
| 3 | ×2.25 | ×1.32 | ×1.44 |

### Système de difficulté

Le multiplicateur est appliqué aux **HP, vitesse et DPS** de tous les ennemis au démarrage :

| Difficulté | Multiplicateur | Vagues |
|------------|---------------|--------|
| Facile | ×0.5 | 10 |
| Normal | ×1.0 | 10 |
| Difficile | ×1.2 | 10 |
| Endless | ×1.0 | Infini |

---

## 9. Assets requis

Tous les assets doivent être placés dans `Assets/` à la racine du projet. Le CMake les copie automatiquement dans `build/Assets/` après chaque compilation.

### Fichiers obligatoires

```
Assets/Fonts/font.ttf
Assets/Backgrounds/Menu_background.png
Assets/Backgrounds/map_bg.png
Assets/Ost/Menu/Black Relay(Menu Ost).mp3
Assets/Data/enemies.json
Assets/Data/units.json
```

### Sprites des tours (optionnel — fallback rectangle si absent)

Chaque tour possède 3 animations, nommées ainsi :

```
Assets/Sprites/{id}/idle_00.png
Assets/Sprites/{id}/idle_01.png
...
Assets/Sprites/{id}/walk_00.png
...
Assets/Sprites/{id}/death_00.png
...
```

### Format des fichiers JSON

#### enemies.json
```json
{
  "enemies": [
    {
      "id": "scout",
      "name": "Scout",
      "pv": 80,
      "vitesse": 2.5,
      "dps": 8,
      "drop": 10,
      "scaling_pv": 0.15,
      "type_degats": "single"
    }
  ]
}
```

#### units.json
```json
{
  "units": [
    {
      "id": "sniper",
      "name": "Sniper",
      "pv": 100,
      "shards": 60,
      "dps": 35,
      "portee": "tres_longue",
      "type_degats": "single",
      "upgrade1": "Portée +15%, DPS +50%",
      "upgrade2": "Tir critique",
      "unique": false
    }
  ]
}
```

---

## 10. Design Patterns utilisés

### MVC (Modèle-Vue-Contrôleur)

Séparation stricte des responsabilités :
- **Modèle** (`Game`) : logique pure, aucun affichage
- **Vue** (`GameView`, `Affichage_Menu`) : rendu pur, aucune logique
- **Contrôleur** (`Controller`) : traduction des événements, aucun affichage

### Observer

Découplage fort entre le modèle et la vue :
- `Game` hérite de `IObservable`
- `GameView` implémente `IObserver`
- Le modèle n'a aucune référence à la vue

### Factory

Création centralisée des objets :
- `EnemyFactory::create("scout", wave)` → instancie le bon sous-type
- `TowerFactory::create("sniper", x, y)` → instancie depuis le catalogue JSON
- Ajout d'un nouveau type = modification du JSON uniquement

---

## 11. Équipe et répartition du travail

| Composant | Développeur |
|-----------|-------------|
| `Game/Model/` (Game, WaveManager, Slot) | Melvin |
| `Game/Model/Enemy/` (Enemy, sous-classes, EnemyFactory) | Melvin |
| `Game/Model/Player_Tower/` (Player_Tower, TowerFactory) | Melvin |
| `Game/Controller/` | Melvin |
| `Affichage/Game/` (GameView) | Melvin |
| `Menu/` (Menu, Options, Scores) | Collègue |
| `Affichage/Affichage_Menu/` | Collègue |
| `Affichage/Affichage_Options/` | Collègue |
| `main.cpp` fusionné | Melvin + Collègue |
| `CMakeLists.txt` fusionné | Melvin + Collègue |

### Fusion des deux parties

Les deux modules ont été développés indépendamment puis fusionnés :
- Résolutions différentes harmonisées : **1280x720** menu / **1448x1030** jeu
- Rebind des références fenêtre lors de la transition
- Transmission de la difficulté et du son depuis le menu vers le modèle
- Ajout de `setDiffMult()` dans `EnemyFactory` pour appliquer la difficulté
- Correction du bug de hauteur du bouton Retour dans `Affichage_Options`

---

## 12. Pistes d'amélioration

- [ ] Sprites animés pour les ennemis (actuellement fallback cercles colorés)
- [ ] Musique et effets sonores en jeu (tirs, explosions, mort d'ennemi)
- [ ] Écrans OPERATEURS, ARSENAL, RECHERCHE, DOSSIER du menu
- [ ] Sauvegarde des scores en fichier (JSON ou CSV)
- [ ] Effets de particules (explosions, soins, auras)
- [ ] Sélection de map / écran de niveau
- [ ] Indicateur visuel lors d'un placement de tour réussi/refusé
- [ ] Informations détaillées des tours au survol (portée, DPS, coût upgrade)

---

*Black Relay — La Plateforme 2025*
