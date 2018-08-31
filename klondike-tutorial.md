# Klondike Tutorial

This tutorial will walk you through the creation of a simple solitaire game called klondike using Drac Card Game Library. You will see how simple it is to create your own card games with Drac.

First thing to do is to include the necessary header files.

```cpp
#include <stdio.h>
#include <stdlib.h>

#include "Engine/Includes/CGame.h"
#include "Font/font.h"
```

Define the Card Region ID's for easy identificaton. This will be used later to identify what card region the player has clicked.

```cpp
#define     CRD_PILE            0
#define     CRD_FOUNDATION      1
#define     CRD_RESERVE         2
#define     CRD_WASTE           3
```

We then define the essential objects in our game. screen is where we draw the actual game and it is what the player sees. We also define a CGame object and name it Klondike, this will take care of game-specific instructions.

```cpp
SDL_Surface *screen;
SDLFont *font1;        // 2 fonts
SDLFont *font2;
CGame Klondike;
```

We create a function called Initialize where we perform all necessary initialization. We also create in here all the Card Regions required in the game. Card Regions handle the display and manipulation of Card Stacks. A Card Stack is the fundamental data structure representing collections of cards. To create a Card Region, we call Klondike's member function CreateRegion(...) and pass in the values that describe how the Card Region is displayed and how it behaves. We specify how the region is displayed, how it accepts cards when cards are being dropped on it, and how cards are dragged from it.

The first thing you have to do in creating a card game is to understand how the game is played. When we look at other Klondike games, we will observe that generally there are 13 regions where cards are located. We group those Card Regions with the same behavior and assign an ID. In Klondike there are basically 4 groups, the card pile(1 region), the reserved pile(1 region), the waste piles(4 regions), and the foundation stacks(7 regions). 

```cpp
void Initialize() // must be called only once
{
    //do some initialization and create the regions in here

    SDL_WM_SetCaption("My Card Game", NULL); // Set the window title
    font1 = initFont("font/data/font1");
    font2 = initFont("font/data/font2");

    InitDeck(screen);
    Klondike.Initialize(screen);

    //index 0
    Klondike.CreateRegion(CRD_PILE, CRD_VISIBLE|CRD_3D, 0, 0, CRD_OSYMBOL, 35, 10, 2, 2);
    //index 1-7
    for(int i=1; i <= 7; i++)
        Klondike.CreateRegion(CRD_FOUNDATION, CRD_VISIBLE|CRD_DODRAG|CRD_DODROP, CRD_DOOPCOLOR|CRD_DOLOWER|CRD_DOLOWERBY1|CRD_DOKING, CRD_DRAGFACEUP, CRD_HSYMBOL, (CARDWIDTH * (i -1)) + (i * 35), CARDHEIGHT + 40, 0, 16);
    //index 8
    Klondike.CreateRegion(CRD_RESERVE, CRD_VISIBLE|CRD_FACEUP|CRD_DODRAG|CRD_3D, CRD_DOALL, CRD_DRAGTOP, CRD_NSYMBOL, CARDWIDTH + 65, 10, 0, 0);
    //index 9-12
    for(int i=4; i <= 7; i++)
        Klondike.CreateRegion(CRD_WASTE, CRD_VISIBLE|CRD_3D|CRD_DODRAG|CRD_DODROP, CRD_DOSINGLE|CRD_DOHIGHER|CRD_DOHIGHERBY1|CRD_DOACE|CRD_DOSUIT, CRD_DRAGTOP, CRD_HSYMBOL, (CARDWIDTH * (i -1)) + (i * 35), 10, 0, 0);
}
```

In a card game or in any other game, the player must be able to start a new game, so we define a function, NewGame() wherein we perform all necessary processes that will set the stage for the player.

```cpp
void NewGame()
{
    //Reset pile symbol
    Klondike[0].SetSymbol(CRD_OSYMBOL);

    //Empty the card regions from the previous game
    Klondike.EmptyStacks();

    //create then shuffle the deck
    Klondike[0].NewDeck();
    Klondike[0].Shuffle();

    //deal
    for(int i=1; i <= 7; i++)
        Klondike[i].Push(Klondike[0].Pop(i));

    //initialize all card coordinates
    Klondike.InitAllCoords();

    //set initial faced up cards in foundations
    for(int i=1; i <= 7; i++)
        Klondike[i].SetCardFaceUp(true, Klondike[i].Size() -1);
}
```

The following functions represent the actual game play. In Klondike, all the player has to do is click on the cards, drag and drop them to the appropriate Card Stacks, and with a little luck , he'll be able to finish the game.

Here, we handle keyboard events. Starts a new game when 'n' is pressed, the other keys are used for testing.

```cpp
void HandleKeyDownEvent(SDL_Event &event)
{
    if(event.key.keysym.sym == SDLK_n)      { NewGame(); Klondike.DrawStaticScene(); }
    if(event.key.keysym.sym == SDLK_a)      { AnimateCards(); }; // Test animation
    if(event.key.keysym.sym == SDLK_r)      { Klondike.DrawStaticScene(); }; // Refresh
}
```

Here, mouse down events are handled. First we determine if the player used the left or right mouse button. If it is the left button, we determine what Card Region the player has clicked, do some stuff based on what Card Region was clicked - if the Card Region is draggable, we initialize the drag sequence, if the Card Region doesn't allow dragging but only transfers its cards to another stack, we perform the transfer. I used right click instead of double-click since no double click handler is implemented yet. We perform here the same things other Klondike games do, when the player right clicks on a card, that card is checked and if it can be transferred, it will be transferred to the appropriate waste pile.

