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
 * @file machine_virtuelle.h
 * @brief constant and types used by virtual machine
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

// Contient toutes les types et constantes liees a la machine virtuelle mais detachees du mode d'affichage (console / graphique)
// Egalement les prototypes des fonctions de machine_virtuelle.c 

// TODO : L'implementation des own est pas intelligence : on trouvera une astuce pour meler le num du programme et le type (pair / impair ?)

#ifndef MACHINE_VIRTUELLE_H
#define MACHINE_VIRTUELLE_H

#include"../general.h"


// Nombre maximal de combattant accepte dans un tournoi
#define MAX_COMBATTANTS 16

// Nombre de round par combat
#define NB_ROUND 1000

// Nombre d'executions total pour chacun des deux programmes avant la fin du round considere comme nul
#define LIMIT_CYCLE 8000


// Modulo taille de la memoire
#define MOD_TAILLE_MEM &0xFFF //anciennement '% TAILLE_MEM', '&0xFFF' est plus efficace du fait que TAILLE_MEM = 2^12


// Temps d'affichage
#define DEFAUT_waitingtime 500000 // 0.5 s
#define MAX_waitingtime 2000000 // 2 s
#define MIN_waitingtime 50000 // 0.05 s
#define INCR_waitingtime 50000 // 0.05 s

#define TIME_END_MATCH 5000000 // 5 s : temps durant l'affichage du resultat a la fin d'un round

#define NB_CLIGN 6 // nombre de clignotement du resultat durant TIME_END_MATCH sec


#define QUITTER_AFFICHAGE -20 // Pour signaler la volonter de quitter le programme



typedef unsigned long temps;


struct combattant {
	sequence seq;
	
	int num;
	int point; // Nombre de points
	int place; // Place actuelle dans le classement
	char * nom; // Nom de la sequence
};
typedef struct combattant combattant;

enum type_own {OWNER=0, OWNER_BOMB=2, OWNER_VAR=4};

enum own {OWNER_CASE_VIDE=-1, 
OWNER_P0=0, OWNER_P1=1, 
OWNER_P0_BOMB=2, OWNER_P1_BOMB, 
OWNER_P0_VAR=4, OWNER_P1_VAR,
OWNER_P0_KILL=6, OWNER_P1_KILL,
}; // version detaillee

enum combat_mode {MODE_NORMAL =1, MODE_MAXSPEED_WITHDISPLAY, MODE_MAXSPEED_NODISPLAY};
// MODE_MAXSPEED_NODISPLAY : il n'y a pas de temps d'attente entre chaque action ni d'affichage (de la zone memoire)
// MODE_MAXSPEED_WITHDISPLAY : il n'y a pas de temps d'attente entre chaque action mais tous de meme un affichage (de la zone memoire)


struct environnement {
	/*Environnement pour un match*/
	
	/* memoire */
	int mem[TAILLE_MEM]; // contenu de la zone memoire
	int ip[2]; // instruction pointee par les deux programmes dans la memoire -> mem[ip] est la prochaine instruction a executer
	
	/* statistiques pour le combat actuel entre deux programmes */
	enum own mem_owner[TAILLE_MEM]; // numero du programme proprietaire (plus d'infos voir enum own)
	int nb_case_owned[2]; // nombre de case conquis pour ne pas avoir a le recalculer
	int nb_gagne[2];
	int nb_match_nul;
	int num_round_actuel; // De 1 a NB_ROUND compris
	char * nom[2]; // Nom des deux programmes -> un raccourci vers le nom de sequence, ne me libere pas !
	//int num_case_nouvel_own; // Numero d'une case (en plus de l'ip actuelle) dont la possession a ete modifie.
	
	
	int current_step; // copie du compteur du nb d'instructruction depuis le debut pour l'affichage (qui se stoppe a LIMIT_CYCLE)
	
	
	// Tableau pour savoir si on doit rebuff ou pas une case de la memoire -> permet d'optimiser le temps de buff graphique
	bool need_rebuff[TAILLE_MEM];
	
	
	/* Options et parametres globaux modifiables au fur et a mesure */
	
	enum combat_mode mode; // mode d'affichage graphique
	int waitingtime; // Temps d'attente entre chaque action (cela prends en compte le temps de calcul et d'affichage)
	
	#ifdef DEBUG
	// utilise pour le debug
	temps dernier_rafraichissement; // Uniquement pour calculer le fps
	#endif
};

/* Variable globale d'environnement :
 * Contient la memoire simulee, l'ip des deux programmes, ainsi que les statistiques globales et les options en temps reels */
extern struct environnement env;





char * prog_own_stats2string (int nb_case_owned, int total_case);


#ifdef DEBUG
void DEBUG_own (void);
void DEBUG_prog_ip (void);
#endif

#endif
