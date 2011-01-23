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
 * @file graphique.c
 * @brief graphic mode
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

// Contient toutes les fonctions graphiques pour l'affichage de la machine virtuelle avec la bibliotheque graph.h

#include<stdio.h> 
#include<stdlib.h>
#include<unistd.h>

#include<string.h>

#include"../../general.h"
#include"../machine_virtuelle.h"
#include"../listes.h"
#include"element.h"

#include"../../graph.h"
#include"graphique.h"

#include"../../desassembleur/desassembleur.h"




/// Les variables externes composent l'environnement graphique
/* Elles sont utilisees ici pour ne pas les passer en arguments a des fonctions d'affichages. 
 * Ainsi, on renforce la barriere d'abstraction 
 * (c'est surement sur le meme principe qu'il existe des variables globales dans graph.c) 
 * Neanmoins, elle sont locales dans graphique.c */

// la liste est modifie uniquement a l'initialisation (affichage_initialiser) et libere a la fin (affichage_fermer)
liste_element liste_el; 

// Un tableau d'element permettant de retrouver l'id d'un element (modifie a l'initialisation)
int element[MAX_ELEMENT]; 

// Un tableau de couleur appele palette (modifie a l'initialisation)
couleur palette[MAX_COULEUR]; // est lie a couleur_role


/// Macros : pour eviter de des petites fonctions de ces outils tres utilises ici (but: augmenter le performance)

// renvoi le type couleur correspondant a la couleur passe en hexa
#define hexa2couleur(hexa) CouleurParComposante((hexa>>16)&0xFF,(hexa>>8)&0xFF,hexa&0xFF)

// Obtenir le temps depuis le lancement du programme
#define get_temps() Microsecondes()

// Attends un certain temps
#define delay(t) usleep((useconds_t)t)

// copie la zone de buffer sur la zone d'ecran 
#define flip() CopierZone(SCR_BUFF,SCREEN,0,0,W_FENETRE,H_FENETRE,0,0) 

// Version rapide
#define buff_zone_mem()	buff_zone_memoire(DEFAUT_ZONE_MEMOIRE_X,DEFAUT_ZONE_MEMOIRE_Y,DEFAUT_ZONE_MEMOIRE_W,DEFAUT_ZONE_MEMOIRE_CASE_W,DEFAUT_ZONE_MEMOIRE_CASE_H);



/// Types internes

enum bouton {BTN_AUCUN=0, BTN_G, BTN_D, BTN_H, BTN_B };
typedef enum bouton bouton;


/// Constantes internes pour le graphique

#define W_FENETRE 850 // Largeur de la fenetre
#define H_FENETRE 700 // Hauteur de la fenetre


// Pixmap utilises
#define SCREEN 0
#define SCR_BUFF 1 	// brouillon qui sert a rafraichir (double buffering)
#define SCR_ELEMENT 2 // brouillon ou les elements sont charges
#define SCR_SAVE 3 // sauvegarde d'eventuel morceau de buff pour ne pas a avoir a recalculer ces morceaux

// en microsecondes
//#define REFRESH_TIME 10000 // -> 100 Hz
#define REFRESH_TIME 13333 // -> 75 Hz
//#define REFRESH_TIME 16666 // -> 60 Hz


#define DEFAUT_ZONE_MEMOIRE_W 768
#define DEFAUT_ZONE_MEMOIRE_X (W_FENETRE-DEFAUT_ZONE_MEMOIRE_W)/2
#define DEFAUT_ZONE_MEMOIRE_Y 100
#define DEFAUT_ZONE_MEMOIRE_CASE_W 12
#define DEFAUT_ZONE_MEMOIRE_CASE_H 8



/// Prototypes des fonctions internes

// Evenements claviers
void action (bouton btn);
int actions_sur_pression_touche_combat (void);
int pressanykey (void);

// Fonctions de gestions des images
bool superpose_avec_autre_element (rectangle rect);
int affecter_emplacement_element (int * x, int * y, int w, int h);

// Retourne l'id de l'element associe (0 si erreur)
int charger_image_en_element (char * file, int xx, int yy, int w, int h);
int copier_element_partiel (element_nom nom_el, short x_el, short y_el, short w_el, short h_el, short x, short y, int num_ecran);
int copier_element (element_nom nom_el, short x, short y, int num_ecran);

// FONCTIONS buff_*
void buff_waitingtime (void);
void buff_changemode (void);
void buff_instruction_prog (int num_programme);
void buff_instruction_selection (int num_programme);
void buff_stat_prog (int num_programme);
void buff_scores (void);
void buff_combat_new (void);
void buff_liste_combattants (combattant * cbt, int nb_combattants, int x, int y, int w, int h);
void buff_classement_combattants (combattant * cbt, int nb_combattants, int x, int y, int w, int h);
void buff_top5_combattants(combattant * cbt, int nb_combattants, int x, int y, int w, int h);
void buff_liste_combat (liste_combat l, int x, int y, int w, int h);
void buff_combat_text_info (char * texte, couleur_role clr_texte);
void buff_ecrire_texte (char * texte, int x, int y, int taille, couleur_role c);
void buff_ecrire_texte_xaligncenter (char * texte, int y, int taille, couleur_role c);
void buff_ecrire_nombre (int nombre, int nb_chiffre, short x, short y, element_nom el_chiffre);
void buff_stepbar (int step, int total, couleur_role c);
void buff_case_ip (int x, int y, int w, int h, couleur_role rc);
void buff_case_memoire (int x, int y, int w, int h, int mem_owner);
void buff_zone_memoire (int x_debut, int y_debut, int w, int w_unicase, int h_unicase);

// buff_* d'ordre plus general :
int buff_element (element_nom nom_el, short x, short y);
void buff_rect (int x, int y, int w, int h, couleur_role c);
void buff_segment (int x, int y, int xx, int yy, couleur_role c);
void buff_save_zone (int x, int y, int w, int h, int x_buff, int y_buff);
void save_buff_zone (int x, int y, int w, int h, int x_save, int y_save);

// Autres
char * waitingtime2string (void);
char * info_fin_round2string (int gagnant);

#ifdef DEBUG
// pour le debug / stats de performance
void update_dernier_rafraichissement (void);
temps retourner_temps_passe (void);
float retourner_fps (void);
void DEBUG_temps_perdu (void);
#endif



/// Fonctions d'affichages directement utilisables depuis l'exterieur (externe)

