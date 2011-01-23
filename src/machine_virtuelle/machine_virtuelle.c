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
 * @file machine_virtuelle.c
 * @brief constant and types used by virtual machine
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

// Contient toutes les fonctions liees a la machine virtuelle mais detachees du mode d'affichage (console / graphique)
// Contient egalement le main commun a la console et au graphique


#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include<string.h>


#include"machine_virtuelle.h"
#include"../general.h"
#include"../verificateur.h"

#include"listes.h"

#ifdef MODE_GRAPHIQUE
#include"graphique/graphique.h"
#endif

#ifdef MODE_CONSOLE
#include"console/console.h"
#endif


#define TAILLE_NOM_COMBATTANT 16



struct environnement env;


// Un tournoi : ensemble de combat entre tous les participants
int proceder_tournoi (combattant * , int nb_combattants);

// Un combat : Un certain nombre de rounds (NB_ROUND)
int proceder_combat (combattant *, combattant *);

// Un round : un match entre deux programmes
int proceder_round (combattant *, combattant *);

int proceder_instruction (int num_programme);

/// Traitements fichiers, sequences

int f_prog2seq (FILE * f_prog, sequence * seq);
void placer_2_seq_en_memoire (sequence seq0, sequence seq1);
void own_case_memoire (int num_combattant, enum type_own type, int num_case);


/// Fonctions pour executer une instruction
// Execute une instruction a l'adresse ip, en gérant sa modification
void execute_instruction (int * ip, int num_prog);
void inc_IP (int *ip);
// Les operations
void Mov (int op1, int adr2);
void Add (int op1, int adr2);
void Sub (int op1, int adr2);
void Jmp (int adr1, int * ip);
void Jmz (int adr1, int op2, int * ip);
void Jmn (int adr1, int op2, int * ip);
void Djn (int adr1, int adr2, int * ip);
void Cmp (int op1, int op2, int * ip);

/// divers
char * creer_nom_combattant (char * src);
void calcul_positions (combattant * cbt, int nb_combattants);

#ifdef DEBUG
void DEBUG_env (void);
#endif


