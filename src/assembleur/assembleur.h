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
 * @file assembleur.h
 * @brief 
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */

#ifndef ASSEMBLEUR_H
#define ASSEMBLEUR_H

#include<stdio.h>

#include"../general.h"

#define AUCUNE_INSTRUCTION -1
#define PB_ALLOCATION -2
#define FICHIER_BINAIRE -3


struct instruction_env_info {
	char * filename;
	int ligne_actuelle;
};

extern struct instruction_env_info ins_env_info;

extern const char * valid_mnemonique[TOTAL_MNEMONIQUE];

extern const char * output_ext;


int file2seq (FILE * f_input, sequence * seq);

char * generate_output_name (const char * input_name);

bool verifier_output_name (char * outputname);

#endif