// Initialiser tout l'environnement graphique
int affichage_initialiser (void)
{
	int i;
	
	if( InitialiserGraphique() )
		return PROBLEME_AFFICHAGE;
	if( CreerFenetre( (Maxx()-W_FENETRE)/2, (Maxy()-W_FENETRE)/2, W_FENETRE, H_FENETRE ) )
		return PROBLEME_AFFICHAGE;
	
	EcrireTexte(300,H_FENETRE/2,"Chargement en cours...",2);
	
	liste_el=NULL;
	
	for(i=0;i<MAX_ELEMENT;i++)
		element[i] = 0;
	
	for(i=0;i<MAX_COULEUR;i++)
		palette[i] = 0;

	
	// On charge toutes les images
	
	/* mode d'emploi: 
	 * - On ajoute le NOM_DE_MON_ELEMENT dans le 'enum element_nom' de graphique.h
	 * - on charge l'image avec cette commande :
	 * element[NOM_DE_MON_ELEMENT] = charger_image_en_element ("LIEN_VERS_MON_ELEMENT",X_sur_l_image,Y_sur_l_image,LARGEUR,HAUTEUR);
	 * - A l'avenir nous utiliserons NOM_DE_MON_ELEMENT pour utiliser notre element
	 */
	
	
	

	element[EL_CASE_P0_BOMB] = charger_image_en_element ("gfx/mine_bleu.xpm",0,0,12,8);
	element[EL_CASE_P1_BOMB] = charger_image_en_element ("gfx/mine_rouge.xpm",0,0,12,8);
	element[EL_CASE_P0_VAR] = charger_image_en_element ("gfx/var_bleu.xpm",0,0,12,8);
	element[EL_CASE_P1_VAR] = charger_image_en_element ("gfx/var_rouge.xpm",0,0,12,8);
	element[EL_CASE_P0_KILL] = charger_image_en_element ("gfx/kill_bleu.xpm",0,0,12,8);
	element[EL_CASE_P1_KILL] = charger_image_en_element ("gfx/kill_rouge.xpm",0,0,12,8);
	
	element[EL_IND_G] = charger_image_en_element ("gfx/ind_g.xpm",0,0,30,28);
	element[EL_IND_D] = charger_image_en_element ("gfx/ind_d.xpm",0,0,30,28);
	
	element[EL_CHIFFRES] = charger_image_en_element ("gfx/chiffres.xpm",0,0,220,32);
	element[EL_CHIFFRES_BLEU] = charger_image_en_element ("gfx/chiffres_bleu.xpm",0,0,220,32);
	element[EL_CHIFFRES_ROUGE] = charger_image_en_element ("gfx/chiffres_rouge.xpm",0,0,220,32);

	element[EL_FLECHE_B] = charger_image_en_element ("gfx/fleche_b.xpm",0,0,50,50);
	element[EL_FLECHE_H] = charger_image_en_element ("gfx/fleche_h.xpm",0,0,50,50);
	element[EL_FLECHE_G] = charger_image_en_element ("gfx/fleche_g.xpm",0,0,50,50);
	element[EL_FLECHE_D] = charger_image_en_element ("gfx/fleche_d.xpm",0,0,50,50);
	
	element[EL_MODE1] = charger_image_en_element ("gfx/mode1.xpm",0,0,50,50);
	element[EL_MODE2] = charger_image_en_element ("gfx/mode2.xpm",0,0,50,50);
	element[EL_MODE3] = charger_image_en_element ("gfx/mode3.xpm",0,0,50,50);

	element[EL_RANK1] = charger_image_en_element ("gfx/r1.xpm",0,0,30,30);
	element[EL_RANK2] = charger_image_en_element ("gfx/r2.xpm",0,0,30,30);
	element[EL_RANK3] = charger_image_en_element ("gfx/r3.xpm",0,0,30,30);
	
	element[EL_VS] = charger_image_en_element ("gfx/vs.xpm",0,0,100,67);

	element[EL_LOGO] = charger_image_en_element ("gfx/logo.xpm",0,0,400,80);
	
	// On charge toutes les couleurs dans la palette
	
	/* mode d'emploi: 
	 * - On ajoute le NOM_DE_MA_COULEUR dans le 'couleur_role' de graphique.h
	 * - on charge la couleur avec cette commande :
	 * palette[NOM_DE_MA_COULEUR] = CouleurParComposante(...); // ou CouleurParNom(...);
	 * - A l'avenir nous utiliserons NOM_DE_MA_COULEUR pour utiliser notre couleur
	 */
	
	palette[CLR_BACKGROUND] = hexa2couleur (0xFFFFFF);
	palette[CLR_TEXTE_DEFAUT] = hexa2couleur (0x9a662f);
	
	palette[CLR_TEXTE_INACTIF] = hexa2couleur(0xfac68f);
	palette[CLR_PROG0] = hexa2couleur(0x0000FF);
	palette[CLR_PROG1] = hexa2couleur(0xFF0000);
	
	palette[CLR_CASE_VIDE] = hexa2couleur(0xF0F0F0);
	palette[CLR_CASE_VIDE_CONTOUR] = hexa2couleur(0xdFdFdF);
	
	palette[CLR_CASE_P0] = hexa2couleur(0x9090FF);
	palette[CLR_CASE_P0_CONTOUR] = hexa2couleur(0x7070FF);
	
	palette[CLR_CASE_P1] = hexa2couleur(0xFF9090);
	palette[CLR_CASE_P1_CONTOUR] = hexa2couleur(0xFF7070);
	
	palette[CLR_CASE_IP0] = hexa2couleur(0x0000FF);
	palette[CLR_CASE_IP1] = hexa2couleur(0xFF0000);
	
	palette[CLR_TEXTE_INFO] = hexa2couleur(0x5a9a2f);
	
	palette[CLR_STEPBAR] = hexa2couleur (0x9a662f);
	palette[CLR_STEPBAR_BACK] = hexa2couleur (0xDEB080);
	
	palette[CLR_LISTE_COMBATTANT_BACK] = hexa2couleur (0xEAE1D0);
	palette[CLR_LISTE_COMBATTANT_CONTOUR] = hexa2couleur (0xC8AC89);
	palette[CLR_TEXTE_COMBATTANT] = hexa2couleur (0xB27B55);
	
	
	
	for(i=0;i<MAX_ELEMENT;i++)
		if(element[i]==0)
			fprintf(stderr,"attention: l'image de l'element %d n'a pas chargee\n",i);
	
	ChoisirCurseur (0);
		
	return 0;
}

void affichage_fermer (void)
{
	FermerGraphique();
	free_liste_element(&liste_el);
}


