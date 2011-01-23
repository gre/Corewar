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
 * @file listes.c
 * @brief 
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */
#include<stdio.h>
#include<stdlib.h>

#include"../general.h"
#include"listes.h"
#include"machine_virtuelle.h"

/// liste_combat : Liste des combats restants dans un tournoi

void push_combat ( liste_combat * l, combattant * c0, combattant * c1 )
{
	liste_combat p;
	
	p=malloc(sizeof(chainon_combat));
	
	p -> before = NULL;
	p ->  next  = *l;
	
	if(*l != NULL)
		(*l)->before = p;
	
	p -> cbt[0] = c0;
	p -> cbt[1] = c1;
	p -> score[0] = 0;
	p -> score[1] = 0;
	
	*l = p;
}
chainon_combat pop_combat (liste_combat *l)
{
	liste_combat   p = *l;
	chainon_combat c = *p;
	(*l) = p->next;
	free(p);
	p = (*l);
	if(p!=NULL)
		p -> before = NULL;
	return c;
}
bool combat_empty (liste_combat l)
{
	return (l==NULL);
}

chainon_combat top_combat (liste_combat l)
{
	return *l;
}

bool avancer_combat (liste_combat *l)
{
	if(*l != NULL)
	{
		*l = (*l)->next;
		return VRAI;
	}
	else
		return FAUX;
}

chainon_combat * offset_combat (liste_combat l, int offset)
{
	if (offset!=0)
	{
		if(offset>0)
			while (!combat_empty (l) && offset)
			{
				offset --;
				l = l -> next;
			}
		
		else
			while (!combat_empty (l) && offset)
			{
				offset ++;
				l = l -> before;
			}
	}
	
	if(offset)
		return NULL;
	else
		return l; 
}


int count_combat_restant (liste_combat l)
{
	int i=0;
	while(l!=NULL)
	{
		l=l->next;
		i++;
	}
	return i;
}
