#ifndef _HAND_H_
#define _HAND_H_

#include "common.h"
#include "Card.h"

class Hand
{                      
public:
	Hand() {}
	virtual ~Hand() {};

	void initHand();

	void addCard(Card newCard);
	void sortCards();

	int getTrivialCard();
	int getHighestCard();
	int getHighestCardExceptNumber(int number);
	int getHighestCardExceptSuit(int suit);
	int  getTotalCard();

	void setHand(const Hand *hand);

	Card getCard(int i) { return cards[i]; }
	int getCardIndex(Card card);

	//void applyRule(int begin_suit);

	int findCard(int suit, int number);
	int countSuit(int suit);
	bool hasSuit(int suit);

	bool getHigherPlayers(int suit, int number);

	// compare
	//bool highestCardOfAll(int* suit, int* value);
	int highestCard(int suit);
	int lowestCard(int suit);
	int nearestCard(int suit, int value);
	int highCard(int suit, int value);
	int lowCard(int suit, int value);
	int lowCardExcept(int suit, int value, int except);
	int highCardExcept(int suit, int value, int except);
	int highestCardExcept(int suit, int value);
	int lowestCardExcept(int suit, int except);

	void removeCard(int suit, int value);
	bool empty();

	int countLowNearest(int suit, int value);
	int countHighNearest(int suit, int value);
	int countLowBroken(int suit, int value);
	int countHighBroken(int suit, int value);
	int countLow(int suit, int value);
	int countHigh(int suit, int value);
	int countLow(int value);
	int countHigh(int value);
	int countEqual(int value);

	int getTotalValue(int suit);

	int numCardsInHand;
	int numCardsDiscard;

	Card cards[CARDS_PER_PLAYER];
};

#endif //_HAND_H_