```cpp
bool startdrag = false;
void HandleMouseDownEvent(SDL_Event &event)
{
    CCardRegion *srcReg;
    if(event.button.button == SDL_BUTTON_LEFT)
    {
        srcReg = Klondike.OnMouseDown(event.button.x, event.button.y);
        if(srcReg == NULL) return;
        //clicked on the top of the foundations
        if((srcReg->Id == CRD_FOUNDATION) && srcReg->PtOnTop(event.button.x, event.button.y))
        {
            srcReg->SetCardFaceUp(true, srcReg->Size() -1);
        }
        //clicked on the foundations, reserve, wastes for dragging
        if(((srcReg->Id == CRD_FOUNDATION) || (srcReg->Id == CRD_RESERVE) || (srcReg->Id == CRD_WASTE)) && Klondike.InitDrag(event.button.x, event.button.y))
        {
            startdrag = true;
            SDL_WM_GrabInput(SDL_GRAB_ON);
        }
        //clicked on the pile
        if(srcReg->Id == CRD_PILE)
        {
            CCardStack *cs = new CCardStack;
            if(srcReg->Empty() && !Klondike[8].Empty()) //Bring back the cards
            {
                *cs = Klondike[8].Pop(Klondike[8].Size());
                cs->SetCardsFaceUp(false);
                Klondike.InitDrag(cs, -1, -1);
                Klondike.DoDrop(&Klondike[0]);
                Klondike[0].Reverse();
                Klondike[0].InitCardCoords();
            }

            else if(!srcReg->Empty() && (!Klondike[8].Empty() || Klondike[8].Empty()))
            {
                *cs = Klondike[0].Pop(1);
                cs->SetCardsFaceUp(true);
                Klondike.InitDrag(cs, -1, -1);
                Klondike.DoDrop(&Klondike[8]);
            }
        }
    }

    //substitute right-click for double-click event
    if(event.button.button == SDL_BUTTON_RIGHT)
    {
        srcReg = Klondike.OnMouseDown(event.button.x, event.button.y);
        if(srcReg == NULL) return;
        CCardRegion *cr;
        CCard card =  srcReg->GetCard(srcReg->Size()-1);

        //clicked on the top of the foundations
        if(((srcReg->Id == CRD_FOUNDATION) || (srcReg->Id == CRD_RESERVE)) && card.FaceUp() && srcReg->PtOnTop(event.button.x, event.button.y))
        {
            if(cr = Klondike.FindDropRegion(CRD_WASTE, card))
            {
                CCardStack *cs = new CCardStack;
                *cs = srcReg->Pop(1);
                Klondike.InitDrag(cs, -1 , -1);
                Klondike.DoDrop(cr);
            }
        }
    }
}
```

Here we handle mouse movement, usually the player moves the mouse to transfer the cards so we take care of the actual dragging here.

```cpp
void HandleMouseMoveEvent(SDL_Event &event)
{
    if(event.motion.state == SDL_BUTTON(1) && startdrag)
        Klondike.DoDrag(event.motion.x, event.motion.y);
}
```

Here we end the drop, we also check whether the player has won.

```cpp
void HandleMouseUpEvent(SDL_Event &event)
{
    if(startdrag)
    {
        startdrag = false;
        Klondike.DoDrop();
        SDL_WM_GrabInput(SDL_GRAB_OFF);
    }
    if(Klondike[0].Empty() && Klondike[8].Empty())
    {
        Klondike[0].SetSymbol(1);
        Klondike.DrawStaticScene();
    }
    //victory
    if((Klondike[9].Size() == 13) && (Klondike[10].Size() == 13) && (Klondike[11].Size() == 13) && (Klondike[12].Size() == 13))
    {
        AnimateCards();
        NewGame();
        Klondike.DrawStaticScene();
    }
}
```

The main function, the usual SDL stuff. We initalize SDL, set the video, we also initialize Klondike by calling Initialize(), start a new game and draw the initial scene. We perform all of these prior to the actual game loop. And when the game loop ends, we do some cleaning.

```cpp
int main(int argc, char *argv[])
{
    if( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 )
    {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    screen=SDL_SetVideoMode(800, 600, 32, SDL_SWSURFACE|SDL_HWPALETTE|SDL_FULLSCREEN);
//  screen=SDL_SetVideoMode(800, 600, 32, SDL_SWSURFACE|SDL_HWPALETTE); //Slow

    if(screen == NULL)
    {
        printf("Unable to set 0x0 video: %s\n", SDL_GetError());
        exit(1);
    }

    Initialize();
    NewGame();
    Klondike.DrawStaticScene();

    SDL_Event event;
    int done = 0;

    while(done == 0)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    return 0;

                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_ESCAPE) { done = 1; }
                    HandleKeyDownEvent(event);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    HandleMouseDownEvent(event);
                    break;

                case SDL_MOUSEMOTION:
                    HandleMouseMoveEvent(event);
                    break;

                case SDL_MOUSEBUTTONUP:
                    HandleMouseUpEvent(event);
                    break;
            }
        }
    }

// perform cleaning up in here

    freeFont(font1);
    freeFont(font2);
    return 0;
}
```