int affichage_avant_tournoi (combattant * cbt, int nb_combattants){
	
	temps current_time, refresh_diff_time, refresh_previous_time;
	bool quitter;
	
	/* On remplit la fenetre de la couleur de fond */
	buff_rect(0,0,W_FENETRE,H_FENETRE,CLR_BACKGROUND);
	
	/* On met le logo */
	buff_element (EL_LOGO, (W_FENETRE-400)/2 ,0);
	
	buff_ecrire_texte_xaligncenter ("Voici la liste des combattants",120,2,CLR_TEXTE_DEFAUT);
	
	buff_liste_combattants (cbt, nb_combattants, 10, 150, W_FENETRE-20, H_FENETRE -200 );
	
	
	buff_ecrire_texte_xaligncenter ("Appuyez sur n'importe quelle touche pour commencer le tournoi.",H_FENETRE-20,1,CLR_TEXTE_INFO);
	
	
	refresh_previous_time=get_temps();
	
	quitter=FAUX;
	while(!quitter)
	{
		current_time=get_temps();	
		
		quitter = pressanykey();
	
		if(quitter==QUITTER_AFFICHAGE)
			return QUITTER_AFFICHAGE;
				
		refresh_diff_time=current_time-refresh_previous_time;
		
		if(refresh_diff_time > REFRESH_TIME)
		{
			flip(); 
			refresh_previous_time=current_time;
		}
		else
		if(!quitter)
		{
			delay(REFRESH_TIME - refresh_diff_time); // pause le temps qu'il reste
		}
		
	}
	return 0;
}

int affichage_apres_tournoi (combattant * cbt, int nb_combattants){
		
	temps current_time, refresh_diff_time, refresh_previous_time;
	bool quitter;
	
	/* On remplit la fenetre de la couleur de fond */
	buff_rect(0,0,W_FENETRE,H_FENETRE,CLR_BACKGROUND);
	
	/* On met le logo */
	buff_element (EL_LOGO, (W_FENETRE-400)/2 ,0);
	
	buff_ecrire_texte_xaligncenter ("Voici le classement final des combattants",120,2,CLR_TEXTE_DEFAUT);
	
	buff_classement_combattants (cbt, nb_combattants, 10, 150, W_FENETRE-20, H_FENETRE -200 );
	
	buff_ecrire_texte_xaligncenter ("Appuyez sur n'importe quelle touche pour quitter.",H_FENETRE-20,1,CLR_TEXTE_INFO);
	
	
	
	refresh_previous_time=get_temps();
	
	quitter=FAUX;
	while(!quitter)
	{
		current_time=get_temps();
		
		
		quitter = pressanykey();
	
		if(quitter==QUITTER_AFFICHAGE)
			return QUITTER_AFFICHAGE;
				
		refresh_diff_time=current_time-refresh_previous_time;
		
		if(refresh_diff_time > REFRESH_TIME)
		{
			flip(); 
			refresh_previous_time=current_time;
		}
		else
		if(!quitter)
		{
			delay(REFRESH_TIME - refresh_diff_time); // pause le temps qu'il reste
		}
		
		
	}
	return 0;
}

int affichage_avant_combat (liste_combat l, combattant * cbt, int nb_combattants)
{
	temps current_time, refresh_diff_time, refresh_previous_time;
	bool quitter;
	
	char * s = malloc(sizeof(char) * 128);
	int count;
	
	/* On remplit la fenetre de la couleur de fond */
	buff_rect(0,0,W_FENETRE,H_FENETRE,CLR_BACKGROUND);
	
	/* On met le logo */
	buff_element (EL_LOGO, (W_FENETRE-400)/2 ,0);
	
	buff_ecrire_texte_xaligncenter ("Tournoi en cours",120,2,CLR_TEXTE_DEFAUT);
	
	buff_ecrire_texte("combats :",100,200,2,CLR_TEXTE_DEFAUT);
	buff_liste_combat (l,10,220,350,300);
	
	
	count=count_combat_restant(l);
	if(count>=2)
		snprintf(s,127,"Il reste %d combats.",count);
	else if(count==1)
		snprintf(s,127,"Il reste un combat.");
	else
		snprintf(s,127,"Il ne reste plus de combats.");
	
	buff_ecrire_texte(s,50,550,2,CLR_TEXTE_DEFAUT);
	
	
	buff_ecrire_texte("TOP 5 des combattants :",440,200,2,CLR_TEXTE_DEFAUT);
	buff_top5_combattants(cbt, nb_combattants, 500, 250, 300, 350);
	
	buff_ecrire_texte_xaligncenter ("Appuyez sur n'importe quelle touche pour Lancer le combat.",H_FENETRE-20,1,CLR_TEXTE_INFO);
	
	free(s);
	
	refresh_previous_time=get_temps();

	quitter=FAUX;
	while(!quitter)
	{
		current_time=get_temps();
		
		quitter = pressanykey();
		
		if(quitter==QUITTER_AFFICHAGE)
			return QUITTER_AFFICHAGE;

		refresh_diff_time=current_time-refresh_previous_time;
		
		if(refresh_diff_time > REFRESH_TIME)
		{
			flip();
			refresh_previous_time=current_time;
		}
		else
		if(!quitter)
		{
			delay(REFRESH_TIME - refresh_diff_time); // pause le temps qu'il reste
		}
		
		
	}
	
	// Apres l'affichage de la liste
	buff_combat_new();
	buff_waitingtime();
	buff_changemode();
	
	return 0;
}

int affichage_apres_combat (liste_combat l)
{
	
	return 0;
}

