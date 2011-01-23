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
 * @file console.c
 * @brief console mode (not used)
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

// Contient tout ce qui constitue l'affichage graphique, il est base sur les memes fonctions que dans graphique.c
#include<stdio.h>
#include<stdlib.h>

#include<unistd.h>

#include"console.h"
#include"../machine_virtuelle.h"
#include"../../desassembleur/desassembleur.h"
#include"../listes.h"

#define delay(t) usleep((useconds_t)t)


void afficher_classement_ordre_programme (combattant * cbt, int nb_combattants)
{
	int i, j;
	
	
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
	
	printf("Voici le classement:\n");
	for(i=0;i<nb_combattants;i++)
		printf("%s est %de avec %d points.\n",c[i].nom,c[i].place,c[i].point);
	
	free(c);
}

int affichage_avant_tournoi (combattant * cbt, int nb_combattants){
	int i;
	printf("Le tournoi commence!\n");
	printf("Voici la liste des %d participants :\n", nb_combattants);
	for(i=0;i<nb_combattants;i++)
		printf("%s ",cbt[i].nom);
	putchar('\n');
	delay(1000000);
	printf("\n\tmode affichage instruction (1)\n\tmode affichage moins detaille (2)\n\tmode sans affichage (3)\nQuel mode souhaitez-vous ?");
	scanf("%d",&i);
	while(i!=1 && i!=2 && i!=3)
	{
		printf("Retapez: ");
		scanf("%d",&i);
	}
	if(i==1) env.mode=MODE_NORMAL;
	else if(i==2) env.mode=MODE_MAXSPEED_WITHDISPLAY;
	else if(i==3) env.mode=MODE_MAXSPEED_NODISPLAY;
	
	if(env.mode==MODE_NORMAL)
	{
		printf("Indiquez la vitesse en microsecondes (comprise entre %d et %d):",MIN_waitingtime, MAX_waitingtime);
		scanf("%d",&i);
		while(i<MIN_waitingtime || i>MAX_waitingtime)
		{
			printf("Retapez: ");
			scanf("%d",&i);
		}
		env.waitingtime=i;
	}
	return 0;
}
int affichage_apres_tournoi (combattant * cbt, int nb_combattants){
	afficher_classement_ordre_programme(cbt,nb_combattants);
	return 0;
}

int affichage_avant_combat (liste_combat l, combattant * cbt, int nb_combattants){
	printf("Combat: %s contre %s.\n",env.nom[0], env.nom[1]);
	return 0;
}
int affichage_apres_combat (liste_combat l){
	int gagnant;
	int perdant;
	if(env.nb_gagne[0]==env.nb_gagne[1])
		printf("Match nul : %d round gagne chacun\n",env.nb_gagne[0]);	
	else
	{
		gagnant = (env.nb_gagne[0]>env.nb_gagne[1]) ? 0 : 1;
		perdant = (gagnant==0) ? 1 : 0;
		
		printf("%s gagne avec %d victoire contre %d de %s\n",
		env.nom[gagnant],env.nb_gagne[gagnant],
		env.nb_gagne[perdant],env.nom[perdant]);
		
	}
	return 0;
}

int affichage_avant_round(void){
	if (env.mode!=MODE_MAXSPEED_NODISPLAY)
		printf("debut round %d\n",env.num_round_actuel);
	return 0;
}
int affichage_apres_round(int gagnant){
	if (env.mode!=MODE_MAXSPEED_NODISPLAY)
	{
		printf("fin round %d : ",env.num_round_actuel);
		if(gagnant==-1)
			printf("Match nul\n");
		else
		{
			printf("gagnant: %s\n", env.nom[gagnant]);
		}
		delay(2*env.waitingtime);
	}
	return 0;
}

int affichage_avant_execution_instruction(int num_programme){
	if (env.mode==MODE_NORMAL)
	{
		if(num_programme==1)
			printf("\t\t\t");
		printf("%d: ",env.ip[num_programme]); 
		afficher_instruction(env.mem[env.ip[num_programme]]);
		putchar('\n');
	}
	return 0;
}
int affichage_apres_execution_instruction(int num_programme){
	if (env.mode==MODE_NORMAL)
		delay(env.waitingtime);
	return 0;
}

int affichage_instruction_non_executable(int num_programme){
	if (env.mode!=MODE_MAXSPEED_WITHDISPLAY)
	{
		instruction ins = env.mem[env.ip[num_programme]];
		printf("%s a rencontre une instruction non executable: ",env.nom[num_programme]);
		printf("case %d, code instruction %d.",env.ip[num_programme],instruction2ins(ins));
		putchar('\n');
	}
	return 0;
}
