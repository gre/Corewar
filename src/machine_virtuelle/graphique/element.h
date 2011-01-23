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
 * @file element.h
 * @brief 
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

struct rectangle {
	short x; // x position
	short y; // y position
	short w; // width : largeur
	short h; // height : hauteur
};
typedef struct rectangle rectangle;

struct chainon_element {
	rectangle rect;
	int el; // id de l'element
	struct chainon_element * next;
};

typedef struct chainon_element * liste_element;


liste_element push_element (liste_element l, int el, rectangle rect);
bool element_existe (liste_element l, int num_el);
void free_liste_element (liste_element * l);

rectangle affecter_rectangle (short x, short y, short w, short h);
rectangle obtenir_rectangle_element (liste_element l, int num_el);
bool rectangle_superposition (rectangle r1, rectangle r2);

#ifdef DEBUG
void DEBUG_rectangle (rectangle r);
void DEBUG_element (struct chainon_element cel);
void DEBUG_liste_element (liste_element lel);
#endif
