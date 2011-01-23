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
 * @file general.h
 * @brief constant and types used by several project module
 * @author Gaetan Renaudeau < contact@grenlibre.fr >
 */


#ifndef GENERAL_H
#define GENERAL_H

#define MODE_GRAPHIQUE 1

// Taille de la memoire
#define TAILLE_MEM 4096

// Limite de la taille d'une sequence d'un programme
#define LIMIT_TAILLE_PROG 1024

typedef int instruction; // au lieu de la structure

/* On defini le type sequence : c'est une suite d'instruction (sequence)
 * Ce type est lie a un programme */
struct sequence {
	instruction seq[LIMIT_TAILLE_PROG]; // Sequence d'instructions binaires
	int size; // Taille de la sequence
};
typedef struct sequence sequence;

// On defini les mnemonique pour retrouver facilement leurs valeurs
enum MNEMONIQUE {DAT,MOV,ADD,SUB,JMP,JMZ,JMN,DJN,CMP,TOTAL_MNEMONIQUE};

// On defini bool : soit VRAI, soit FAUX
enum bool {FAUX=0, VRAI=1};
typedef enum bool bool;


/// Macros tres utilisees (decalages et des masques pour recuperer des informations sur instruction)

#define instruction2ins(i) ((i>>28)&0xF)
#define instruction2mode1(i) ((i>>26)&0x3)
#define instruction2mode2(i) ((i>>24)&0x3)
#define instruction2arg1(i) ( (i&0x800000) ? (((i>>12)&0xFFF)-TAILLE_MEM) : ((i>>12)&0xFFF) ) // afin d'obtenir l'argument en entier signe
#define instruction2arg2(i) ( (i&0x800) ? ((i&0xFFF)-TAILLE_MEM) : (i&0xFFF) ) // afin d'obtenir l'argument en entier signe

#define instruction_create(ins,mode1,mode2,arg1,arg2) ( (ins<<28) | ((mode1&0x3)<<26) | ((mode2&0x3)<<24) | ((arg1&0xFFF)<<12) | (arg2&0xFFF) )


#endif
