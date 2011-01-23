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
 * @file verificateur.c
 * @brief various verification
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */
// Verifications diverses

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

#include"general.h"
#include"verificateur.h"


bool instruction_correcte (instruction ins)
{
	int i = instruction2ins(ins);
	int m1 = instruction2mode1(ins);
	int m2 = instruction2mode2(ins);
	return ( (i==DAT
		||  i==MOV
		||  i==ADD
		||  i==SUB
		||  i==JMP
		||  i==JMZ
		||  i==JMN
		||  i==DJN
		||  i==CMP) 
	&& (m1==0 || m1==1 || m1==2) 
	&& (m2==0 || m2==1 || m2==2) 
	);
}


bool instruction_est_executable (instruction ins)
{	
	int i = instruction2ins(ins);
	return (i==MOV
		||  i==ADD
		||  i==SUB
		||  i==JMP
		||  i==JMZ
		||  i==JMN
		||  i==DJN
		||  i==CMP);
}

// quota de d'acceptation de caractere non imprimable et non blanc en pourcentage
#define QUOTA_PROBABILITE_BIN 20

bool fichier_problablement_binaire (FILE * f)
{
	int i, n;
	char c;
	
	fseek(f, 0, SEEK_SET);
	for(i=0,n=0;i<100 && !feof(f);i++)
	{ // On regarde les 100 caracteres, si on trouve des caracteres non imprimable, on compte 
		fread(&c,sizeof c,1,f);
		if(!isprint(c) && !isspace(c))
			n++;
	}
	
	if( 100 * n > QUOTA_PROBABILITE_BIN * i )
		return 1;
	else return 0;
}
