/*
	filename:		CardGraphics.cpp
	author:			Rico Roberto Zuñiga
	last modified:	8/25/03

	description:	Card drawing subs
*/

#include "../includes/CardGraphics.h"
#include <stdlib.h>
#include <time.h>

int const CARDBACKLOC = 53 * CARDWIDTH; 						//modify to allow custom card backs

SDL_Surface *deck;
SDL_Surface *scr;

SDL_Rect SrcBack;
SDL_Rect SrcCard;

int InitDeck(SDL_Surface *s)									// modify to allow initialization from .ini file
{																// for custom card graphics, custom window background, custom card back
	deck = SDL_LoadBMP("gfx/deck_default.bmp");
	scr = s;

	if(deck == NULL)											// if custom card graphics and file not found, revert to default card graphics
	{
		printf("Unable to load deck.bmp: %s\n", SDL_GetError());
		return -1;
	}

	SrcBack.x = CARDBACKLOC;
	SrcBack.y = 0;
	SrcBack.w = CARDWIDTH;
	SrcBack.h = CARDHEIGHT;
	
	SrcCard.y = 0;
 	SrcCard.w = CARDWIDTH;
 	SrcCard.h = CARDHEIGHT;

	SDL_SetColorKey(deck, SDL_SRCCOLORKEY, SDL_MapRGB(deck->format, 0, 255, 0));	// might as well enable setting own transparent color

	return 0;
}

int DrawCard(int x, int y, int nCdIndex)
{
	DrawCard(x, y, nCdIndex, NULL);
}

int DrawCard(int x, int y, int nCdIndex, SDL_Surface *s)
{
	if(s == NULL)
  		s = scr;

	if(nCdIndex < 0) nCdIndex = 0;
	if(nCdIndex > 51) nCdIndex = 51;

	SrcCard.x = nCdIndex * CARDWIDTH;

	SDL_Rect dest;
	dest.x = x;
	dest.y = y;

	return SDL_BlitSurface(deck, &SrcCard, s, &dest);
}

int DrawCardBack(int x, int y)
{
	DrawCardBack(x, y, NULL);
}

int DrawCardBack(int x, int y, SDL_Surface *s)
{
	if(s == NULL)
  		s = scr;

	SDL_Rect dest;
	dest.x = x;
	dest.y = y;

	return SDL_BlitSurface(deck, &SrcBack, s, &dest);
}

/*
	Symbols:
		0:	Blank, no symbol
		1:	X symbol
		2:	O symbol
		3:	Holder symbol
*/
int DrawSymbol(int x, int y, int nSymbol)
{
	DrawSymbol(x, y, nSymbol, NULL);
}

int DrawSymbol(int x, int y, int nSymbol, SDL_Surface *s)
{
	if(nSymbol < 1) return 0;
	if(nSymbol > 3) nSymbol = 3;

	if(s == NULL)
  		s = scr;

	SrcCard.x = (nSymbol + 53) * CARDWIDTH;

	SDL_Rect dest;
	dest.x = x;
	dest.y = y;

	return SDL_BlitSurface(deck, &SrcCard, s, &dest);
}

/*
x,y = position of card
xspeed = initial horizontal speed
yspeed = 0
gravity = constant of gravity, tweak this to get the effect you want
for every frame
  yspeed = yspeed + gravity
  x = x + xspeed
  y = y + yspeed
  if y > MAXY:
     y = MAXY
     yspeed = -yspeed * 0.8          # tweak this constant to change how high the card bounces
  draw card at (x,y)
*/

// Thanks to Clemens for the pseudocode
/*
int AnimateCard(CCard card)
{
	return AnimateCard(card, NULL);
}
int AnimateCard(CCard card, SDL_Surface *s)
*/
int AnimateCards()
{
	srand((unsigned)time(NULL));

	int rot;
	int id;
	int x;
	int y;
	int xspeed;
	int yspeed;

	int		GRAVITY = 1;
	int		MAXY = scr->h;
	float	BOUNCE = 0.8;

	do
	{
		rot = rand() % 2;
		id = rand() % 51;
		x  = rand() % scr->w;
		y  = rand() % scr->h/2;
/*
		if(x + (CARDWIDTH / 2) > (scr->w / 2)) //animate to the left
			xspeed = -4; //pixels to move
		else
			xspeed =  4;
*/
		if(rot)
			xspeed = -4;
		else
			xspeed =  4;

		yspeed = 0;

		do //while card is within the screen
		{
			SDL_PumpEvents();
			if(SDL_GetMouseState(NULL, NULL))
				return -1; // stop the animation

			yspeed = yspeed + GRAVITY;
			x += xspeed;
			y += yspeed;

			if(y + CARDHEIGHT > MAXY)
			{
				y = MAXY - CARDHEIGHT;
				yspeed = int(-yspeed * BOUNCE);
			}

			DrawCard(x, y, id, scr);
			SDL_Flip(scr);
		}
		//73 here is CARDWIDTH, but when using CARDWIDTH, it doesn't work
		while((x + 73 > 0) && (x < scr->w));
//		while((x + CARDWIDTH > 0) && (x < scr->w));
	}
	while(1); //or while within specified time

	return 0;
}