int main (int argc, char * argv[])
{
	//test();
	
		
	FILE ** f_prog = NULL; // un tableau de pointeur sur les fichiers des programmes combattants
	combattant * cbt = NULL; // un tableau des sequences des combattants
	int i;
	int erreur; // booleen
	int nb_combattants;
	
	
	/* Filtrage selon le nombre de combattants */
	
	if(argc==1 || argc>MAX_COMBATTANTS+1)
	{ // On lance le module d'aide
		printf("Usage:\n");
		printf("\t%s fichier1 [fichier2 ... ]\n\n",argv[0]);
		printf("Le programme 'corewar' consiste a simuler un tournoi de combat de programmes dans une memoire finie.\n");
		printf("Un combat est l'affrontement de deux combattants sur %d rounds.\n",NB_ROUND);
		printf("Les fichiers binaires passees en lignes de commande sont des fichiers assembles du langage CoreWar (version du microprojet).\n\n");
		printf("Si un seul programme est donne, on cree un doublon pour l'affronter. Si plus de deux programmes sont passes, on creera un processus de tournoi avec plusieurs combats.\n\n");
		printf("Exemples :\n");
		printf("\t%s prog1.cwb prog2.cwb\n",argv[0]);
		printf("\t%s le_bon.cwb la_brute.cwb le_truand.cwb\n",argv[0]);
		printf("\t%s ./cw/*.cwb\n",argv[0]);
		printf("\t%s solo.cwb\n\n",argv[0]);
		printf("Supporte %d combattants au maximum.\n",MAX_COMBATTANTS);
	}
	else
	{
		/* On ouvre tous les fichiers. Si l'ouverture conduit a une erreur, on indique l'erreur,
		 * on continue a ouvrir tous les fichiers mais on quittera apres cette operation.*/
		
		f_prog = malloc((argc-1)*sizeof(FILE *)); // Allocation du tableau de fichiers
		erreur=FAUX;
		for(i=0;i<argc-1;i++)
		{
			f_prog[i] = fopen(argv[i+1],"rb");
			if(f_prog[i]==NULL)
			{
				printf("%s: Ouverture impossible!\n",argv[i+1]);
				erreur=VRAI;
			}
		}
		if(erreur)
			return EXIT_FAILURE;
		
		
		
		/* On decode chaque fichier ouvert et on enregistre la sequence du programme 
		 * L'espace alloue pour le fichier est libere une fois sa sequence enregistree */
		
		cbt = malloc(argc*sizeof(combattant)); // Allocation du tableau de combattants (une seq de reserve pour le cas argc=2)
		if(cbt==NULL)
		{
			fprintf(stderr,"Probleme a l'allocation des combattants");
			return EXIT_FAILURE;
		}
		
		
		for(i=0;i<argc-1;i++)
		{
			f_prog2seq (f_prog[i],&cbt[i].seq);
			cbt[i].nom=creer_nom_combattant (argv[i+1]);
			cbt[i].num=i;
			cbt[i].place=1;
			cbt[i].point=0;
			fclose(f_prog[i]);
			f_prog[i]=NULL;
		}
		
		free(f_prog);
		
		nb_combattants=argc-1;
		
		
		if(argc==2)
		{ // Dans le cas ou l'on a un seul combattant, on cree un clone pour que les deux s'affrontent
			cbt[1]=cbt[0];
			cbt[1].nom=creer_nom_combattant(argv[1]);
			cbt[i].num=1;
			nb_combattants ++;
		}
		
		srand(time(NULL));
		
		
		#ifdef MODE_GRAPHIQUE
		if(affichage_initialiser()==PROBLEME_AFFICHAGE)
		{
			fprintf(stderr,"Erreur a l'initialisation du mode graphique\n");
			return EXIT_FAILURE;
		}
		#endif
		
		if(affichage_avant_tournoi(cbt,nb_combattants) != QUITTER_AFFICHAGE)
		{
			if(proceder_tournoi (cbt,nb_combattants) != QUITTER_AFFICHAGE)
				affichage_apres_tournoi(cbt,nb_combattants);
		}
		
		#ifdef MODE_GRAPHIQUE
		affichage_fermer();
		#endif
		
		
		for(i=0;i<nb_combattants;i++)
			free(cbt[i].nom);
		free(cbt);
	
	}
	
	return EXIT_SUCCESS;
}


int proceder_tournoi (combattant * cbt, int nb_combattants)
{
	bool quitter=FAUX;
	
	int i,j;
	int res;
	
	liste_combat l = NULL;
	liste_combat p;
	chainon_combat cb;
	
	
	env.waitingtime = DEFAUT_waitingtime;
	env.mode = MODE_NORMAL;
	
	
	for(i=1;i<nb_combattants;i++)
		for(j=0;j<i;j++)
			push_combat (&l, &cbt[i], &cbt[j]);
	
	p=l;
	while(!combat_empty(p) && !quitter)
	{
		
		
		cb = top_combat (p);
		
		env.nom[0]=(cb.cbt[0])->nom;
		env.nom[1]=(cb.cbt[1])->nom;
		
		if(affichage_avant_combat(p,cbt,nb_combattants)==QUITTER_AFFICHAGE)
			quitter=VRAI;
		else
		{
			/* Lancement du combat entre deux programmes */
			res=proceder_combat (cb.cbt[0],cb.cbt[1]);
			
			if(res==QUITTER_AFFICHAGE)
				quitter=VRAI;
			else
			{
				i=(cb.cbt[0])->num;
				j=(cb.cbt[1])->num;
				
				p->score[0] = env.nb_gagne[0];
				p->score[1] = env.nb_gagne[1];
				
				cbt[i].point += env.nb_gagne[0];
				cbt[j].point += env.nb_gagne[1];
				
				/* On recalcule la place */
				calcul_positions (cbt, nb_combattants);
			
			
				if(affichage_apres_combat(p)==QUITTER_AFFICHAGE)
					quitter=VRAI;
			
				avancer_combat(&p);
			}
		}
	}
	
	while(!combat_empty(l))
		pop_combat(&l);
	
	if(quitter)
		return QUITTER_AFFICHAGE;
	else
		return 0;
}

