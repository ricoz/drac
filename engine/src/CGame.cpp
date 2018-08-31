/*
	filename:		CGame.h
	author:			Rico Roberto Zuñiga
	last modified:	9/12/03

	description:	Game subs.
*/

#include "../includes/CGame.h"
#include <stdio.h>

void CGame::CreateRegion(int id, unsigned int attribs, unsigned int amode, int dmode, int symbol, int x, int y, int xoffset, int yoffset)
{
    CCardRegion *cr = new CCardRegion(id, attribs, amode, dmode, symbol, x, y, xoffset, yoffset);
	this->push_back(*cr);
}

bool CGame::DeleteRegion(CCardRegion *pRegion)
{
	for(rVI vi = this->begin(); vi != this->end(); ++vi)
	{
		if(&(*vi) == pRegion)
		{
			this->erase(vi);
			return true;
		}
	}

	return false;
}

void CGame::EmptyStacks()
{
    for(rVI vi = this->begin(); vi != this->end(); ++vi)
        vi->Clear();
}

void CGame::InitAllCoords()
{
	for(rVI vi = this->begin(); vi != this->end(); ++vi)
	{
		vi->InitCardCoords();
		vi->InitCardFaces();
	}
}

//------------------------------------------------------------------------------------------------
//Drag and drop

CCardRegion* CGame::OnMouseDown(int x, int y)
{
	for(rVI vi = this->begin(); vi != this->end(); ++vi)
	{
		if(vi->PtInStack(x, y) )
		{
			SourceRegion = &(*vi);
			return SourceRegion;
		}
	}
	return NULL;
}

bool CGame::InitDrag(int x, int y)
{
	return InitDrag(NULL, x, y);
}

// optimization needed
bool CGame::InitDrag(CCardStack *CargoStack, int x, int y)
{
	if(CargoStack == NULL)
	{
		if(SourceRegion->Empty())
			return false;

		int idx = SourceRegion->GetClickedCard(x, y);
		if(idx == -1)
			return false; // no card found

		switch(SourceRegion->GetDragMode())
		{
			case CRD_DRAGCARDS:
				DragStack.Push(SourceRegion->RemoveCard(idx));
				break;

			case CRD_DRAGTOP:
				if( SourceRegion->Size() - 1 == idx )
					DragStack.Push(SourceRegion->Pop());
				else
					return false;
				break;

			case CRD_DRAGFACEUP:
				if(SourceRegion->CardFaceUp(idx))
					DragStack.Push( SourceRegion->Pop( SourceRegion->Size() - idx ) );
 				else
 					return false;
 				break;

			default: // CRD_DRAGSTACKS
				DragStack.Push( SourceRegion->Pop( SourceRegion->Size() - idx ) );
		}
	}
	else
		DragStack.Push(*CargoStack);

	SourceRegion->InitCardCoords();

	DrawBackground();
	SDL_BlitSurface(screen, NULL, background, NULL);

	CCardRegion DragRegion(0, SourceRegion->GetAttributes()|CRD_FACEUP, 0, 0, 0, 0, 0, SourceRegion->GetxOffset(), SourceRegion->GetyOffset());
	DragRegion.Push(DragStack);

	dcard.x = DragStack[0].x;
	dcard.y = DragStack[0].y;
	dcard.width = DragRegion.GetStackWidth();
	dcard.height = DragRegion.GetStackHeight();

	dragface = SDL_CreateRGBSurface(SDL_SWSURFACE, dcard.width, dcard.height, 32, 0, 0, 0, 0);
	SDL_FillRect(dragface, NULL, SDL_MapRGB(dragface->format, 0, 255, 0));
	SDL_SetColorKey(dragface, SDL_SRCCOLORKEY, SDL_MapRGB(dragface->format, 0, 255, 0));

	DragRegion.DrawCardStack(screen);
	DragRegion.InitCardCoords();
	DragRegion.DrawCardStack(dragface);

	oldx = x;
	oldy = y;

	SDL_Flip(screen);
	return true;
}

// optimization needed
void CGame::DoDrag(int x, int y)
{
	SDL_Rect rcs;
	SDL_Rect rcd;

	rcs.x = dcard.x - 2;
	rcs.y = dcard.y - 2;
	rcs.w = dcard.width + 4;
 	rcs.h = dcard.height + 4;

	rcd.x = dcard.x - 2;
 	rcd.y = dcard.y - 2;

	if(dcard.x < 0) rcs.x = rcd.x = 0;
	if(dcard.y < 0) rcs.y = rcd.y = 0;

	if(x < oldx) dcard.x -= oldx - x; else dcard.x += x - oldx;
	if(y < oldy) dcard.y -= oldy - y; else dcard.y += y - oldy;

	oldx = x;
	oldy = y;

	SDL_Rect dest;
	dest.x = dcard.x;
	dest.y = dcard.y;

	SDL_BlitSurface(background, &rcs, screen, &rcd);
	SDL_BlitSurface(dragface, NULL, screen, &dest);

	SDL_Flip(screen);
}

