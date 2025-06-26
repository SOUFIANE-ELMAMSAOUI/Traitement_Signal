# 🎹 Synthétiseur Numérique Temps Réel - STM32F746
- Un synthétiseur numérique avancé implémenté sur microcontrôleur STM32F746 avec enveloppe ADSR, reverb algorithmique et contrôles MIDI en temps réel.


## 🎯 Vue d'ensemble
Ce projet implémente un synthétiseur numérique professionnel sur la carte STM32F746G-DISCO, offrant :

- Synthèse audio temps réel à 44.1 kHz/16-bit
- Enveloppe ADSR complète avec contrôles MIDI
- Reverb algorithmique basé sur feedback delay network
- Filtrage FIR/IIR adaptatif avec contrôle fréquentiel
- Gestion polyphonique intelligente (note en attente)
- Interface MIDI USB native

## 🎮 Démonstration Rapide
- 🎹 Jouez une note → 🎵 Signal carré → 🔊 Filtre FIR → 📈 ADSR → 🌊 Reverb → 🔈 Sortie

## ✨ Fonctionnalités
### 🎼 Moteur Audio

- Oscillateur : Signal carré avec lookup table optimisée
- Filtrage : FIR/IIR passe-bas variable (ARM DSP Library)
- Fréquence : Table de 200 notes (C1 à C8+)
- Latence : < 23 µs par échantillon (contraintes temps réel)

### 📈 Enveloppe ADSR

- Attack : 100ms à 5000ms (contrôlable)
- Decay : 100ms à 5000ms (contrôlable)
- Sustain : 0% à 100% (contrôlable)
- Release : 100ms à 5000ms (contrôlable)
- Transitions : Automatiques et fluides

### 🌊 Reverb Algorithmique

- Algorithme : Feedback Delay Network
- Délai : 54ms (2400 échantillons à 44.1kHz)
- Équation : v(n) = u(n-2400)
- Paramètres : Gain feedback + Mix delay
- Stabilité : Anti-saturation intégré

### 🎛️ Gestion Notes Multiples

- Monophonique avec file d'attente intelligente
- Note active + Note en attente
- Transitions fluides sans clicks

## 🏗️ Architecture
### 🧩 Modules Principaux
#### main.c - Contrôleur principal

- Callback audio temps réel (44.1 kHz)
- Gestion MIDI USB (Note On/Off, Control Change, Pitchbend)
- Orchestration des modules
- Debug visuel LED

#### adsr.c/h - Enveloppe sonore

- Machine d'état 6 états (INIT, ATTACK, DECAY, SUSTAIN, RELEASE, NOTE_OFF)
- Calcul incréments temps réel
- Contrôles MIDI CC1-4

#### reverb.c/h - Effet spatial

- Buffer circulaire 2400 échantillons
- Algorithme feedback delay
- Contrôles CC1 (durée) + Pitchbend (gain)

#### Modules support

- notes.h : Table fréquences 200 notes
- signalTables.h : Lookup tables formes d'onde
- FIR_filter.h : Filtrage adaptatif ARM DSP