int proceder_combat (combattant *cbt0, combattant *cbt1)
{// retour -1 si match nul, sinon 0 ou 1 selon le programme gagnant
	int res;
	int finalres;
	int i;
	bool quitter=FAUX;
	
	
	env.nb_gagne[0] =0;
	env.nb_gagne[1] =0;
	env.nb_match_nul=0;
	

	
	env.nom[0] = cbt0->nom;
	env.nom[1] = cbt1->nom;
	
	for(i=0;i<NB_ROUND && !quitter;i++)
	{
		env.num_round_actuel=i;
		if(affichage_avant_round()==QUITTER_AFFICHAGE)
			quitter=VRAI;
		else
		{
			res=proceder_round(cbt0, cbt1);
			if(res==QUITTER_AFFICHAGE)
				quitter=VRAI;
			else
			{
				if(res==-1)
					env.nb_match_nul ++;
				else if (res==0 || res==1)
					env.nb_gagne[res] ++;
				
				if(affichage_apres_round(res)==QUITTER_AFFICHAGE)
					quitter=VRAI;
			}
		}
	}
	
	if(env.nb_gagne[0]==env.nb_gagne[1])
		finalres=-1;
	else if(env.nb_gagne[0]>env.nb_gagne[1])
		finalres=0;
	else
		finalres=1;
	
	if(quitter)
		return QUITTER_AFFICHAGE;
	else
		return finalres;
}

int proceder_round (combattant *cbt0, combattant  *cbt1)
{// retour -1 si match nul, sinon 0 ou 1 selon le programme gagnant
	int res=-1;
	
	int r;
	
	int cpt;
	int fin;
	
	int i;
	int prem, deux;
	
	for(i=0;i<TAILLE_MEM;i++)
	{
		env.mem_owner[i]=OWNER_CASE_VIDE; // Initialiser le possesseur d'une zone a vide
		env.mem[i]=0; // Initialiser la zone memoire
		env.need_rebuff[i]=VRAI;
	}
	env.nb_case_owned[0]=0;
	env.nb_case_owned[1]=0;



	placer_2_seq_en_memoire (cbt0->seq,cbt1->seq);
	
	
	// On alterne une fois sur deux le premier programme qui commence
	prem = env.num_round_actuel & 1; // modulo 2
	deux = (prem==0) ? 1 : 0;

	cpt=0;
	
	fin=FAUX;
	while ( !fin && cpt<LIMIT_CYCLE )
	{
		env.current_step=cpt; // pour les statistiques
		
		// Si la premiere instruction rate, proceder_instruction renvoi VRAI et on n'execute pas le deuxieme

		if((r=proceder_instruction (prem)) || (r=proceder_instruction (deux)) )
			fin = VRAI;
		
		cpt++;
	}
	
	if(r==QUITTER_AFFICHAGE)
		return QUITTER_AFFICHAGE;
	
	if(cpt>=LIMIT_CYCLE)
		res=-1;
	else 
	{
		if(instruction_est_executable(env.mem[env.ip[0]]))
			res=0;
		else
			res=1;
	}
	
	return res;
}

int proceder_instruction (int num_programme)
{
		if( instruction_est_executable( env.mem[env.ip[num_programme]] ) )
		{
			if(affichage_avant_execution_instruction(num_programme)==QUITTER_AFFICHAGE)
				return QUITTER_AFFICHAGE;
			execute_instruction(&env.ip[num_programme],num_programme);
			if(affichage_apres_execution_instruction(num_programme)==QUITTER_AFFICHAGE)
				return QUITTER_AFFICHAGE;
			return FAUX;
		}
		else
			return VRAI;
}


/// Traitements fichiers, sequences

int f_prog2seq (FILE * f_prog, sequence * seq)
{
	int l;
	int err=0;
	
	fseek(f_prog,0,SEEK_SET);
		for(l=0;l<LIMIT_TAILLE_PROG && !feof(f_prog);l++)
		{
			fread(&seq->seq[l], sizeof(instruction), 1, f_prog);
		}
		
		seq->size=l-1;
		
		if(!feof(f_prog))
		{
			err++;
			printf("attention: Votre programme a depasse le maximum d'instructions autorisees (%d). Il sera tronque\n",LIMIT_TAILLE_PROG);
		}
	
	return err;
}

