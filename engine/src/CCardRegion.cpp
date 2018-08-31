/*
	filename:		CCardRegion.cpp
	author:			Rico Roberto Zuñiga
	last modified:	9/12/03

	description:	Card region data structure subs.
*/

#include "../includes/CCardRegion.h"

void CCardRegion::DrawCardStack()
{
	DrawCardStack(NULL);
}

void CCardRegion::DrawCardStack(SDL_Surface *s)
{
	if(!(Attributes&CRD_VISIBLE))
		return;

	DrawSymbol(xCoord, yCoord, Symbol);
	for(VI vi = InternalStack.begin(); vi != InternalStack.end(); ++vi)
	{
		if(vi->FaceUp())
			DrawCard(vi->x, vi->y, vi->Idx, s);
		else
			DrawCardBack(vi->x, vi->y, s);
	}
}

/*
void CCardRegion::AnimateStack()
{
	for(VI vi = InternalStack.begin(); vi != InternalStack.end(); ++vi)
	{
		if(AnimateCard(*vi) == -1)
			return;
	}
}
*/
void CCardRegion::InitCardCoords()
{
	if(InternalStack.Empty())
		return;

	int x = xCoord;
	int y = yCoord;

	int pos = 0;
	int it = 0;

	if(Attributes&CRD_3D)
	{
		for(VI vi = InternalStack.begin(); vi != InternalStack.end(); ++vi)
		{
			vi->SetCardLoc(x, y);
			if( !((it++)%4) )
				pos++;
			x = (pos*2)+xCoord;
			y = pos+yCoord;
		}
	}

	else
	{
		for(VI vi = InternalStack.begin(); vi != InternalStack.end(); ++vi)
		{
			vi->SetCardLoc(x, y);
			x += xOffset;
			y += yOffset;
		}
	}
}
//__________________________________________________________
/*															|
|  Imperfect, doesn't work well with diagonal, reversed,  	|
|  and 3d stacks... Needs tweaking							|
|_________________________________________________________*/

bool CCardRegion::PtInStack(int x, int y)
{
	if(!(Attributes&CRD_VISIBLE))
		return false;

	if(Attributes&CRD_3D && !this->Empty())
		return InternalStack[InternalStack.Size()-1].PtInCard(x, y);

	int StackWidth = this->GetStackWidth();
	int StackHeight = this->GetStackHeight();

	if(x >= xCoord && x <= xCoord + StackWidth && y >= yCoord && y <= yCoord + StackHeight)
		return true;
	else
		return false;
}

bool CCardRegion::PtOnTop(int x, int y)
{
	if(this->Empty())
		return false;

    return InternalStack[InternalStack.Size()-1].PtInCard(x, y);
}

int CCardRegion::GetOverlapRatio(int x, int y, int w, int h)
{
	if(!(Attributes&CRD_DODROP))
		return 0;

	int StackWidth = this->GetStackWidth();
	int StackHeight = this->GetStackHeight();

	int wi, hi;

/*
 _______________________________
|								|
|			 _______			|
|			|		|			|
|			|		|			|
|___________|_______|___________|
			|		|
			|_______|
*/
	if( PtInStack(x, y) )
	{
		if(xCoord + StackWidth > x + w)
			wi = w;
		else
			wi = (xCoord + StackWidth) - x;

		hi = (yCoord + StackHeight) - y;
	}
/*
	 _______________________________
	|								|
 ___|___				 			|
|	|	|							|
|	|	|							|
|	|___|___________________________|
|		|
|_______|
*/
	else if( PtInStack(x + w, y))
	{
		wi = (x + w) - xCoord;
		hi = (yCoord + StackHeight) - y;
	}
/*
			 _______
			|		|
 ___________|_______|___________
|			|		|			|
|			| 		|			|
|			|_______|			|
|			 					|
|_______________________________|
*/
	else if( PtInStack(x, y + h) )
	{
		if(xCoord + StackWidth > x + w)
			wi = w;
		else
			wi = (xCoord + StackWidth) - x;

		hi = (y + h) - yCoord;
	}
/*
 _______
|		|
|	 ___|___________________________
|	|	|							|
|	|	|		 					|
|___|___|       					|
	|			 					|
	|_______________________________|
*/
	else if( PtInStack(x + w, y + h))
	{
		wi = (x + w) - xCoord;
		hi = (y + h) - yCoord;
	}
/*
 _______________________________
|								|
|								|
|								|
|								|
|_______________________________|
*/
	else
		return 0;

	return wi * hi;

}