int affichage_avant_round(void)
{
	buff_scores();
	
	// Vide la stepbar
	buff_stepbar(1,1,CLR_STEPBAR_BACK);
	
	if(env.mode == MODE_MAXSPEED_NODISPLAY)
	{
		flip();
	}
	else
	{
		
		/* Affichage de la partie de Changement du temps de pause */
		buff_waitingtime();
	
		/* Affichage de la partie de Changement du mode */
		buff_changemode();
		
	}
	return 0;
}
int affichage_apres_round(int gagnant)
{
	if(actions_sur_pression_touche_combat ()==QUITTER_AFFICHAGE)
		return QUITTER_AFFICHAGE;
	if(env.mode == MODE_MAXSPEED_NODISPLAY)
		return 0;
	
	temps debut;
	temps refresh_previous_time, current_time, refresh_diff_time, temps_fin;
	bool quitter;
	
	// On travaille avec un cycle de taille limit_clign
	int limit_clign = TIME_END_MATCH / (REFRESH_TIME*NB_CLIGN);
	int iclign = 0;
	bool affiche_texte=FAUX;
	
	char * inf = info_fin_round2string( gagnant );
	
	int x,y,w,h; // zone prise par le texte
	
	
	
	x = (W_FENETRE - TailleChaineEcran(inf,2))/2;
	y = H_FENETRE/2 - 50 - TailleSupPolice(2);
	w = TailleChaineEcran(inf,2);
	h = TailleSupPolice(2) + TailleInfPolice(2);
	
	x=x-5;
	y=y-4;
	w=w+10;
	h=h+10;
	
	couleur_role clr_back, clr_texte;
	
		
	if(gagnant==-1)
	{
		clr_back=CLR_BACKGROUND;
		clr_texte=CLR_TEXTE_INFO;
	}
	else if(gagnant==0)
	{
		clr_back=CLR_CASE_P0;
		clr_texte=CLR_PROG0;
		env.mem_owner [ env.ip[1] ] = OWNER_P1_KILL;
		env.need_rebuff[ env.ip[1] ]  = VRAI;
		buff_zone_mem();
	}
	else if(gagnant==1)
	{
		clr_back=CLR_CASE_P1;
		clr_texte=CLR_PROG1;
		env.mem_owner [ env.ip[0] ] = OWNER_P0_KILL;
		env.need_rebuff[ env.ip[0] ]  = VRAI;
		buff_zone_mem();
	}
	
	
	/* On sauvegarde la zone 'sans texte' dans save (depuis buffer),
	 * On ecrit le texte dans buffer,
	 * On copie la zone 'avec texte' dans save
	 */
	
	save_buff_zone (x,y,w,h,0,0);
	
	
	buff_rect (x,y,w,h,clr_back);
	buff_combat_text_info (inf, clr_texte);
	
	free(inf);
	inf=NULL;
	
	save_buff_zone (x,y,w,h,0,100);
	
	
	
	refresh_previous_time=debut=get_temps();
	temps_fin=env.waitingtime;
	
	quitter=FAUX;
	while(!quitter)
	{
		
		current_time=get_temps();
		
		
		quitter = ((current_time-debut) > TIME_END_MATCH);


		if(actions_sur_pression_touche_combat ()==QUITTER_AFFICHAGE)
			return QUITTER_AFFICHAGE;
		
		
		if ( iclign==limit_clign )
		{
			iclign= 0;
			buff_save_zone (0,(affiche_texte)?100:0,w,h,x,y);
			affiche_texte = !affiche_texte;
		}
		else
			iclign ++;
		
		refresh_diff_time=current_time-refresh_previous_time;
		
		if(refresh_diff_time > REFRESH_TIME)
		{
			flip(); 
			refresh_previous_time=current_time;
		}
		else 
		if(!quitter)
		{
			delay(REFRESH_TIME - refresh_diff_time); // pause le temps qu'il reste
		}
		
		
	}
	return 0;
}

int affichage_avant_execution_instruction (int num_programme)
{
	if(actions_sur_pression_touche_combat ()==QUITTER_AFFICHAGE)
		return QUITTER_AFFICHAGE;
	if(env.mode == MODE_MAXSPEED_NODISPLAY)
		return 0;

	flip();
	return 0;
}

int affichage_apres_execution_instruction (int num_programme)
{
	actions_sur_pression_touche_combat ();
	if(env.mode == MODE_MAXSPEED_NODISPLAY)
		return 0;
	
	temps debut;
	temps refresh_previous_time, current_time, refresh_diff_time, temps_fin;
	bool quitter;

	
	buff_zone_mem();
	buff_stepbar(env.current_step, LIMIT_CYCLE, CLR_STEPBAR);
	
	buff_stat_prog(num_programme);
	buff_instruction_prog(num_programme);
	
	buff_instruction_selection(!num_programme); // prochain prog
	flip();
	
	
	refresh_previous_time=debut=get_temps();
	temps_fin=env.waitingtime;
	if(env.mode == MODE_MAXSPEED_WITHDISPLAY)
		quitter=VRAI;
	else
		quitter=FAUX;
	while(!quitter)
	{
		
		
		if(actions_sur_pression_touche_combat ()==QUITTER_AFFICHAGE)
			return QUITTER_AFFICHAGE;
			
		current_time=get_temps();
		refresh_diff_time=current_time-refresh_previous_time;
		
		quitter = ((current_time-debut) > temps_fin);
		

		if(env.mode != MODE_NORMAL)
				quitter=VRAI;
				
		if(refresh_diff_time > REFRESH_TIME)
		{

			//printf("%f\n",retourner_fps());
			
			flip(); // update_dernier_rafraichissement();
			
			
			refresh_previous_time=current_time;
		}
		else 
		if(!quitter)
		{
			delay(REFRESH_TIME - refresh_diff_time); // pause le temps qu'il reste
		}
		
		
	}
	return 0;
}



/// Autres fonctions internes a graphique.c
	
// Evenements claviers

void action (bouton btn)
{
	switch (btn)
	{
		case BTN_G:
			if(env.mode == MODE_NORMAL)
			env.mode = MODE_MAXSPEED_NODISPLAY;
			else if(env.mode == MODE_MAXSPEED_NODISPLAY)
				env.mode = MODE_MAXSPEED_WITHDISPLAY;
			else
				env.mode = MODE_NORMAL;
			break;

		case BTN_D:
			if(env.mode == MODE_NORMAL)
				env.mode = MODE_MAXSPEED_WITHDISPLAY;
			else if(env.mode == MODE_MAXSPEED_WITHDISPLAY)
				env.mode = MODE_MAXSPEED_NODISPLAY;
			else
				env.mode = MODE_NORMAL;
			break;
		
		case BTN_H:
			env.waitingtime += INCR_waitingtime;
			if(env.waitingtime > MAX_waitingtime)
				env.waitingtime -= INCR_waitingtime;
			break;
		
		case BTN_B:
			env.waitingtime -= INCR_waitingtime;
			if(env.waitingtime < MIN_waitingtime)
				env.waitingtime += INCR_waitingtime;
			break;
			
		case BTN_AUCUN :
		break;
	}
}

int actions_sur_pression_touche_combat (void)
{
	KeySym k;
	int actions_realisees=0;
	while(ToucheEnAttente())
	{
		actions_realisees++;
		k=Touche();
		switch (k)
		{
			case XK_Left:
				action (BTN_G);
				buff_changemode(); 
				buff_waitingtime(); // Changement de mode peux impliquer un changement d'affichage du temps de pause
				flip();
				break;
			
			case XK_Right:
				action (BTN_D);
				buff_changemode(); 
				buff_waitingtime(); // Changement de mode peux impliquer un changement d'affichage du temps de pause
				flip();
				break;
			
			case XK_Up:
				action (BTN_H);
				buff_waitingtime();
				break;
			
			case XK_Down:
				action (BTN_B);
				buff_waitingtime();
				break;
			
			case XK_Escape:
				return QUITTER_AFFICHAGE;
		}
		
		// printf("touche %d\n",k);
		
	}
	return actions_realisees;
}

