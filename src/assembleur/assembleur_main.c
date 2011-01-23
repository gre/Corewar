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
 * @file assembleur_main.c
 * @brief contain assembleur main function
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

// Contient le main du module assembleur


#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"assembleur.h"
#include"../general.h"


int main (int argc, char * argv[])
{
	/// ICI on gere la ligne de commande tape par l'utilisateur
	/* Deux synthaxes possibles :
	 * assembleur <input>
	 * assembleur <input> -o <output>  (ou)  assembleur -o <output> <input>
	 */

	FILE * f_input = NULL;
	FILE * f_output = NULL;
	
	char * input_name = NULL;
	char * output_name = NULL;
	
	sequence seq;
	
	bool afficher_aide;
	int position_o_rencontre;
	
	// variables temporaires
	char c;
	int i;
	
	
	/* On effectue tous les tests de verification de la ligne de commande 
	 * et ceci de la maniere la plus efficace possible */
	
	if(argc!=2 && argc!=4) // les deux synthaxes ne sont pas respectees
		afficher_aide=VRAI;
	else
	{	
		afficher_aide=FAUX;
		position_o_rencontre=0;
		for(i=1; i<argc && !afficher_aide; i++)
			if( (*argv[i]=='-') )
			{
				if(*(argv[i]+1)=='o' && *(argv[i]+2)=='\0' )
				{ // L'option est 'o' (et seulement 'o')
					if(position_o_rencontre!=0)
						afficher_aide=VRAI;
					else
					{
						position_o_rencontre=i;
						if((i==3) || (argc!=4))
							afficher_aide=VRAI; // Dans la deuxieme synthaxe le -o se trouve soit en pos 1 soit en 2, PAS EN 3 !!!
					}
				}
				else
					afficher_aide=VRAI;	
			}
		if(argc==4 && position_o_rencontre==0)
			afficher_aide=VRAI;	
	}
	
	
	
	if (afficher_aide) 
	{ // Si la synthaxe passee sur la ligne de commande est incorrecte, on affiche l'aide
		
		// Affichage de l'aide (le 'HOW TO') -> source d'inspiration: 'tar --help' :-)
		
		// Usages possibles
		printf("Usage:\n");
		printf("\t%s [-o ouput] input\n\n",argv[0]);

		printf("Le programme 'assembleur' permet d'assembler un fichier source du langage CoreWar (la version du microprojet) vers un fichier binaire compile.\n\n");
		printf("Exemples :\n");
		printf("\t%s fichier.cws\t# fichier de sortie: fichier.cwb\n",argv[0]);
		printf("\t%s -o fichierbin.cwb fichier.cws\n",argv[0]);
		printf("\t%s fichier.cws -o fichierbin.cwb\n",argv[0]);
		return EXIT_FAILURE;
	}
	else
	{
		// On commence par localiser ou est le nom de fichier d'entree
		if(argc==2)
		{ // pour la synthaxe 1
			input_name=argv[1];
		}
		else
		{ // pour la synthaxe 2
			if(position_o_rencontre == 1)
				input_name=argv[3];
			else
				input_name=argv[1];
		}
		
		ins_env_info.filename = input_name;
		
		/// On tente d'ouvrir le fichier d'entree
		
		f_input = fopen(input_name, "r");
		if(f_input==NULL)
		{ // Probleme a l'ouverture
			fprintf(stderr,"Impossible de lire le fichier source '%s'\n",input_name);
			return EXIT_FAILURE;
		}
		else
		{
			fclose(f_input);
			f_input=NULL;
		}
		
			
		
		/// On met le nom de sortie dans output_name
		
		
		if(argc==2)
		{ // C'est nous qui gerons le nom de fichier de sortie
			output_name = generate_output_name((const char *)input_name);
			if( output_name == NULL )
			{
				fprintf(stderr,"Echec de la compilation au moment de la generation du fichier de sortie.\n");
				return EXIT_FAILURE;
			}
		}
		else
		{ // Le nom du fichier de sortie est precise : sa position est : 'position_o_rencontre+1'
			output_name=argv[position_o_rencontre+1];
			if(!verifier_output_name(output_name))
			{
				printf("attention: il est conseille de mettre l'extension '%s' a votre fichier de sortie.\n",output_ext);
				printf("Continuer tout de meme ? (o/n) ");
				scanf("%c",&c);
				if(c!='o' && c!='O')
				{
					if(argv[position_o_rencontre+1] != output_name)
						free(output_name);
					output_name=NULL;
					return EXIT_FAILURE;
				}
				while(scanf("%*c")); // vidage buffer
			}
		}
	}
	
	
	/// On ouvre, convertit en sequence d'instructions, puis ferme le fichier d'entree
	
	f_input = fopen(input_name, "r");
	i=file2seq (f_input, &seq);
	fclose(f_input);
	f_input=NULL;
	
	if(i==FICHIER_BINAIRE)
	{
		fprintf(stderr,"erreur: le fichier source semble etre un fichier binaire. Il est aussi peut-etre vide.\n");
	}
	else
	if(i==PB_ALLOCATION)
	{
		fprintf(stderr,"erreur: probleme d'allocation d'une ligne du fichier.\n");
	}
	else
	if(i==AUCUNE_INSTRUCTION)
	{
		fprintf(stderr,"erreur: Votre programme ne contient aucune instruction.\n");
	}
	else
	if(i>0) // i = nombre d'erreur
	{
		fprintf(stderr,"erreur: %d instruction(s) defaillante(s).\n",i);
	}
	else
	{
		
		/// Si le fichier de sortie existe, on demande si on veux l'ecraser ou non (si non, on quitte)
		
		f_output = fopen(output_name, "rb");
		if(f_output!=NULL)
		{ // Il existe !
			fclose(f_output);
			f_output = NULL;
			printf("Voulez-vous ecraser le fichier de sortie '%s' ? (o/n) ",output_name);
			scanf("%c",&c);
			if(c!='o' && c!='O')
			{
				if(argv[position_o_rencontre+1] != output_name)
					free(output_name);
				output_name=NULL;
				return EXIT_FAILURE;
			}
			while(scanf("%*c")); // vidage buffer
		}
		
		
		/// Maintenant, on s'apprete a ecrire le fichier de sortie
		
		f_output = fopen(output_name, "wb");
		if(f_output==NULL)
		{ // Fichier de sortie non cree
			fprintf(stderr,"Impossible de creer le fichier de sortie '%s'\n",output_name);
		}
		else
		{
			fseek(f_output,0,SEEK_SET);
			fwrite(seq.seq,sizeof(instruction),seq.size,f_output);
			fclose(f_output);
			f_output = NULL;
			
			printf("La compilation a reussi avec succes.\n");
			printf("Fichier de sortie: '%s'\n",output_name);
			
		}
		
	}
	
	
	return EXIT_SUCCESS;
}
