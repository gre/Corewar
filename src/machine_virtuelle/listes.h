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
 * @file listes.h
 * @brief 
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

#ifndef LISTES_H
#define LISTES_H


#include"../general.h"
#include"machine_virtuelle.h"


struct chainon_combat {
	
	combattant * cbt[2];
	int score[2];
	
	struct chainon_combat * before;
	struct chainon_combat * next;
};
typedef struct chainon_combat chainon_combat;
typedef chainon_combat * liste_combat;

void push_combat ( liste_combat * l, combattant * c0, combattant * c1 );
chainon_combat pop_combat (liste_combat *l);
bool combat_empty (liste_combat l);
chainon_combat top_combat (liste_combat l);

bool avancer_combat (liste_combat *l);
chainon_combat * offset_combat (liste_combat l, int offset);
int count_combat_restant (liste_combat l);

#endif
