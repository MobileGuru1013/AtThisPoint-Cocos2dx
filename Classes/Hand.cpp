#include "cocos2d.h"
#include "Hand.h"


USING_NS_CC;

void Hand::initHand()
{
	numCardsInHand = 0;
	numCardsDiscard = 0;
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {
		cards[i].suit = NONE_SUIT;
	}
}

void Hand::addCard(Card newCard)
{
	cards[numCardsInHand++] = newCard;
}

int compareCards(const void* p1, const void* p2)
{
	Card *card1 = (Card*)p1;
	Card *card2 = (Card*)p2;

	int index1 = card1->suit*MAX_NUMBER + card1->number;
	int index2 = card2->suit*MAX_NUMBER + card2->number;

	if (index1 > index2) return 1;
	if (index1 < index2) return -1;

	return 0;
}

void Hand::sortCards()
{
	qsort(cards, numCardsInHand, sizeof(Card), compareCards);
}

void Hand::setHand(const Hand *hand)
{
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {
		cards[i] = hand->cards[i];
	}
}

int  Hand::getTotalCard()
{
	int cnt = 0;
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {

		if (cards[i].suit == NONE_SUIT) continue;

		cnt++;
	}

	return cnt;
}

int Hand::getHighestCard()
{
	int maxNumber = -1;
	int index = INVALID;
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {

		if (cards[i].suit == NONE_SUIT) continue;

		if (cards[i].number > maxNumber) {

			maxNumber = cards[i].number;
			index = i;
		}
	}

	return index;
}

int Hand::getHighestCardExceptNumber(int number)
{
	int maxNumber = -1;
	int index = INVALID;
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {

		if (cards[i].suit == NONE_SUIT) continue;
		if (cards[i].number == number) continue;

		if (cards[i].number > maxNumber) {

			maxNumber = cards[i].number;
			index = i;
		}
	}

	return index;
}

int Hand::getHighestCardExceptSuit(int suit)
{
	int maxNumber = -1;
	int index = INVALID;
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {

		if (cards[i].suit == NONE_SUIT) continue;
		if (cards[i].suit == suit) continue;

		if (cards[i].number > maxNumber) {

			maxNumber = cards[i].number;
			index = i;
		}
	}

	return index;
}

int Hand::getTrivialCard()
{
	int minNumber = MAX_NUMBER;
	int index;
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {

		if (cards[i].suit == NONE_SUIT) continue;

		if (cards[i].number < minNumber) {

			minNumber = cards[i].number;
			index = i;
		}
	}

	return index;
}

//void Hand::applyRule(int begin_suit)
//{
//	// leading allow anything
//	if (begin_suit == NONE_SUIT) {
//		return;
//	}
//
//	// If void in suit led, can play anything.
//	// If have suit led, limit to that suit
//	if (hasSuit(begin_suit)) {
//		for (int i = 0; i < CARDS_PER_PLAYER; i++) {
//			if (cards[i].suit != begin_suit) {
//				cards[i].suit = NONE_SUIT;
//			}
//		}
//	}
//}

// Find position of a card
// Return -1 if not found
int Hand::findCard(int suit, int number)
{
	for ( int i = 0; i < CARDS_PER_PLAYER; i++) {
		if (cards[i].suit == suit && cards[i].number == number) {
			return i;
		}
	}
	return INVALID;
}

// Returns how many cards of a certain suit
int Hand::countSuit(int suit)
{
	int suit_count = 0, i;

	for (i = 0; i < CARDS_PER_PLAYER; i++) {
		if (cards[i].suit == suit) {
			suit_count++;
		}
	}
	return suit_count;
}

bool Hand::hasSuit(int suit)
{
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {
		if (cards[i].suit == suit) {
			return true;
		}
	}
	return false;
}

// Returns highest card regardless of suit in hand
//bool Hand::highestCardOfAll(Suit* suit, int* value)
//{
//	Suit s = BLANK;
//	int v = CARD_BLANK;
//
//	for ( int i = 0; i < PLAYER_CARD_NUM; i++) {
//		if (cards[i].suit != BLANK && cards[i].value > v) {
//			s = cards[i].suit;
//			v = cards[i].value;
//		}
//	}
//
//	if (s != BLANK){
//		*value = v;
//		*suit = s;
//		return true;
//	}
//
//	return false;
//}

// Returns highest card of certain suit in hand
int Hand::highestCard(int suit)
{
	for (int i = CARD_K; i >= 0; i--) {

		int index = findCard(suit, i);
		if (index != INVALID) {
			return index;
		}
	}
	return INVALID;
}

// Returns lowest card of certain suit in hand
int Hand::lowestCard(int suit)
{
	for (int i = 0; i < MAX_NUMBER; i++) {

		int index = findCard(suit, i);
		if (index != INVALID) {
			return index;
		}
	}
	return INVALID;
}

// Returns lowest card of certain suit in hand
int Hand::lowestCardExcept(int suit, int except)
{
	for (int i = 0; i < MAX_NUMBER; i++) {

		int index = findCard(suit, i);
		if (index != INVALID && i != except) {
			return index;
		}
	}
	return INVALID;
}

// Returns highest from small cards than card of certain (suit, value) in hand
// if no exist, returns nearest card large than certain card.
int Hand::nearestCard(int suit, int value)
{
	for (int i = value - 1; i >= 0; i--) {

		int index = findCard(suit, i);
		if (index != INVALID) {
			return index;
		}
	}

	for (int i = value + 1; i < MAX_NUMBER; i++) {

		int index = findCard(suit, i);
		if (index != INVALID) {
			return index;
		}
	}

	return INVALID;
}

