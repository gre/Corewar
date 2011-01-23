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
 * @file desassembleur_main.c
 * @brief contain desassembleur main fonction
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

// Contient le main du module desassembleur

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"desassembleur.h"

#include"../verificateur.h"

int main (int argc, char * argv[])
{
	FILE * f_input = NULL;
	
	
	int n_err;
	
	// Filtrage des erreurs
	
	if(argc != 2)
	{ // Nombre d'arguments incorrecte
		fprintf(stderr,"Usage: %s <fichier_binaire>\n",argv[0]);
		return EXIT_FAILURE;
	}
	
	f_input = fopen(argv[1], "r");
	if(f_input==NULL)
	{ // Fichier source non ouvert
		fprintf(stderr,"Impossible de lire le fichier binaire '%s'.\n",argv[1]);
		return EXIT_FAILURE;
	}
	
	
	if(!fichier_problablement_binaire(f_input))
		fprintf(stderr, "erreur: Il semblerait que le fichier soit un fichier texte.\n");
	else
	{
		printf("; decompilation de %s\n",argv[1]);

		n_err=decompiler (f_input);
	}
	
	fclose(f_input);
	
	
	return EXIT_SUCCESS;
}
