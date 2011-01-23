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
 * @file desassembleur.c
 * @brief 
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

// Contient toute les fonctions pour le module desassembleur

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"desassembleur.h"
#include"../general.h"
#include"../verificateur.h"


const char * valid_mnemonique[TOTAL_MNEMONIQUE] = 
{"DAT","MOV","ADD","SUB","JMP","JMZ","JMN","DJN","CMP"};


char * ins2string  (instruction instr)
{ // Utile pour l'affichage graphique
	char * s = NULL;
	
	int ins = instruction2ins(instr);
	int mode1 = instruction2mode1(instr);
	int mode2 = instruction2mode2(instr);
	int arg1 = instruction2arg1(instr);
	int arg2 = instruction2arg2(instr);
	
	
	s=malloc(sizeof(char) * 32);
	
	if (!instruction_correcte(instr))
	{
		free(s);
		return NULL;
	}
	
	sprintf(s,"%s ",valid_mnemonique[ins]);
	
	
	if(ins != DAT)
	{
		if (mode1 == 0)
			sprintf(s,"%s#",s);
		else if (mode1 == 2)
			sprintf(s,"%s@",s);
		sprintf(s,"%s%d ",s,arg1);
	}
	
	if(ins != JMP)
	{
		if (mode2 == 0)
			sprintf(s,"%s#",s);
		else if (mode2 == 2)
			sprintf(s,"%s@",s);
		sprintf(s,"%s%d",s,arg2);
	}
	
	return s;
}



int afficher_instruction (instruction instr)
{
	
	char * s = malloc(sizeof(char)*32);
	
	s=ins2string(instr);
	if(s==NULL)
		return 1;
	else
	{
		printf("%s",s);
		return 0;
	}
}

int decompiler (FILE * f_input)
{
	int err=0;
	int compteur=0;
	
	instruction ins;
	
	fseek(f_input, 0, SEEK_SET);
	
	fread(&ins,sizeof ins, 1, f_input);
	while(!feof(f_input) && compteur<LIMIT_TAILLE_PROG)
	{
		compteur++;
		if(afficher_instruction (ins))
			fprintf(stdout, "; warning: instruction numero %d incorrecte",compteur);
		putchar('\n');
		fread(&ins,sizeof ins, 1, f_input);
	}
	
	if(compteur>=LIMIT_TAILLE_PROG)
	{
		fprintf(stderr, "erreur : il y a trop d'instruction !\n");
		err++;
	}
	
	return err;
}