//#include <stdio.h>
bool CCardRegion::CanDrop(CCardStack *stack)
{
	if(InternalStack.Empty() && (!(AcceptMode&CRD_DOKING) && !(AcceptMode&CRD_DOACE)))
		return false;

	VI vi = stack->begin();

	//if empty
	if(InternalStack.Empty() && (AcceptMode&CRD_DOKING) && (vi->Rank() != 13))
		return false;
	else if(InternalStack.Empty() && (AcceptMode&CRD_DOACE) && (vi->Rank() != 1))
		return false;
	else if(InternalStack.Empty())
		return true;

	VI myvi = InternalStack.end()-1;

	if(myvi->FaceDown())
		return false;


//	printf("vi->Suit() = %i, vi->Rank() = %i, myvi->Suit() = %i, myvi->Rank() = %i\n",vi->Suit(),vi->Rank(),myvi->Suit(),myvi->Rank());

	if(!(Attributes&CRD_DODROP))
		return false;

	if(AcceptMode&CRD_DOALL)
		return true;

	if( (AcceptMode&CRD_DOSINGLE) && (stack->Size() > 1) )
	{
//		printf("fSingle returned false\n");
		return false;
	}

	if( (AcceptMode&CRD_DOOPCOLOR) && ( (myvi->IsBlack() && vi->IsBlack()) || (myvi->IsRed() && vi->IsRed()) ) )
	{
//		printf("fOpColor returned false\n");
		return false;
	}

	if( ( (AcceptMode&CRD_DORED) && !(AcceptMode&CRD_DOBLACK) ) && vi->IsBlack() )
	{
//		printf("fRed && !fBlack returned false\n");
		return false;
	}

//	if( ( ( !(AcceptMode&CRD_DORED) && (AcceptMode&CRD_DOBLACK) ) || ( !(AcceptMode&CRD_DORED) && !(AcceptMode&CRD_DOBLACK) ) ) && vi->IsRed() )
	if( ( !(AcceptMode&CRD_DORED) && (AcceptMode&CRD_DOBLACK) ) && vi->IsRed() )
	{
//		printf("!fRed && fBlack returned false\n");
		return false;
	}

	if(!(AcceptMode&CRD_DOHIGHER) && !(AcceptMode&CRD_DOLOWER))
  		AcceptMode |= CRD_DOEQUAL;

	if(AcceptMode&CRD_DOEQUAL)
	{
//		printf("in fEqual\n");
		if( ((AcceptMode&CRD_DOHIGHER) && !(AcceptMode&CRD_DOLOWER)) && (vi->Rank() < myvi->Rank()) )
			return false;
		if( (!(AcceptMode&CRD_DOHIGHER) && (AcceptMode&CRD_DOLOWER)) && (vi->Rank() > myvi->Rank()) )
			return false;
		if( (!(AcceptMode&CRD_DOHIGHER) && !(AcceptMode&CRD_DOLOWER)) && (vi->Rank() != myvi->Rank()) )
			return false;
		//by 1
		if( ((AcceptMode&CRD_DOHIGHERBY1) && !(AcceptMode&CRD_DOLOWERBY1)) && ((vi->Rank() - myvi->Rank()) >  1) )
			return false;
		if( (!(AcceptMode&CRD_DOHIGHERBY1) && (AcceptMode&CRD_DOLOWERBY1)) && ((vi->Rank() - myvi->Rank()) < -1) )
			return false;

	}
	if(!(AcceptMode&CRD_DOEQUAL))
	{
//		printf("in !fEqual\n");
		if( ((AcceptMode&CRD_DOHIGHER) && !(AcceptMode&CRD_DOLOWER)) && (vi->Rank() <= myvi->Rank()) )
			return false;
		if( (!(AcceptMode&CRD_DOHIGHER) && (AcceptMode&CRD_DOLOWER)) && (vi->Rank() >= myvi->Rank()) )
			return false;
		if( (!(AcceptMode&CRD_DOHIGHER) && !(AcceptMode&CRD_DOLOWER)) && (vi->Rank() == myvi->Rank()) )
			return false;
		//by 1
		if( ((AcceptMode&CRD_DOHIGHERBY1) && !(AcceptMode&CRD_DOLOWERBY1)) && ((vi->Rank() - myvi->Rank()) >  1) )
			return false;
		if( (!(AcceptMode&CRD_DOHIGHERBY1) && (AcceptMode&CRD_DOLOWERBY1)) && ((vi->Rank() - myvi->Rank()) < -1) )
			return false;
	}

	if( (AcceptMode&CRD_DOSUIT) && (vi->Suit() != myvi->Suit()) )
	{
//		printf("fSuit returned false\n");
		return false;
	}

	return true;
}

int CCardRegion::GetStackWidth()
{
	if(InternalStack.Empty())
		return CARDWIDTH;

	return (InternalStack[InternalStack.Size()-1].x + CARDWIDTH) - InternalStack[0].x;
//	return CARDWIDTH + ( (this->Size()-1) * xOffset );
}

int CCardRegion::GetStackHeight()
{
	if(InternalStack.Empty())
		return CARDHEIGHT;

	return (InternalStack[InternalStack.Size()-1].y + CARDHEIGHT) - InternalStack[0].y;
//	return CARDHEIGHT + ( (this->Size()-1) * yOffset );
}