int pressanykey (void)
{
	KeySym k;
	int nb=0;
	while(ToucheEnAttente())
	{
		nb++;
		k=Touche();
		if(k==XK_Escape)
			return QUITTER_AFFICHAGE;
	}
	return nb;
}


// Fonctions de gestions des images

bool superpose_avec_autre_element (rectangle rect)
{ // VRAI si on trouve une superposition de rect avec un autre element
	int i;
	bool superposition_trouve=FAUX;
	for(i=1; element_existe(liste_el,i) && !superposition_trouve; i++)
	{
		if( rectangle_superposition(rect,obtenir_rectangle_element(liste_el,i) ))
			superposition_trouve=VRAI;
	}
	return superposition_trouve;
}

int affecter_emplacement_element (int * x, int * y, int w, int h)
{ // Si possible affecte un emplacement a l'element sur la pixmap (modifie x et y)
	
	bool place_trouve;
	int i;
	int id;
	
	rectangle dernier_element;
	rectangle place;
	
	if(w>W_FENETRE || h>H_FENETRE) // Impossible de trouver de la place si cela depasse les dimensions de la fenetre
		return 0;
	
	if(liste_el==NULL)
	{
		// Pas encore d'element, on commence a remplir en haut a gauche
		*x=0;
		*y=0;
		id = 1; // le numero commence a 1
	}
	else
	{
		
		for(i=1; element_existe(liste_el,i); i++);
			id = i;
		
		dernier_element = obtenir_rectangle_element(liste_el,id-1);
		
		place=affecter_rectangle(0,0,w,h);
		
		place_trouve=FAUX;
		
		// On parcours les abscisses et ordonnees pour trouver une place
		// Pour simplifier le temps de calcul on reprends la recherche a l'ordonnee du dernier element
		for(place.y=dernier_element.y;(place.y<=(H_FENETRE-place.h)) && !place_trouve;place.y++) 
			for(place.x=0;(place.x<=(W_FENETRE-place.w)) && !place_trouve;place.x++)
				if(!superpose_avec_autre_element(place))
					place_trouve=VRAI;
				
		if(place.y>0)
			place.y --;
		if(place.x>0)
			place.x --;
		
		if(place_trouve)
		{
			*x = (int)place.x;
			*y = (int)place.y;
		}
		else
			id=0;
	}
	return id;
}


// Retourne l'id de l'element associe (0 si erreur)
int charger_image_en_element (char * file, int xx, int yy, int w, int h) // Adresse de l'image et ses dimensions (xx et yy correspondent a la position initiale sur l'image)
{
	int x,y;
	rectangle re;
	int id;
	
	// On verifie la bonne existance du fichier image
	FILE * f=fopen(file,"r");
	if(f==NULL)
	{
		fprintf(stderr,"Le chargement de l'image %s a echoue : impossible d'ouvrir le fichier image.\n",file);
		return 0;
	}
	else
		fclose(f);
	
	// On cherche un emplacement libre pour notre element
	id=affecter_emplacement_element(&x,&y,w,h);
	
	if(id)
	{
		// On enregistre l'element dans la liste des elements
		re=affecter_rectangle(x,y,w,h);
		liste_el=push_element(liste_el,id,re);
		
		if(liste_el==NULL)
			return 0;
		
		// On charge l'element sur le pixmap SCR_ELEMENT
		ChoisirEcran(SCR_ELEMENT);
		ChargerImage(file,x,y,xx,yy,w,h);
		ChoisirEcran(SCREEN);
	}
	else
		fprintf(stderr,"Le chargement de l'image %s a echoue : pas de place libre sur le pixmap pour cette image.\n",file);
	
	return id;
}

int copier_element_partiel (element_nom nom_el, short x_el, short y_el, short w_el, short h_el, short x, short y, int num_ecran)
{
	int id;
	
	rectangle r;
	
	id = element[nom_el];
	if (element_existe(liste_el,id))
		r = obtenir_rectangle_element (liste_el, id);
	else
	{
		fprintf(stderr,"L'element demande (%d) n'existe pas! (id:%d)",nom_el,id);
		return 1;
	}
	
	ChoisirEcran(num_ecran);
	
	if(w_el<=0)
		w_el=r.w;

	if(h_el<=0)
		h_el=r.h;
	
	CopierZone(SCR_ELEMENT,num_ecran,r.x+x_el,r.y+y_el,w_el,h_el,x,y);
	
	return 0;
}

int copier_element (element_nom nom_el, short x, short y, int num_ecran)
{
	return copier_element_partiel (nom_el, 0, 0, -1, -1, x, y, num_ecran);
}


// FONCTIONS buff_*

// Toute les constantes externes des bufferisations (positions des elements)
	const int yalign_text_p = 25;
	const int xalign_text_p0 = 50;
	const int xalign_text_p1 = 680;
	const int yoffset_text_p = 30;
	const int yalign_score = 20;
	const int x_score0 = W_FENETRE/2-120-60;
	const int x_score1 = W_FENETRE/2+120;
	const int x_num_round = W_FENETRE/2-30;
	const int y_num_round = 65;
	
	const int xalign_changetime = 100;
	const int yalign_changetime = H_FENETRE - 70;
	
	const int xalign_changemode = 550;
	const int yalign_changemode = H_FENETRE - 70;
	
#define changetime "Changer le temps de pause"
#define changemode "Changer de mode"


void buff_waitingtime (void)
{
	int x, y;
	couleur_role tmp_clr;
	char * s=waitingtime2string ();
	if(s!=NULL)
	{
		if(env.mode!=MODE_NORMAL)
			tmp_clr = CLR_TEXTE_INACTIF;
		else
			tmp_clr = CLR_TEXTE_DEFAUT;
		// On doit remettre un fond blanc
		x=xalign_changetime+80;
		y=yalign_changetime+42;
		buff_rect(x,y-20,80,20,CLR_BACKGROUND);
		buff_ecrire_texte (s,x, y, 2, tmp_clr);
		free(s);
	}
}


void buff_changemode (void)
{
	element_nom tmp_eln;
	if(env.mode==MODE_MAXSPEED_NODISPLAY)
		tmp_eln = EL_MODE3;
	else if(env.mode==MODE_MAXSPEED_WITHDISPLAY)
		tmp_eln = EL_MODE2;
	else
		tmp_eln = EL_MODE1;
	buff_element (tmp_eln, xalign_changemode+70, yalign_changemode+10);
}