void placer_2_seq_en_memoire (sequence seq0, sequence seq1)
{
	int  j;
	int pos[2];

	int p;
	
	int a, b;
	
	pos[0]=rand() MOD_TAILLE_MEM;
	
	a=(pos[0]+seq0.size) MOD_TAILLE_MEM;
	b=(pos[0]-seq1.size) MOD_TAILLE_MEM;
	if(b<a)
		b+=TAILLE_MEM;
	
	pos[1] = (rand()%(b-a) + a)MOD_TAILLE_MEM;
	
	
	/* Anciennement (plus lent, moins equi-reparti)
	pos[1]=rand() MOD_TAILLE_MEM;
	while(abs(pos[0]-pos[1]) < LIMIT_ESPACE_MINI_PROG)
		pos[1]=rand() MOD_TAILLE_MEM;
	*/

	env.ip[0]=pos[0];
	for(j=0;j<seq0.size;j++)
	{
		p=(pos[0]+j)MOD_TAILLE_MEM;
		env.mem[p] = seq0.seq[j];
		own_case_memoire(0,OWNER,p);
	}
	
	env.ip[1]=pos[1];
	for(j=0;j<seq1.size;j++)
	{
		p=(pos[1]+j)MOD_TAILLE_MEM;
		env.mem[p] = seq1.seq[j];
		own_case_memoire(1,OWNER,p);
	}

}


// Acquerir une case memoire
void own_case_memoire (int num_combattant, enum type_own type, int num_case)
{
	// TODO : cet algo est a tester car pas tout a faire correct
	
	int num_c2; // designe l'autre combattant
	num_c2 = ( (num_combattant==0) ? 1 : 0 );
	
	// Si la case appartenait a l'autre combattant, on decremente son compteur de case total
	if( env.mem_owner[num_case] == (OWNER+num_c2)
		|| env.mem_owner[num_case] == (OWNER_BOMB+num_c2)
		|| env.mem_owner[num_case] == (OWNER_VAR+num_c2) )
		env.nb_case_owned[num_c2] --;
	
	// Si la case n'appartenant pas deja au combattant, on increment son compteur de case total
	if( env.mem_owner[num_case] != (OWNER+num_combattant)
		&& env.mem_owner[num_case] != (OWNER_BOMB+num_combattant)
		&& env.mem_owner[num_case] != (OWNER_VAR+num_combattant) )
		env.nb_case_owned[num_combattant] ++;
		
	env.mem_owner[num_case] = type+num_combattant;
	
	//env.num_case_nouvel_own = num_case;
	
	env.need_rebuff[num_case] = VRAI;
}


/// Fonctions pour executer une instruction

