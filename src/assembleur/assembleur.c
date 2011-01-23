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
 * @file assembleur.c
 * @brief 
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

// Contient toute les fonctions pour le module assembleur


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#include"assembleur.h"
#include"../verificateur.h"
#include"../general.h"


#define IGNORER_LIGNE -10
#define CODE_INVALIDE -1


#define INTERVALLE_ALLOC_LIGNE 64


// constantes externes

const char * valid_mnemonique[TOTAL_MNEMONIQUE] = 
{"DAT","MOV","ADD","SUB","JMP","JMZ","JMN","DJN","CMP"};

const char * input_ext = "cws";
const char * output_ext = "cwb";

// variable externe, pour les affichages consoles durant la compilation
struct instruction_env_info ins_env_info;



// Prototypes des fonctions internes

int strcmp_sans_casse (const char * s1, const char * s2 );
int string2instruction (char * s, instruction * ins);


// Fonctions utilisees par le main

int file2seq (FILE * f_input, sequence * seq)
{
	int err, res_conv, num_instruction, alloc_actuel;
	int i;
	char c;
	char * ligne;
	instruction tmpins;	
	
	
	alloc_actuel=INTERVALLE_ALLOC_LIGNE;
	
	ligne = (char*)malloc((INTERVALLE_ALLOC_LIGNE+1)* sizeof(char));
	
	if(ligne==NULL)
		return PB_ALLOCATION;
	
	err = 0;
	
	ins_env_info.ligne_actuelle = 1;
	
	
	if(fichier_problablement_binaire(f_input))
		return FICHIER_BINAIRE;

	
	fseek(f_input, 0, SEEK_SET);
	num_instruction=0;
	while(!feof(f_input) && num_instruction<LIMIT_TAILLE_PROG)
	{ // Boucle qui parcours chaque ligne du fichier source
		
		
		
		/// On commence par transferer une ligne du fichier dans la chaine ligne
		
		// Enregistrement du premier caractere
		i=0;
		fread(&c,sizeof c,1,f_input);
		ligne[i]=c;
		while( c!='\n' && c!='\r' && !feof(f_input))
		{ // Boucle qui parcours les caracteres d'une ligne			

			i++;
			
			if(i>alloc_actuel)
			{ // Reallocation tout les INTERVALLE_ALLOC_LIGNE caracteres de la ligne
				ligne=(char*)realloc(ligne,alloc_actuel+INTERVALLE_ALLOC_LIGNE+1);
				if(ligne==NULL)
					return PB_ALLOCATION;
				alloc_actuel+=INTERVALLE_ALLOC_LIGNE;
			}
			
			// Enregistrement du caractere actuel
			fread(&c,sizeof c,1,f_input);
			ligne[i]=c;
		}		
		ligne[i+1]='\0'; // On met le caractere de fin de ligne
		
		
		/// On converti la ligne en instruction, avec gestion (affichage) des erreurs
		res_conv=string2instruction(ligne,&tmpins);
		
		
		/// On ajoute l'instruction de cette ligne a la sequence si il n'y a aucune erreur
		
		if(res_conv==0)
		{ // Si aucune erreur, ajout de l'instruction a la sequence
			seq->seq[num_instruction]=tmpins;
			num_instruction ++;
		}
		else
		if(res_conv!=IGNORER_LIGNE)
			err++; // Si c'est une erreur d'instruction, on la compte
		
		ins_env_info.ligne_actuelle ++; // le numero de la ligne actuelle est incremente, meme si la ligne est ignoree
	}
	// Fin de lecture du fichier
	
	if(num_instruction==0 && !err)
	{
		err=AUCUNE_INSTRUCTION;
	}
	else
	if(num_instruction>=LIMIT_TAILLE_PROG)
		fprintf(stderr,"attention: Votre programme depasse la limite d'instructions autorisees (%d).\n",LIMIT_TAILLE_PROG);
	
	// On enregistre le nombre d'instruction dans la sequence
	seq->size=num_instruction;
	
	free(ligne);
	return err;
}

bool verifier_output_name (char * outputname)
{
	bool ret;
	char * tmp_string = NULL;
	tmp_string= strrchr(outputname,'.');
	
	//printf("debug: %s\n",tmp_string);
	if(tmp_string==NULL || strcmp(tmp_string,".cwb")!=0)
	{
		ret=FAUX;
	}
	else ret=VRAI;
	return ret;
}