void CGame::DoDrop()
{
	DoDrop(NULL);
}
// optimization needed
void CGame::DoDrop(CCardRegion *DestRegion)
{
	CCardStack *DestStack;
	CCardRegion *BestRegion;

	if(DestRegion != NULL) BestRegion = DestRegion;
//	else BestRegion = this->GetBestStack(dcard.x, dcard.y, dcard.width, dcard.height, &DragStack);
	else BestRegion = this->GetBestStack(dcard.x, dcard.y, CARDWIDTH, CARDHEIGHT, &DragStack);
	if(BestRegion == NULL) BestRegion = SourceRegion;

	DestStack = BestRegion->GetCardStack();
	DestStack->Push(DragStack);
	BestRegion->InitCardCoords();

	VI vi;
	switch(SourceRegion->GetDragMode())
	{
		case 2:
		case 3:
			vi = DestStack->end() - 1;
			break;
		default: // 1 and 4
			vi = DestStack->end() - DragStack.Size();
	}

	DragStack.Clear();

	//when no movement
	if(dcard.x == vi->x && dcard.y == vi->y)
		return;

	ZoomCard(dcard.x, dcard.y, vi->x, vi->y, dcard.width, dcard.height, background, dragface);
}
CCardRegion* CGame::FindDropRegion(int Id, CCard card)
{
	CCardStack stack;
	stack.Push(card);
	return FindDropRegion(Id, stack);
}

CCardRegion* CGame::FindDropRegion(int Id, CCardStack stack)
{
	for(rVI vi = this->begin(); vi != this->end(); ++vi)
	{
		if((vi->Id == Id) && vi->CanDrop(&stack))
			return &(*vi);
	}
	return NULL;
}

void CalcPt(int x0, int y0, int x1, int y1, float t, int &X, int &Y)
{
	X = int(x0 + t * (x1 - x0) + .5);
	Y = int(y0 + t * (y1 - y0) + .5);
}

void CGame::ZoomCard(int &sx, int &sy, int &dx, int &dy, int w, int h, SDL_Surface *bg, SDL_Surface *fg)
{
	SDL_Rect rcs;
	SDL_Rect rcd;
	SDL_Rect dest;

	int px, py;
	float precision = 0.1;

	for(float i = 0.0; i <= 1.0; i += precision)
	{
		SDL_PumpEvents();
		CalcPt(sx, sy, dx, dy, i, px, py);

		rcs.x = sx - 2;
		rcs.y = sy - 2;
		rcs.w = w + 4;
 		rcs.h = h + 4;

		rcd.x = sx - 2;
	 	rcd.y = sy - 2;

		if(sx < 0) rcs.x = rcd.x = 0;
		if(sy < 0) rcs.y = rcd.y = 0;

		sx = dest.x = px;
		sy = dest.y = py;

		SDL_BlitSurface(bg, &rcs, screen, &rcd);	//background
		SDL_BlitSurface(fg, NULL, screen, &dest);	//foreground

		SDL_Flip(screen);
	}
	DrawStaticScene();
	SDL_FreeSurface(dragface);
}

void CGame::DrawStaticScene()
{
	//background color
	SDL_FillRect(screen,NULL,0x008040);

	for(rVI vi = this->begin(); vi != this->end(); ++vi)
	{
		SDL_PumpEvents();
		vi->DrawCardStack();
	}

	SDL_Flip(screen);
}

void CGame::DrawBackground()
{
	//background color
	SDL_FillRect(screen,NULL,0x008040);

	for(rVI vi = this->begin(); vi != this->end(); ++vi)
		vi->DrawCardStack();
}

CCardRegion *CGame::GetBestStack(int x, int y, int w, int h, CCardStack *stack)
{
	int maxoverlap =  0;
	int percent = 0;
	CCardRegion *best = 0;

	for(rVI vi = this->begin(); vi != this->end(); ++vi)
	{
		SDL_PumpEvents();
		if(vi->CanDrop(stack))
			percent = vi->GetOverlapRatio(x, y, w, h);

		if(percent > maxoverlap)
		{
			maxoverlap = percent;
			best = &(*vi);
		}

	}
	return best;
}
/*
void CGame::AnimateRegions(int Id)
{
	for(rVI vi = this->begin(); vi != this->end(); ++vi)
	{
		if(vi->Id == Id)
			vi->AnimateStack();
	}
}
*/