// Execute une instruction a l'adresse ip, en gérant sa modification
void execute_instruction (int * ip, int num_prog)
{ 
	
	// num_prog sert ici pour completer les stats sur la domination d'un programme

	instruction ins; // instruction a executer
	int * memoire; // raccourci vers la memoire
	int adr1, adr2; // adresse des operandes pour les adressages non direct
	int p1, p2; // pointeur dans le cas des adressages indirects
	int op1, op2; // les deux operandes
	
	
	memoire=env.mem;
	
	ins = env.mem[*ip];
	//printf("%d ",env.mem[*ip]);
	//DEBUG_ins(ins);

	// Les explications des manipulations selon le mode sont expliquees dans le rapport
	
	int i=instruction2ins(ins);
	int mode1=instruction2mode1(ins);
	int mode2=instruction2mode2(ins);
	int arg1=instruction2arg1(ins);
	int arg2=instruction2arg2(ins);
	
	switch(mode1)
	{
	case 0 :
			adr1= arg1 MOD_TAILLE_MEM;
			op1 = arg1;
			break;
	case 1 :
			adr1 = (*ip + arg1) MOD_TAILLE_MEM;
			op1 = memoire[adr1];
			break;
	case 2 :
			p1 = (*ip + arg1) MOD_TAILLE_MEM;
			adr1 = (p1 + memoire[p1]) MOD_TAILLE_MEM;
			op1 = memoire[adr1];
			break;
	}
	
	switch(mode2)
	{
	case 0 :
			adr2= arg2 MOD_TAILLE_MEM;
			op2 = arg2;
			break;
	case 1 :
			adr2 = (*ip + arg2) MOD_TAILLE_MEM;
			op2 = memoire[adr2];
			break;
	case 2 :
			p2 = (*ip + arg2) MOD_TAILLE_MEM;
			adr2 = (p2 + memoire[p2]) MOD_TAILLE_MEM;
			op2 = memoire[adr2];
			break;
	}

	adr1 = adr1 MOD_TAILLE_MEM;
	adr2 = adr2 MOD_TAILLE_MEM;
	
		
	//printf("DEBUG: op1:%d, op2:%d, adr1:%d, adr2:%d\n",op1,op2,adr1,adr2);
	

	
	// Le programme execute l'instruction donc on considere qu'il la possede
	own_case_memoire(num_prog, OWNER, *ip);
	
	switch(i)
	{
	case DAT :  // NON EXECUTABLE, on n'arrivera pas ici
			//Dat(arg2,*ip); // peu importe le mode
			//inc_IP(ip); 
			break;
	case MOV : 
			Mov(op1,adr2);
			own_case_memoire(num_prog, OWNER_BOMB, adr2); // On considere que faire un Mov est une bombe (souvent oui !)
			inc_IP(ip);
			break;
	case ADD : 
			Add(op1,adr2);
			own_case_memoire(num_prog, OWNER_VAR, adr2); // Ajouter sert generalement pour les variables
			inc_IP(ip);
			break;
	case SUB : 
			Sub(op1,adr2);
			own_case_memoire(num_prog, OWNER_VAR, adr2); // Soustraire sert generalement pour les variables
			inc_IP(ip);
			break;
	case JMP : 
			Jmp(adr1,ip);
			break;
	case JMZ : 
			Jmz(adr1,op2,ip);
			break;
	case JMN : 
			Jmn(adr1,op2,ip);
			break;
	case DJN : 
			Djn(adr1,adr2,ip);
			own_case_memoire(num_prog, OWNER_VAR, adr2); // le fait de soustraire sert generalement pour les variables
			break;
	case CMP : 
			Cmp(op1,op2,ip);
			break;
	}
	
	env.need_rebuff[*ip] = VRAI;
	
}

void inc_IP (int *ip)
{
	*ip=(*ip+1)MOD_TAILLE_MEM;
}

// Les operations


/* // finalement, c'est non executable
void Dat (int arg2, int ip)
{ // Initialise à la valeur B l’adresse actuelle
	env.mem[ip] = arg2;
	#ifdef DEBUG
	printf("DEBUG: Dat met %d sur l'adresse actuelle %d\n",arg2,ip);
	#endif
}*/

void Mov (int op1, int adr2)
{ // Met le contenu de l’adresse A à l’adresse B (B <- A)
	env.mem[adr2] = op1;
	#ifdef DEBUG
	printf("DEBUG: Mov met %d a l'adresse %d\n",op1,adr2);
	#endif
}

void Add (int op1, int adr2)
{ // Additionne le contenu de l’adresse A avec le contenu l’adresse B et 
  // range le resultat a l’adresse B (B <- A+B)
	env.mem[adr2] += op1;
	#ifdef DEBUG
	printf("DEBUG: Add ajoute %d a l'adresse %d\n",op1,adr2);
	#endif
}

void Sub (int op1, int adr2)
{ // Soustrait le contenu de l’adresse A du contenu l’adresse B et 
  // range le resultat a l’adresse B (B = B-A)
	env.mem[adr2] -= op1;
	#ifdef DEBUG
	printf("DEBUG: Sub enleve %d a l'adresse %d\n",op1,adr2);
	#endif
}

void Jmp (int adr1, int * ip)
{ // Continuer a l’adresse A
	*ip = adr1;
}

void Jmz (int adr1, int op2, int * ip)
{ // Si le contenu de l’adresse B contient 0 alors continuer à l’adresse A
	if((op2)==0)
		*ip = adr1;
	else
		inc_IP(ip);
}

void Jmn (int adr1, int op2, int * ip)
{ // Si le contenu de l’adresse B est différent de 0 alors continuer à l’adresse A
	if((op2)!=0)
		*ip = adr1;
	else
		inc_IP(ip);
}

