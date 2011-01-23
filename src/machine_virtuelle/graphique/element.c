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
 * @file element.c
 * @brief 
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

// Ensemble des fonctions qui gere les elements graphiques et aussi les structures 'rectangle'
#include<stdio.h>
#include<stdlib.h>

#include"../../general.h"
#include"../../graph.h"
#include"element.h"


liste_element push_element (liste_element l, int el, rectangle rect)
{
	if(el==0) // Un element ne peux pas etre nul
		return NULL;
	
	liste_element nl=malloc(sizeof(struct chainon_element));
	if(nl==NULL)
		return NULL;
	nl->el=el;
	nl->rect=rect;
	nl->next=l;
	return nl;
}

int pop_element (liste_element * l)
{ // On suppose avoir verifie que la liste n'est pas vide
	int el;
	liste_element nl = NULL;
	el = (*l)->el;
	nl = (*l)->next;
	free(*l);
	*l=nl;
	return el;
}

bool liste_vide (liste_element l)
{
	return ( l == NULL );
}

void free_liste_element (liste_element * l)
{
	while( ! liste_vide(*l) )
		pop_element(l);
}

bool element_existe (liste_element l, int num_el)
{
	liste_element p;
	p=l;
	if(p==NULL)
		return FAUX;
	while(p!=NULL)
	{
		if(p->el == num_el)
			return VRAI;
		p=p->next;
	}
	return FAUX;
}



rectangle affecter_rectangle (short x, short y, short w, short h)
{
	rectangle rect;
	rect.x=x;
	rect.y=y;
	rect.w=w;
	rect.h=h;
	return rect;
}

rectangle obtenir_rectangle_element (liste_element l, int num_el)
{ // On suppose avoir verifier l'existance de l'element
	rectangle rect;
	liste_element p;
	p=l;
	while(p->el!=num_el)
		p=p->next;
	rect=p->rect;
	return rect;
}


rectangle rectangle_intersection(rectangle r1, rectangle r2) {
	
	rectangle itsc;
	itsc.x = (r1.x > r2.x)? r1.x : r2.x;
	itsc.y = (r1.y > r2.y)? r1.y : r2.y;

	itsc.w = ((r1.x + r1.w) < (r2.x + r2.w))? r1.x + r1.w - itsc.x: r2.x + r2.w - itsc.x;
	itsc.h = ((r1.y + r1.h) < (r2.y + r2.h))? r1.y + r1.h - itsc.y: r2.y + r2.h - itsc.y;

	if (itsc.w < 0 || itsc.h < 0) {
		itsc.x = -1; itsc.y = -1;
		itsc.w = 0; itsc.h = 0;
	}
	
	return itsc;
}
bool rectangle_superposition (rectangle r1, rectangle r2)
{
	rectangle inter = rectangle_intersection (r1,r2);
	return inter.w && inter.h;
}

#ifdef DEBUG

void DEBUG_rectangle (rectangle r)
{
	printf("pos:(%d , %d), dim:(%d , %d)",r.x, r.y, r.w, r.h);
}

void DEBUG_element (struct chainon_element cel)
{
	printf("el: %d, ",cel.el); 
	DEBUG_rectangle(cel.rect);
	putchar('\n');
}

void DEBUG_liste_element (liste_element lel)
{
	while(!liste_vide(lel))
	{
		DEBUG_element (*lel);
		lel = lel -> next;
	}
}
#endif
