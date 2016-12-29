
#ifndef __PointGame__Deck__
#define __PointGame__Deck__

#include "common.h"
#include "Card.h"

class Deck
{
public:
    
    Deck();
    ~Deck();
    
    void init();
    void reset(){ itsCardsInDeck = 0; }
    
    void addCard(Card newCard);
    Card removeCard();
	int countTotal(){ return itsCardsInDeck; }
    
protected:

    Card removeCard(int index);
    
    Card itsCards[NUM_CARDS];
    int  itsCardsInDeck;
};

#endif /* defined(__PointGame__Deck__) */