void Djn (int adr1, int adr2, int * ip)
{ // Décrémente le contenu de l’adresse B de 1. 
  // Si le contenu de l’adresse B contient 0 alors aller à l’adresse A

	env.mem[adr2] --;
	if((env.mem[adr2])==0)
		*ip = adr1;
	else
		inc_IP(ip);
	
	//printf("DEBUG: Djn decremente contenu de l'adresse %d qui passe a %d.\n",adr2,env.mem[adr2]);
	
}

void Cmp (int op1, int op2, int * ip)
{ // Si le contenu de l’adresse A est égal au contenu d’adresse B
  // alors ne pas exécuter  la prochaine instruction (et aller à la suivante)
	if((op1)==(op2))
		inc_IP(ip);
	inc_IP(ip);
}


/// divers
char * prog_own_stats2string (int nb_case_owned, int total_case)
{
	if(!total_case) return NULL;
	char * s;
	float f = ((float)(nb_case_owned*100))/((float)total_case);
	int allocsize = 32;
	
	s = malloc(sizeof(char) * allocsize);
	
	snprintf(s,allocsize,"%d o (%.1f%%)",nb_case_owned,f);
	return s;
}


char * creer_nom_combattant (char * src)
{ // il y a une limite de caractere, on retire l'extension si c'est '.cwb' et on remplace les '_' par des ' '
	char * dest = NULL;
	char *q;
	int i,n;
	
	q=strrchr(src,'/');
	if(q!=NULL)
		src=q+1;
	
	i=strlen(src);
	if(i>TAILLE_NOM_COMBATTANT)
		i=TAILLE_NOM_COMBATTANT;
	
	dest = malloc(sizeof(char) * (i+1));
	
	q=strrchr(src,'.');
	if(q!=NULL && strcmp(q,".cwb")==0)
		i-=4;

	strncpy (dest,src,i);
	dest[i]='\0';
	
	// on remplace les '_' par des ' '	
	for(n=0;n<i;n++)
		if(dest[n]=='_')
			dest[n]=' ';
			
	return dest;
}

void calcul_positions (combattant * cbt, int nb_combattants)
{
	int i, j, k;
	bool ex_aequo, a_remonter;
	
	/* Calcul des positions */
	
	for(i=0; i<nb_combattants; i++)
		cbt[i].place = 1; // tous premier par defaut
	
	ex_aequo=FAUX;
	for(i=0; i<nb_combattants; i++)
		for(j=0; j<i; j++)
			if(cbt[i].point > cbt[j].point ) // i bas j
				cbt[j].place ++; // j perd une place
			else if ( cbt[i].point < cbt[j].point )
				cbt[i].place ++; // i perd une place
			else if ( cbt[i].point == cbt[j].point )
				ex_aequo=VRAI;

	if(ex_aequo)
	{ // il y a eu ex aequo, on doit peut-etre faire remonter la position
		for(k=0; k<nb_combattants; k++)
		{ // on fait nb_combattants fois pour remonter plusieurs fois eventuellement
			for(i=0; i<nb_combattants; )
			{
				a_remonter=VRAI;
				if(cbt[i].place == 1)
					a_remonter=FAUX;// on ne peux pas remonter le premier !
				else	
					for(j=0; j<nb_combattants; j++)
						if(cbt[i].place == cbt[j].place+1 && cbt[i].point != cbt[j].point) // si on trouve une position qui suit
							a_remonter=FAUX; // on ne remonte pas dans le classement pour ne pas 'ecraser'
				if(a_remonter)
					cbt[i].place --;
				else 
					i++; // si on a pas a remonter on ne reste pas sur ce joueur.
			}
		}
	}
	
}



#ifdef DEBUG
/// Quelques fonctions de debug
void DEBUG_prog_ip (void)
{
	printf("prog:ip - %s:%d\t%s:%d\n",env.nom[0],env.ip[0],env.nom[1],env.ip[1]);
	
	
}

void DEBUG_own (void)
{
	printf("p#:acquis - p0:%d , p1:%d\n",env.nb_case_owned[0],env.nb_case_owned[1]);
}
#endif
