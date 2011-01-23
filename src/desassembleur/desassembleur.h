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
 * @file desassembleur.h
 * @brief 
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

#ifndef DESASSEMBLEUR_H
#define DESASSEMBLEUR_H

#include"../general.h"

#include<stdio.h>

char * ins2string  (instruction ins);
int afficher_instruction (instruction ins);
int decompiler (FILE * f_input);

#endif