char * generate_output_name (const char * input_name)
{ // Genere un fichier de sortie base sur le meme nom que le fichier d'entree
	
	int i, slen;
	char * tmp = NULL;
	char * nom = NULL; // nom du fichier sans l'arborescence
	char * ext = NULL;
	
	char * output_name = NULL;
	
	if(input_name == NULL)
		return NULL;
	
	nom = strrchr (input_name, '/');
	if(nom==NULL)
	{
		// Il n'y a pas de / donc le nom de fichier est le nom direct
		nom=(char*)input_name;
	}
	else
	{
		// Il y a l'arborescence, nom pointe vers le nom du fichier
		nom ++;
		if(*nom == '\0')
		{
			fprintf(stderr, "erreur: Le lien designe est un dossier et pas un fichier source CoreWar.\n");
			return NULL;
		}
	}
	
	tmp = nom+1;
	
	while(*tmp!='\0')
	{
		tmp ++;
		if(*tmp == '.' && *(tmp+1)!='\0')
			ext = tmp + 1;
	}
	
	if(ext!=NULL)
	{
		if(strcmp(ext,input_ext)==0)
		{
			// C'est un fichier avec la bonne extension ! rien a faire
			
		}
		else
		{
			if(strcmp(ext,output_ext)==0)
			{
				// echec parce que l'extension de sortie ne doit pas etre utilise en extension d'entree
				fprintf(stderr, "erreur: L'extension d'un fichier de sortie '%s' ne doit pas etre utilise pour un fichier d'entree '%s'.\n",output_ext,input_ext);
				return NULL;
			}
			else 
			{
				// extension inconnue : va a la fin
				fprintf(stderr, "attention: Le fichier d'entree n'a pas la bonne extension. Pensez a lui mettre l'extension '%s'.\n",input_ext);
				
				while(*ext!='\0')
					ext ++;
			}
			
		}
		
	}
	else
	{
		// le fichier est sans extension
		fprintf(stderr, "attention: Le fichier d'entree n'a pas d'extension. Pensez a lui ajouter l'extension '%s'.\n",input_ext);
		
		ext=nom;
		while(*ext!='\0')
			ext ++;
		
	}
	
	i=(ext-input_name);

	if(*ext == '\0')
		i++;
	
	slen=strlen(output_ext);
	
	
	output_name = malloc ( sizeof(char) * ( i + slen + 1) );
	
	if(output_name==NULL)
	{
		fprintf(stderr, "erreur: l'allocation du nom du fichier de sortie a pose probleme.\n");
		return NULL;
	}

	strncpy (output_name, input_name, i);
	
	
	ext = output_name + (i-1); // ext pointe maintenant sur notre chaine de sortie
	
	*ext = '.'; // point avant l'extension

	ext++;
	
	
	strncpy(ext,output_ext,slen);
	
	ext[slen] = '\0';
	

	return output_name;
}


/// Les fonctions d'assemblage

int arg2code_modearg (char * str)
{ // Recupere le mode d'un argument, Verifie par la meme occasion l'argument

	int code=CODE_INVALIDE;
	
	if(*str == '#')
	{
		code= 0;
		str++;
	}
	else if(*str == '@')
	{
		code= 2;
		str++;
	}
	else
		code= 1;
	
	/* Verification de bien trouver un chiffre apres le mode */
	if( !isdigit(*str) && (*str != '-' || !isdigit(*(str+1)) ) )
		code=CODE_INVALIDE;
	
	return code;
}

int arg2int (char * str)
{ // Recupere la valeur d'un argument (outre son mode)
	char *p, *q;
	int val=0;
	int negatif=FAUX; // booleen
	
	p=str;
	
	if(!isdigit(*p))
	{
		p++;
		if(!isdigit(*p))
			p++;
		if( *(p-1) == '-' )
			negatif=VRAI;
	}
	q=p;
	
	while(isdigit(*q))
		q++;
	*q = '\0';
	
	val=atoi(p);

		
	if(negatif)
		val = -val;	
		
	if(val>=TAILLE_MEM || val<=-TAILLE_MEM)
		fprintf(stderr, "%s:%d: attention: l'entier %d depasse la limite representable sur 12 bits. Votre entier sera considere modulo %d.\n",
		ins_env_info.filename,ins_env_info.ligne_actuelle,val,TAILLE_MEM);
		
	return val;
}