// return lowest of high cards
int Hand::highCard(int suit, int value)
{
	for (int i = value + 1; i <= CARD_A; i++) {

		int index = findCard(suit, i);
		if (index != INVALID) {
			return index;
		}
	}

	return INVALID;
}

// return highest of low cards
int Hand::lowCard(int suit, int value)
{
	for (int i = value - 1; i >= 0; i--) {

		int index = findCard(suit, i);
		if (index != INVALID) {
			return index;
		}
	}

	return INVALID;
}

int Hand::lowCardExcept(int suit, int value, int except)
{
	for (int i = value - 1; i >= 0; i--) {

		int index = findCard(suit, i);
		if (index != INVALID && i != except) {
			return index;
		}
	}

	return INVALID;
}

int Hand::highCardExcept(int suit, int value, int except)
{
	for (int i = value + 1; i <= CARD_A; i++) {

		int index = findCard(suit, i);
		if (index != INVALID && i != except) {
			return index;
		}
	}

	return INVALID;
}

int Hand::highestCardExcept(int suit, int value)
{
	for (int i = CARD_A; i >= 0; i--) {

		int index = findCard(suit, i);
		if (index != INVALID && i != value) {
			return index;
		}
	}

	return INVALID;
}

// Removes desired card and sets new last element to zero
void Hand::removeCard(int suit, int value)
{
	int index = findCard(suit, value);
	cards[index].suit = NONE_SUIT;
	//cards[index].value = 0;
}

bool Hand::empty()
{
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {
		if (cards[i].suit != NONE_SUIT) {
			return false;
		}
	}

	return true;
}

int Hand::countLow(int number)
{
	int count = 0;
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {

		if (cards[i].suit == NONE_SUIT) continue;

		if (cards[i].number <= number) {
			count ++;
		}
	}
	return count;
}

int Hand::countHigh(int number)
{
	int count = 0;
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {

		if (cards[i].suit == NONE_SUIT) continue;

		if (cards[i].number >= number) {
			count ++;
		}
	}
	return count;
}

int Hand::countLow(int suit, int number)
{
	int count = 0;
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {

		if (cards[i].suit == NONE_SUIT || cards[i].suit != suit) continue;

		if (cards[i].number <= number) {
			count++;
		}
	}
	return count;
}

int Hand::countHigh(int suit, int number)
{
	int count = 0;
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {

		if (cards[i].suit == NONE_SUIT || cards[i].suit != suit) continue;

		if (cards[i].number >= number) {
			count++;
		}
	}
	return count;
}

int Hand::countLowNearest(int suit, int value)
{
	int cnt = 0;
	for (int i = value - 1; i >= 0; i--) {

		if (findCard(suit, i) != INVALID) break;
		
		cnt++;
	}

	return cnt;
}

int Hand::countHighNearest(int suit, int value)
{
	int cnt = 0;
	for (int i = value + 1; i <= CARD_A; i++) {

		if (findCard(suit, i) != INVALID) break;

		cnt++;
	}
	
	return cnt;
}

int Hand::countLowBroken(int suit, int value)
{
	int minVal = INVALID;
	for (int i = 0; i < value; i++) {

		if (findCard(suit, i) == INVALID) continue;

		minVal = i;
		break;
	}

	if (minVal == INVALID) return 0;

	int cnt = 0;
	for (int i = minVal + 1; i < value; i++) {

		if (findCard(suit, i) != INVALID) continue;

		cnt++;
	}

	return cnt;		 
}

int Hand::countHighBroken(int suit, int value)
{
	int maxVal = INVALID;
	for (int i = CARD_A; i > value; i--) {

		if (findCard(suit, i) == INVALID) continue;

		maxVal = i;
		break;
	}

	if (maxVal == INVALID) return 0;

	int cnt = 0;
	for (int i = value + 1; i < maxVal; i++) {

		if (findCard(suit, i) != INVALID) continue;

		cnt++;
	}

	return cnt;
}

int Hand::countEqual(int number)
{
	int count = 0;
	for (int i = 0; i < CARDS_PER_PLAYER; i++) {

		if (cards[i].suit == NONE_SUIT) continue;

		if (cards[i].number == number) {
			count ++;
		}
	}
	return count;
}

int Hand::getTotalValue(int suit)
{
	int value = 0;
	int cnt = 0;
	for (int i = 0; i < MAX_NUMBER; i++) {

		int index = suit*MAX_NUMBER + i;
		if (g_bPlayed[index] == true) continue;

		cnt++;
		if (findCard(suit, i) == INVALID) continue;

		value += cnt;
	}

	return value;
}

bool Hand::getHigherPlayers(int suit, int number)
{
	bool bPlayed[MAX_NUMBER];
	
	//init played card info
	for (int i = 0; i < MAX_NUMBER; i++) {

		int index = suit*MAX_NUMBER + i;
		bPlayed[i] = g_bPlayed[index];
	}

	for (int i = 0; i < CARDS_PER_PLAYER; i++) {

		if (cards[i].suit == NONE_SUIT || cards[i].suit != suit) continue;

		bPlayed[cards[i].number] = true;
	}

	bool flag = false;
	for (int i = CARD_A; i > number; i--) {

		if (bPlayed[i] == true) continue;

		flag = true;
		break;
	}

	return flag;
}
