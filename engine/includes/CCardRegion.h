/*
	filename:		CCardRegion.h
	author:			Rico Roberto Zuñiga
	last modified:	9/12/03

	description:	Interface for card region data structure.
*/

#ifndef CCARDREGION_H
#define CCARDREGION_H

#include "CCardStack.h"
#include "CardGraphics.h"

//Drag Modes
const unsigned int CRD_DRAGSTACKS	=	1; //Drag complete stacks
const unsigned int CRD_DRAGCARDS	=	2; //Drag individual cards
const unsigned int CRD_DRAGTOP		=	3; //Drag only top of stack
const unsigned int CRD_DRAGFACEUP	=	4; //Variation of 1, but only cards facing up

//Attributes
const unsigned int CRD_VISIBLE		= ( 1 << 0 ); // card region is drawn or not
const unsigned int CRD_FACEUP		= ( 1 << 1 ); // draw the front or back of the card
const unsigned int CRD_3D			= ( 1 << 2 ); // draw card region as a 3d stack
const unsigned int CRD_DODRAG		= ( 1 << 3 ); // card region can/cannot be dragged
const unsigned int CRD_DODROP		= ( 1 << 4 ); // card region (doesn't)accept drops
//Accept/Drop flags
const unsigned int CRD_DOALL		= ( 1 <<  0 ); // TRUE:accept anything					FALSE:check other flags
const unsigned int CRD_DOSINGLE		= ( 1 <<  1 ); // TRUE:accept only single cards			FALSE:accept any cardstack size
const unsigned int CRD_DOOPCOLOR	= ( 1 <<  2 ); // TRUE:accept only opposite colored		FALSE:check next 2 flags
const unsigned int CRD_DORED		= ( 1 <<  3 ); // TRUE:accept reds,						FALSE:accept only blacks
const unsigned int CRD_DOBLACK		= ( 1 <<  4 ); // TRUE:accept blacks,					FALSE:accept only reds if CRD_DORED is true
const unsigned int CRD_DOHIGHER		= ( 1 <<  5 ); // TRUE:accept higher valued				FALSE:don't accept higher valued
const unsigned int CRD_DOLOWER		= ( 1 <<  6 ); // TRUE:accept lower valued				FALSE:don't accept lower valued
const unsigned int CRD_DOHIGHERBY1	= ( 1 <<  7 ); // TRUE:accept 1 rank higher				FALSE:don't accept higher valued
const unsigned int CRD_DOLOWERBY1	= ( 1 <<  8 ); // TRUE:accept 1 rank lower				FALSE:don't accept lower valued
const unsigned int CRD_DOEQUAL		= ( 1 <<  9 ); // TRUE:accept equal valued    			FALSE:don't accept equal valued
const unsigned int CRD_DOSUIT		= ( 1 << 10 ); // TRUE:accept only cards of same suit	FALSE:accept any suit
//On empty stacks, used in most solitaire games
const unsigned int CRD_DOACE		= ( 1 << 11 ); // TRUE:accept only aces					FALSE:don't accept ace
const unsigned int CRD_DOKING		= ( 1 << 12 ); // TRUE:accept only kings				FALSE:don't accept king

class CCardRegion
{
	public:
		CCardRegion(int id, unsigned int attribs, unsigned int amode, int dmode, int symbol, int x, int y, int xoff, int yoff)
			: Id(id), Attributes(attribs), AcceptMode(amode), DragMode(dmode), Symbol(symbol), xCoord(x), yCoord(y), xOffset(xoff), yOffset(yoff) {}

		CCardRegion()	{}
		~CCardRegion()	{}

		void SetCardStack(const CCardStack & cs)		{ InternalStack = cs; }
		CCardStack* GetCardStack()						{ return &InternalStack; }

		void SetCoords(int x, int y)					{ xCoord = x; yCoord = y; }
		void GetCoords(int &x, int &y)					{ x = xCoord; y = yCoord; }

		void SetOffsets(int x, int y)					{ xOffset = x; yOffset = y; }
		int  GetOffsets(int &xoff, int &yoff)			{ xoff = xOffset; yoff = yOffset; }
		int  GetxOffset()								{ return xOffset; }
		int  GetyOffset()								{ return yOffset; }

	    void InitCardFaces()							{ InternalStack.SetCardsFaceUp(Attributes&CRD_FACEUP); }

   		bool IsVisible()								{ return Attributes&CRD_VISIBLE; }
    	bool FaceUp() const								{ return Attributes&CRD_FACEUP; }
    	bool FaceDown() const							{ return !(Attributes&CRD_FACEUP); }
		bool CanDrag() const							{ return Attributes&CRD_DODRAG; }

        void SetCardFaceUp(bool fTrue, int Idx)			{ InternalStack[Idx].SetFaceUp(fTrue); }
        bool CardFaceUp(int Idx)						{ InternalStack[Idx].FaceUp(); }

		void SetDragMode(int mode)						{ DragMode = mode; }
		int  GetDragMode() const						{ return DragMode; }
		
		void SetSymbol(int symbol)						{ Symbol = symbol; }

		void SetAcceptMode(unsigned int mode)			{ AcceptMode = mode; }
		void SetAttributes(unsigned int attr)			{ Attributes = attr; }
		int  GetAttributes()							{ return Attributes; }

//---------------------------------------------------------------------------------------
//Defined in CCardRegion.cpp

		bool CanDrop(CCardStack *stack);

		void DrawCardStack();
		void DrawCardStack(SDL_Surface *s);

//		void AnimateStack();

		void InitCardCoords();
		int  GetOverlapRatio(int x, int y, int width, int height);

		bool PtInStack(int x, int y); //Imperfect, needs tweaking
		bool PtOnTop(int x, int y);

		int GetStackWidth();
		int GetStackHeight();

//---------------------------------------------------------------------------------------
//CCardStack object wrappers

		void        NewDeck()							{ InternalStack.NewDeck();  }
		void        Shuffle()							{ InternalStack.Shuffle();  }
		void        Clear()								{ InternalStack.Clear();    }
		void        Reverse()							{ InternalStack.Reverse();  }

		void        Push(CCard card)					{ InternalStack.Push(card); }
		void        Push(CCardStack cs)					{ InternalStack.Push(cs);   }

		void		AddJokers(int jokers)				{ InternalStack.AddJokers(jokers); }

    	bool		Empty()								{ return InternalStack.Empty(); }
		int			Size()								{ return InternalStack.Size(); }

		CCard       Pop()								{ return InternalStack.Pop();      }
		CCardStack  Pop(int items)						{ return InternalStack.Pop(items); }

		CCard		RemoveCard(int index)				{ return InternalStack.RemoveCard(index); }

		int			GetClickedCard(int x, int y)		{ return InternalStack.GetClickedCard(x, y); }
		CCard		GetCard(int index)					{ return InternalStack.GetCard(index); }
		
//---------------------------------------------------------------------------------------

		int Id;

	private:

		CCardStack InternalStack;

		int xCoord;
		int yCoord;

		int xOffset;
		int yOffset;

		int DragMode;
		int Symbol;

		unsigned int Attributes;
		unsigned int AcceptMode;
};

#endif //CCARDREGION_H