int mnemonique2int (char * str)
{ // Recupere le codage du mnemonique
	int i;
	for(i=0; i<TOTAL_MNEMONIQUE; i++)
		if(strcmp_sans_casse(valid_mnemonique[i], str) == 0)
			return i;
	
	return CODE_INVALIDE;
}

int erreur_sur_fin_instruction (char s)
{ // VRAI si le caractere 's' indique une fin d'instruction (soit debut de commentaire, soit '\0')
	if(s=='\0' || s==';')
	{
		fprintf(stderr, "%s:%d: erreur: il manque des arguments\n",
		ins_env_info.filename,ins_env_info.ligne_actuelle);
		return 1;
	}
	else return 0;
}


int string2instruction (char * s, instruction * ins)
{ // Transforme une chaine en une instruction
	
	int err=0;
	
	//char * str = NULL;
	char * p;
	char * q;
	char back;
	
	int mnem=0;
	int mode1=0;
	int arg1=0;
	int mode2=0;
	int arg2=0;
 

	p=s;
	while(isspace(*p))
		p++;
	
	if(*p==';' || *p=='\0')
		return IGNORER_LIGNE;
	
	
	q=p+1;
	while(isalpha(*q))
		q++;

	
	// q n'est plus une lettre donc on rempli par un '\0' mais on sauvegarde son ancienne valeur pour la restituer ensuite
	back = *q;
	*q = '\0';
	
	mnem=mnemonique2int(p);
	
	
	
	
	if(mnem == CODE_INVALIDE)
	{
		fprintf(stderr, "%s:%d: erreur: Instruction incorrecte '%s'\n",
				ins_env_info.filename,ins_env_info.ligne_actuelle,p);
		err++;
	}
	else
	{
		
		*q = back;
		
		
		if(mnem!=DAT) // Si ce n'est pas DAT
		{
			// ARGUMENT 1
			
			if(erreur_sur_fin_instruction(*q))
				return ++err;
			
			p=q+1;
			while(isspace(*p))
				p++;
			
			if(erreur_sur_fin_instruction(*p))
				return ++err;
			
			q=p+1;
			while(!isspace(*q) && *q!=';' && *q!='\0')
				q++;
			
			
			back = *q;
			*q = '\0';
			
			mode1=arg2code_modearg(p);
			
			
			if(mode1 == CODE_INVALIDE)
			{
				fprintf(stderr, "%s:%d: erreur: argument1 incorrecte '%s'\n",
						ins_env_info.filename,ins_env_info.ligne_actuelle,p);
				err++;
			}
			else
			{
				arg1=arg2int(p);
			}
			
			*q = back;
		}
		
		if(mnem!=JMP) // Si ce n'est pas JMP
		{
			// ARGUMENT 2
			if(erreur_sur_fin_instruction(*p))
				return ++err;
			
			p=q;
			while(isspace(*p))
				p++;
				
			if(erreur_sur_fin_instruction(*p))
				return ++err;
				
			q=p;
			while(!isspace(*q) && *q!=';' && *q!='\0')
				q++;
			
			
			back=*q;
			*q='\0';
			
			mode2=arg2code_modearg(p);
			
			
			if(mode2 == CODE_INVALIDE)
			{
				fprintf(stderr, "%s:%d: erreur: argument2 incorrecte '%s'\n",
						ins_env_info.filename,ins_env_info.ligne_actuelle,p);
				err++;
			}
			else
			{
				arg2=arg2int(p);
			}
			
			*q=back;
		}
		
		while(isspace(*q))
			q++;
		
		if(*q != '\0' && *q !=';')
			fprintf(stderr, "%s:%d: attention: le reste de la ligne a ete ignore.\n",
				ins_env_info.filename,ins_env_info.ligne_actuelle);
	}
	
	*ins = instruction_create (mnem,mode1,mode2,arg1,arg2);
	
	return err;
}


// Divers
int strcmp_sans_casse (const char * s1, const char * s2 )
{ // Adaptation et modification (au niveau du test) du code source de strcmp
	while (*s1 == *s2
		|| (islower(*s1) && *s2 == (*s1+'A'-'a'))
		|| (isupper(*s1) && *s2 == (*s1+'a'-'A')) )
	{
		s2 ++;
		if (*s1 == 0)
			return 0;
		s1 ++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}
