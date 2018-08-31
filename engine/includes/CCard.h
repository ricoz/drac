/*
	filename:		CCard.h
	author:			Rico Roberto Zuñiga
	last modified:	9/12/03

	description:	Interface for the CCard class. Handles the card data structure.
 					Some functions based on CardLib.
*/

#ifndef CCARD_H
#define CCARD_H

// if will be using custom cards then these should be initialized outside
const unsigned int CARDWIDTH	=	73;
const unsigned int CARDHEIGHT	=	97;

/*
enum eSuit  { Spades = 0, Hearts = 1, Clubs = 2, Diamonds = 3 };
enum eValue { Ace = 1, Two = 2, Three = 3, Four = 4, Five = 5, Six = 6, Seven = 7,
              Eight = 8, Nine = 9, Ten = 10, Jack = 11, Queen = 12, King = 13 };
*/
inline int MAKE_CARD(int Value, int Suit)
{
    if(Value < 1)   Value = 1;
    if(Value >  13) Value = 13;

    if(Suit < 0)    Suit = 0;
    if(Suit > 3)    Suit = 3;

    return ( (Suit * 13) + (Value - 1) );
}

class CCard
{
public:

    CCard()
    {
        Idx = 1; //ace of spades by default
        fFaceUp = true;
    }

    CCard(int value, int suit) //specify a face value [1-13] and suit [0-3]
    {
        Idx = MAKE_CARD(value, suit);
        fFaceUp = true;
    }

    CCard(int index) //specify a 0-51-joker index
    {
        if(index < 0)  index = 0;

        Idx = index;
        fFaceUp = true;
    }

    int Suit() const			{ return ((Idx - ((Idx % 13) + 1)) / 10); }
    int Rank() const			{ return ((Idx % 13) + 1); }

    bool IsBlack() const 		{ return Suit() == 0 || Suit() == 2; }
    bool IsRed() const			{ return !IsBlack(); }

    bool FaceUp() const			{ return fFaceUp; }
    bool FaceDown() const		{ return !fFaceUp; }
    
    bool Joker() const			{ return Idx > 51; }

    void SetFaceUp(bool fTrue)	{ fFaceUp = fTrue; }
    void SetIdx(int nIdx)		{ Idx = nIdx; }

    void SetCardLoc(int lx, int ly)	{ x = lx; y = ly; }

	bool PtInCard(int lx, int ly)
	{
		if(lx >= x && lx <= CARDWIDTH + x && ly >= y && ly <= CARDHEIGHT + y)
			return true;
		else
			return false;
	}

/*	These should be private, but i think it's
	faster if they can be accessed directly,
	just be careful not to accidentally
	modify the values.						*/
	int x;
	int y;
    int Idx;

private:

    bool	fFaceUp;
};

#endif //CCARD_H
