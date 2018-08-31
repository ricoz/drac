/*
	filename:		CardGraphics.h
	author:			Rico Roberto Zuñiga
	last modified:	9/12/03

	description:	Interface for card drawing subs
*/

#ifndef CARDGRAPHICS_H
#define CARDGRAPHICS_H

#include "sdl/sdl.h"
#include "../includes/CCard.h"

const unsigned int CRD_NSYMBOL	=	0;
const unsigned int CRD_XSYMBOL	=	1;
const unsigned int CRD_OSYMBOL	=	2;
const unsigned int CRD_HSYMBOL	=	3;

int InitDeck(SDL_Surface *s);

int DrawCard(int x, int y, int nCdIndex);
int DrawCard(int x, int y, int nCdIndex, SDL_Surface *s);

int DrawCardBack(int x, int y);
int DrawCardBack(int x, int y, SDL_Surface *s);

int DrawSymbol(int x, int y, int nSymbol);
int DrawSymbol(int x, int y, int nSymbol, SDL_Surface *s);

/*
int AnimateCard(CCard card);
int AnimateCard(CCard card, SDL_Surface *s);
*/
int AnimateCards(); //might take seconds as parameter

#endif //CARDGRAPHICS_H