void buff_instruction_prog (int num_programme)
{
	int x, y;
	char * s;
	couleur_role clr;
	
	/* Affichage de l'instruction en cours pour chacun des deux programmes */
	y = yalign_text_p + 2*yoffset_text_p;
	
	s=ins2string( env.mem[ env.ip[num_programme] ] );
	if(s!=NULL)
	{
		if(num_programme==0)
		{
			x=xalign_text_p0;
			clr=CLR_PROG0;
		}
		else
		{
			x=xalign_text_p1;
			clr=CLR_PROG1;
		}
	
		
		
		buff_rect (x,y-20,200,30,CLR_BACKGROUND);
		buff_ecrire_texte (s,x,y,1,clr);
		free(s);
	}
}

void buff_instruction_selection (int num_programme)
{
	int x, y;
	couleur_role clr;
	y = yalign_text_p + 2*yoffset_text_p;
	
	if(num_programme==0)
		{
			x=xalign_text_p0;
			clr=CLR_PROG0;
			
			buff_rect(xalign_text_p1-25,y-14,15,15,CLR_BACKGROUND);
		}
		else
		{
			x=xalign_text_p1;
			clr=CLR_PROG1;
			
			buff_rect(xalign_text_p0-25,y-14,15,15,CLR_BACKGROUND);
		}
	buff_rect(x-25,y-14,15,15,clr);
}

void buff_stat_prog (int num_programme)
{
	char * s;
	int x, y;
	couleur_role clr;
	
	/* Affichage des statistiques actuelles de memoires acquis par chacun des deux programmes */
	
	
	s=prog_own_stats2string (env.nb_case_owned[num_programme],TAILLE_MEM);
	if(s!=NULL)
	{
		
		
		y =yalign_text_p+yoffset_text_p;
		if(num_programme==0)
		{
			x=xalign_text_p0;
			clr=CLR_PROG0;
		}
		else
		{
			x=xalign_text_p1;
			clr=CLR_PROG1;
		}
		
		//  Attention ! sur l'affichage il faut effacer l'ancien texte !
			buff_rect (x,y-16,130,22,CLR_BACKGROUND);
		
		buff_ecrire_texte (s,x,y,1,clr);
		free(s);
	}

}


void buff_scores (void)
{
	/*Affichage des scores actuelles : pas de probleme on ecrase l'ancien graphiquement */
	buff_ecrire_nombre (env.nb_gagne[0],  3 , x_score0, yalign_score, EL_CHIFFRES_BLEU);
	buff_ecrire_nombre (env.nb_gagne[1],  3, x_score1, yalign_score, EL_CHIFFRES_ROUGE);
	buff_ecrire_nombre (env.num_round_actuel, 3, x_num_round, y_num_round, EL_CHIFFRES);


}

void buff_combat_new (void)
{
	/* On remplit la fenetre de la couleur de fond */
	buff_rect(0,0,W_FENETRE,H_FENETRE,CLR_BACKGROUND);
	
	/* Affichages des noms des programmes */

	if(env.nom[0] != NULL)
		buff_ecrire_texte (env.nom[0],xalign_text_p0,yalign_text_p,1,CLR_PROG0);
	
	if(env.nom[1] != NULL)
		buff_ecrire_texte (env.nom[1],xalign_text_p1,yalign_text_p,1,CLR_PROG1);

	/* Affichage de "VS" entre les deux textes des programmes */
	buff_element (EL_VS, (W_FENETRE-100)/2, 0);
	
	
	/* Affichage de la partie de Changement du temps de pause */
	buff_element (EL_FLECHE_B, xalign_changetime, yalign_changetime+10);
	buff_element (EL_FLECHE_H, xalign_changetime+200, yalign_changetime+10);
	buff_ecrire_texte (changetime,xalign_changetime+30,yalign_changetime,1,CLR_TEXTE_DEFAUT);
	
	
	/* Affichage de la partie de Changement du mode */
	buff_element (EL_FLECHE_G, xalign_changemode, yalign_changemode+10);
	buff_element (EL_FLECHE_D, xalign_changemode+140, yalign_changemode+10);
	buff_ecrire_texte (changemode,xalign_changemode+30,yalign_changemode,1,CLR_TEXTE_DEFAUT);
	
}


void buff_liste_combattants (combattant * cbt, int nb_combattants, int x, int y, int w, int h)
{
	int i;
	
	int posx, posy;
	
	char * snum=malloc(sizeof(char)*16);
	char * sseqsize=malloc(sizeof(char)*32);
	
	buff_rect(x,y,w,h,CLR_LISTE_COMBATTANT_CONTOUR);
	buff_rect(x+10,y+10,w-20,h-20,CLR_LISTE_COMBATTANT_BACK);
	
	posx = x+50;
	posy = y+50;
	
	if(nb_combattants<=8)
	{
		posx+=w/4;
		if(nb_combattants==2)
			posy+=h/4;
	}
	
	
	for(i=0;i<nb_combattants;i++)
	{
		
		buff_ecrire_texte (cbt[i].nom,posx, posy, 2, CLR_TEXTE_COMBATTANT);
		
		snprintf(snum,15,"num: %d",cbt[i].num);
		buff_ecrire_texte (snum, posx+10, posy+30, 1, CLR_TEXTE_COMBATTANT);
		
		snprintf(sseqsize,31,"taille: %d instr.",cbt[i].seq.size);
		buff_ecrire_texte (sseqsize, posx+10, posy+50, 1, CLR_TEXTE_COMBATTANT);
		
		if(((i+1)%4)==0)
		{
			posx += w/4;
			posy = y+50;
		}
		else
			posy+= h/4;
	}
	
	free(snum);
	free(sseqsize);
}

