
#include <time.h>
#include <stdlib.h>

#include "Deck.h"


Deck::Deck()
{
    init();
}

Deck::~Deck()
{
    
}

void Deck::init()
{
    Card card;
    itsCardsInDeck = 0;
    
    srand(time(NULL));
    for(int i = 0; i < MAX_SUIT; i++)
    {
        for(int j = 0; j < MAX_NUMBER; j++)
        {
            card.suit = i;
            card.number = j;
            
			itsCards[itsCardsInDeck++] = card;
        }
    }
}

void Deck::addCard(Card newCard)
{
    Card card;
    card.suit = newCard.suit;
    card.number = newCard.number;
    
    itsCards[itsCardsInDeck++] = card;
}

Card Deck::removeCard() {
    int index;
    int counter = 0;
    int semi_rand = ((rand() % itsCardsInDeck) + 1);
    
    for (index = 0; index < NUM_CARDS; index++) {
        if (itsCards[index].suit != NONE_SUIT) {
            counter++;
        }
        
        if (counter == semi_rand) {
            break;
        }
    }
    
    return removeCard(index);
}

Card Deck::removeCard(int index) {
    
	Card card;
	card.suit = NONE_SUIT;
    
    if (index < 0 || index >= NUM_CARDS) {
		return card;
    }
    
	card = itsCards[index];
    
    itsCards[index].suit = NONE_SUIT;
    itsCardsInDeck--;
    
	return card;
}
