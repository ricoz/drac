/*
	filename:		CGame.h
	author:			Rico Roberto Zuñiga
	last modified:	9/12/03

	description:	Interface for game data strucuture.
*/

#ifndef CGAME_H
#define CGAME_H

#include <vector>

#include "CCardRegion.h"

typedef std::vector<CCardRegion>::iterator rVI;

struct DRAGCARD
{
	int x;
	int y;
	int width;
	int height;
};

class CGame : public std::vector<CCardRegion>
{
public:

	CGame() {}
	~CGame() { SDL_FreeSurface(background); }

	void Initialize(SDL_Surface *s)
 	{
  		screen = s;
		background = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->w, screen->h, 32, 0, 0, 0, 0);
		//initialize more values here
    }

	void Clear()	{ this->clear(); }
	int Size()		{ return this->size(); }

//----------------------------------------------------------------------------------------
//Defined in CGame.cpp

	void CreateRegion (int id, unsigned int attribs, unsigned int amode, int dmode, int symbol, int x, int y, int xoffset, int yoffset);
	bool DeleteRegion(CCardRegion *pRegion);

   	void EmptyStacks();
	void InitAllCoords();

//----------------------------------------------------------------------------------------
//Drag and Drop

	CCardRegion *OnMouseDown(int x, int y);

	bool InitDrag(int x, int y);
	bool InitDrag(CCardStack *CargoStack, int x, int y);

	void DoDrag(int x, int y);

	void DoDrop();
	void DoDrop(CCardRegion *DestRegion);

	CCardRegion *FindDropRegion(int Id, CCard card);
	CCardRegion *FindDropRegion(int Id, CCardStack stack);

	void ZoomCard(int &sx, int &sy, int &dx, int &dy, int width, int height, SDL_Surface *bg, SDL_Surface *fg);

	void DrawStaticScene();
	void DrawBackground();

//	void AnimateRegions(int Id);

	CCardRegion *GetBestStack(int x, int y, int width, int height, CCardStack *stack);
//----------------------------------------------------------------------------------------

private:

	CCardStack DragStack;
	CCardRegion *SourceRegion;

	SDL_Surface *screen;
	SDL_Surface *background;
	SDL_Surface *dragface;

	DRAGCARD dcard;

	int oldx;
	int oldy;
};

#endif //CGAME_H
