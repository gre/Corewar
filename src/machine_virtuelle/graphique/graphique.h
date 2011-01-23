/*
 *   this file is part of CoreWar project, a fight programming game
 *   Copyright (C) 2009 Gaetan Renaudeau < contact@grenlibre.fr >
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *   
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file graphique.h
 * @brief graphic mode
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

#ifndef GRAPHIQUE_H
#define GRAPHIQUE_H

#include"../machine_virtuelle.h"
#include"../listes.h"


/// Constantes externe pour le graphique

#define PROBLEME_AFFICHAGE -10 // Pour signaler un probleme d'affichage



/// Types externes

enum element_nom { EL_CHIFFRES_BLEU, EL_CHIFFRES_ROUGE, EL_CHIFFRES, 
EL_CASE_P0_BOMB, EL_CASE_P1_BOMB, EL_CASE_P0_VAR, EL_CASE_P1_VAR,  EL_CASE_P0_KILL, EL_CASE_P1_KILL, 
EL_VS,EL_MODE1, EL_MODE2, EL_MODE3,
EL_FLECHE_G, EL_FLECHE_D, EL_FLECHE_H, EL_FLECHE_B,
EL_LOGO, EL_RANK1, EL_RANK2, EL_RANK3,
EL_IND_G, EL_IND_D, 
MAX_ELEMENT};
typedef enum element_nom element_nom;

enum couleur_role {CLR_BACKGROUND, CLR_TEXTE_DEFAUT, CLR_TEXTE_INFO, CLR_PROG0, CLR_PROG1, 
CLR_CASE_VIDE, CLR_CASE_VIDE_CONTOUR, CLR_CASE_P0, CLR_CASE_P0_CONTOUR, CLR_CASE_P1, CLR_CASE_P1_CONTOUR, 
CLR_CASE_IP0, CLR_CASE_IP1, CLR_TEXTE_INACTIF, CLR_STEPBAR, CLR_STEPBAR_BACK, 
CLR_LISTE_COMBATTANT_BACK, CLR_LISTE_COMBATTANT_CONTOUR, CLR_TEXTE_COMBATTANT,
MAX_COULEUR};
typedef enum couleur_role couleur_role;


/// Prototypes de fonctions externes

int affichage_initialiser (void);
void affichage_fermer (void);

int affichage_avant_tournoi (combattant * cbt, int nb_combattants);
int affichage_apres_tournoi (combattant * cbt, int nb_combattants);

int affichage_avant_combat (liste_combat l, combattant * cbt, int nb_combattants);
int affichage_apres_combat (liste_combat l);

int affichage_avant_round(void);
int affichage_apres_round(int gagnant);

int affichage_avant_execution_instruction (int num_programme);
int affichage_apres_execution_instruction (int num_programme);

#endif