void buff_classement_combattants (combattant * cbt, int nb_combattants, int x, int y, int w, int h)
{
	int i,j;
	
	int posx, posy;
	
	char * s=malloc(sizeof(char)*50);
	char * spoint=malloc(sizeof(char)*32);
	
	
	element_nom el;
	
	combattant * c = malloc(sizeof(combattant)*nb_combattants);
	
	bool cbt_recupere[MAX_COMBATTANTS];
	
	for(i=0;i<nb_combattants;i++)
		cbt_recupere[i] = FAUX;
	
	int place;
	int indice;
	
	// tri selon la place
	
	for(j=0;j<nb_combattants;j++)
	{
		place=MAX_COMBATTANTS+1;
		
		for(i=0;i<nb_combattants;i++)
			if(!cbt_recupere[i] && place>cbt[i].place)
			{
				indice=i;
				place=cbt[i].place;
			}
		
		cbt_recupere[indice]=VRAI;
		c[j] = cbt[indice];
		
	}
	
	
	buff_rect(x,y,w,h,CLR_LISTE_COMBATTANT_CONTOUR);
	buff_rect(x+10,y+10,w-20,h-20,CLR_LISTE_COMBATTANT_BACK);
	
	posx = x+50;
	posy = y+50;
	
	if(nb_combattants<=8)
	{
		posx+=w/4;
		if(nb_combattants==2)
			posy+=h/4;
	}
	
	
	for(i=0;i<nb_combattants;i++)
	{
		
		if(c[i].place>3)
		{
			snprintf(s,49,"%d - %s",c[i].place,c[i].nom);
			buff_ecrire_texte (s, posx, posy, 2, CLR_TEXTE_COMBATTANT);
		}
		else
		{
			if(c[i].place<=1)
				el=EL_RANK1;
			else if(c[i].place==2)
				el=EL_RANK2;
			else
				el=EL_RANK3;
				
			buff_element (el,posx, posy-30);
			//buff_rect (posx,posy-30,30,30,CLR_CASE_P0);
			buff_ecrire_texte (c[i].nom, posx+40, posy, 2, CLR_TEXTE_COMBATTANT);
		}
		
		snprintf(spoint,31,"avec %d points.",c[i].point);
		buff_ecrire_texte (spoint, posx, posy+30, 1, CLR_TEXTE_COMBATTANT);
		
		
		if(((i+1)%4)==0)
		{
			posx += w/4;
			posy = y+50;
		}
		else
			posy+= h/4;
	}
	
	free(s);
	free(spoint);
	free(c);
}

void buff_top5_combattants(combattant * cbt, int nb_combattants, int x, int y, int w, int h)
{
	int i,j;
	
	
	int posx, posy;
	
	char * s=malloc(sizeof(char)*50);
	char * spoint=malloc(sizeof(char)*32);
	
	
	element_nom el;
	
	combattant * c = malloc(sizeof(combattant)*nb_combattants);
	
	bool cbt_recupere[MAX_COMBATTANTS];
	
	for(i=0;i<nb_combattants;i++)
		cbt_recupere[i] = FAUX;
	
	int place;
	int indice;
	
	// tri selon la place
	
	for(j=0;j<nb_combattants;j++)
	{
		place=MAX_COMBATTANTS+1;
		
		for(i=0;i<nb_combattants;i++)
			if(!cbt_recupere[i] && place>cbt[i].place)
			{
				indice=i;
				place=cbt[i].place;
			}
		
		cbt_recupere[indice]=VRAI;
		c[j] = cbt[indice];
		
	}
	
	posx = x+20;
	posy = y+50;

	buff_rect(x,y,w,h,CLR_LISTE_COMBATTANT_CONTOUR);
	buff_rect(x+5,y+5,w-10,h-10,CLR_LISTE_COMBATTANT_BACK);
	
	
	for(i=0;i<nb_combattants && i<5;i++)
	{
		
		if(c[i].place>3)
		{
			snprintf(s,49,"%d - %s",c[i].place,c[i].nom);
			buff_ecrire_texte (s, posx, posy, 2, CLR_TEXTE_COMBATTANT);
		}
		else
		{
			if(c[i].place<=1)
				el=EL_RANK1;
			else if(c[i].place==2)
				el=EL_RANK2;
			else
				el=EL_RANK3;
				
			buff_element (el,posx, posy-30);
			//buff_rect (posx,posy-30,30,30,CLR_CASE_P0);
			buff_ecrire_texte (c[i].nom, posx+40, posy, 2, CLR_TEXTE_COMBATTANT);
		}
		
		snprintf(spoint,31,"avec %d points.",c[i].point);
		buff_ecrire_texte (spoint, posx+10, posy+24, 1, CLR_TEXTE_COMBATTANT);
		
		posy += w/5;
	}
	
	
	
	
	free(s);
	free(spoint);
	free(c);
	
}



void buff_liste_combat (liste_combat l, int x, int y, int w, int h)
{
	int i;
	
	int posx, posy;
	
	int p;
	
	int pcenter=x+w/2;
	
	chainon_combat * c;
	
	char * sscore[2];
	
	for(i=0;i<2;i++)
		sscore[i]=malloc(sizeof(char)*16);
	
	posx = x+40;
	posy = y+10;
	
	for(i=-2;i<=2;i++)
	{
		
		c=offset_combat(l,i);
			
		
		if(c!=NULL)
		{			
			p=pcenter-TailleChaineEcran(c->cbt[0]->nom,1)-40;
			buff_ecrire_texte(c->cbt[0]->nom,p,posy,1,CLR_PROG0);
			if(i<0)
			{
				snprintf(sscore[0],15,"%03d",c->score[0]);
				p=pcenter-TailleChaineEcran(sscore[0],1)-40;
				buff_ecrire_texte(sscore[0],p,posy+25,1,CLR_PROG0);
			}
			else
				if(i==0)
					buff_element(EL_IND_D, p-40,posy-20);
			
						p=pcenter-TailleChaineEcran("VS",2)/2;
			buff_ecrire_texte("VS",p,posy+10,2,CLR_TEXTE_DEFAUT);

			
			p=pcenter+40;
			buff_ecrire_texte(c->cbt[1]->nom,p,posy,1,CLR_PROG1);
			if(i<0)
			{
				snprintf(sscore[1],15,"%03d",c->score[1]);
				buff_ecrire_texte(sscore[1],p,posy+25,1,CLR_PROG1);
			}
			else
				if(i==0)
					buff_element(EL_IND_G, p+TailleChaineEcran(c->cbt[1]->nom,1)+10,posy-20);
			

			
			
		}
		
		posy+=h/5;
	}
	
	
	
	
	
	
	for(i=0;i<2;i++)
		free(sscore[i]);
}

void buff_combat_text_info (char * texte, couleur_role clr_texte)
{
	const int x = (W_FENETRE - TailleChaineEcran(texte,2))/2;
	const int y = H_FENETRE/2 - 50;
	buff_ecrire_texte(texte,x,y,2,clr_texte);
}

void buff_ecrire_texte (char * texte, int x, int y, int taille, couleur_role c)
{
	ChoisirEcran (SCR_BUFF);
	ChoisirCouleurDessin(palette[c]);
	EcrireTexte(x,y,texte,taille);
}

void buff_ecrire_texte_xaligncenter (char * texte, int y, int taille, couleur_role c)
{
	const int x = (W_FENETRE - TailleChaineEcran(texte,taille))/2;
	buff_ecrire_texte (texte,x,y,taille,c);
}

#define ELEMENT_CHIFFRE_W 20

void buff_ecrire_nombre (int nombre, int nb_chiffre, short x, short y, element_nom el_chiffre)
{
	int i, chiffre;
	
// on decompose le nombre en chiffres que l'on affiche au fur et a mesure du poids faible vers le poids fort
	if(nombre>=0)
	for(i=nb_chiffre-1;i>=0;i--){
		chiffre=nombre%10;
		nombre-=chiffre;
		nombre/=10;
		copier_element_partiel(el_chiffre,chiffre*ELEMENT_CHIFFRE_W,0,ELEMENT_CHIFFRE_W,-1,x+i*ELEMENT_CHIFFRE_W,y,SCR_BUFF);
	}
}

void buff_stepbar (int step, int total, couleur_role c)
{
	int x = (step*(W_FENETRE-80))/total;
	buff_rect(40,0,x,4,c);
}


void buff_case_ip (int x, int y, int w, int h, couleur_role rc)
{
	buff_rect (x,y,w,h,rc);
}

void buff_case_memoire (int x, int y, int w, int h, int mem_owner)
{
	couleur_role centre, contour;
	element_nom nel;
	bool est_image=FAUX;
	
	switch (mem_owner)
	{
		case OWNER_CASE_VIDE: // case vide
		centre=CLR_CASE_VIDE;
		contour=CLR_CASE_VIDE_CONTOUR;
		break;
		
		case OWNER_P0: // programme de gauche
		centre=CLR_CASE_P0;
		contour=CLR_CASE_P0_CONTOUR;
		break;
		
		case OWNER_P1: // programme de droite
		centre=CLR_CASE_P1;
		contour=CLR_CASE_P1_CONTOUR;
		break;
		
		case OWNER_P0_BOMB:
		est_image=VRAI;
		nel=EL_CASE_P0_BOMB;
		contour=CLR_CASE_P0_CONTOUR;
		break;
		
		case OWNER_P1_BOMB:
		est_image=VRAI;
		nel=EL_CASE_P1_BOMB;
		contour=CLR_CASE_P1_CONTOUR;
		break;
		
		case OWNER_P0_VAR:
		est_image=VRAI;
		nel=EL_CASE_P0_VAR;
		contour=CLR_CASE_P0_CONTOUR;
		break;
		
		case OWNER_P1_VAR:
		est_image=VRAI;
		nel=EL_CASE_P1_VAR;
		contour=CLR_CASE_P1_CONTOUR;
		break;
		
		case OWNER_P0_KILL:
		est_image=VRAI;
		nel=EL_CASE_P0_KILL;
		contour=CLR_CASE_P0_CONTOUR;
		break;
		
		case OWNER_P1_KILL:
		est_image=VRAI;
		nel=EL_CASE_P1_KILL;
		contour=CLR_CASE_P1_CONTOUR;
		break;
		
		default :
		centre=CLR_BACKGROUND;
		contour=CLR_BACKGROUND;
	}
	
	if(est_image)
		buff_element(nel,x,y);
	else
		buff_rect (x+1,y+1,w-2,h-2,centre);
	
	// Bordure
	buff_segment (x,y,x+w-1,y,contour);
	buff_segment (x+w-1,y,x+w-1,y+h-1,contour);
	buff_segment (x,y,x,y+h-1,contour);
	buff_segment (x,y+h-1,x+w-1,y+h-1,contour);
}

void buff_zone_memoire (int x_debut, int y_debut, int w, int w_unicase, int h_unicase)
{
	int i;
	int x, y;
	
	x=x_debut;
	y=y_debut;

	
	for(i=0;i<TAILLE_MEM;i++)
	{
		
		if(x >= x_debut+w)
		{
			x=x_debut;
			y += h_unicase;
		}
		if(env.need_rebuff[i])
		{	
			
			if (i==env.ip[0] && env.mem_owner[i]!=OWNER_P0_KILL)
				buff_case_ip(x,y,w_unicase,h_unicase,CLR_CASE_IP0);
			else if (i==env.ip[1] && env.mem_owner[i]!=OWNER_P1_KILL)	
				buff_case_ip(x,y,w_unicase,h_unicase,CLR_CASE_IP1);
			else
				buff_case_memoire(x,y,w_unicase,h_unicase,env.mem_owner[i]);
			
			env.need_rebuff[i]=FAUX;
		}
		x += w_unicase;
	}
	
}


// buff_* D'ordre plus general :

int buff_element (element_nom nom_el, short x, short y)
{
	return copier_element (nom_el,x,y,SCR_BUFF);
}

void buff_rect (int x, int y, int w, int h, couleur_role c)
{
	ChoisirEcran(SCR_BUFF);
	ChoisirCouleurDessin(palette[c]);
	RemplirRectangle (x,y,w,h);
}

void buff_segment (int x, int y, int xx, int yy, couleur_role c)
{
	ChoisirEcran(SCR_BUFF);
	ChoisirCouleurDessin(palette[c]);
	DessinerSegment(x,y,xx,yy);
}

void save_buff_zone (int x, int y, int w, int h, int x_save, int y_save)
{ // Sauvegarde un morceau du buffer (rect: x y w h) dans save (position x_save y_save)
	CopierZone(SCR_BUFF,SCR_SAVE,x,y,w,h,x_save,y_save);
}
void buff_save_zone (int x, int y, int w, int h, int x_buff, int y_buff)
{  // Restaure un morceau du save (rect: x y w h) dans buffer (position x_buff y_buff)
	CopierZone(SCR_SAVE,SCR_BUFF,x,y,w,h,x_buff,y_buff);
}





// Autres


char * waitingtime2string (void)
{
	char *s;
	s=malloc(sizeof(char)*12);
	
	sprintf(s,"%.2f sec",env.waitingtime / 1000000.0);
	
	return s;
}

char * info_fin_round2string (int gagnant)
{ // -1 match nul, 0, 1 numero du programme gagnant
	const int len = 100;
	int  perdant= (gagnant==0) ? 1:0;
	char * s = malloc(sizeof(char) * len);
	if(gagnant==-1)
	{
		snprintf(s,len-1,"Match nul : les deux programmes fonctionnent toujours.");	
	}
	else
	{
		snprintf(s,len-1,"%s a gagne contre %s",env.nom[gagnant],env.nom[perdant]);
	}
	return s;
}



#ifdef DEBUG
// pour le debug / stats de performance

void update_dernier_rafraichissement (void)
{
	env.dernier_rafraichissement = get_temps();	
}

temps retourner_temps_passe (void)
{
	return get_temps()-env.dernier_rafraichissement;
	
}

float retourner_fps (void)
{
	temps t = get_temps()-env.dernier_rafraichissement;
	if(t)
		return (1000000.0/(float)(t));
	else
		return 0.0;
}

void DEBUG_temps_perdu (void)
{
	printf("temps perdu: %f\n",(get_temps()-env.dernier_rafraichissement)/1000000.0); update_dernier_rafraichissement();
}

#endif
