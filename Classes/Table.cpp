#include "Table.h"

Table* Table::_instance = NULL;

Table* Table::getInstance()
{
	if (_instance == NULL) {

		_instance = new Table();
	}

	return _instance;
}

Table::Table()
{
	init();
}

Table::~Table()
{

}

void Table::init()
{
	last_index = INVALID;

	for (int i = 0; i < 2; i++) {

		round_score[i] = 0;
		vTakenPile[i].clear();
	}
}

int Table::addCard(Card &new_card)
{
	return INVALID;
}

int Table::countCommonCards()
{
	int cnt = 0;
//	for (int i = 0; i < MAX_DISCARDS_NUM; i++)
//	{
//		if (common_cards[i].suit == NONE_SUIT) continue;
//
//		cnt++;
//	}

	return cnt;
}

// Find position of a card
// Return -1 if not found
int Table::findCard(int suit, int number)
{
//	for (int i = 0; i < MAX_DISCARDS_NUM; i++) {
//		if (common_cards[i].suit == suit && common_cards[i].number == number) {
//
//			return i;
//		}
//	}
	return INVALID;
}

int Table::countPlayedNumber(int number)
{
	int cnt = 0;
	for (int i = 0; i < MAX_SUIT; i++) {

		int index = i*MAX_NUMBER + number;
		if (g_bPlayed[index] == false) continue;

		cnt++;
	}

	return cnt;
}

int Table::countNumber(int number)
{
	int cnt = 0;
	for (int i = 0; i < MAX_SUIT; i++) {

		int index = findCard(i, number);
		if (index == INVALID) continue;

		cnt++;
	}

	return cnt;
}

int Table::countTotal()
{
	int cnt = 0;
//	for (int i = 0; i < MAX_DISCARDS_NUM; i++) {
//
//		if (common_cards[i].suit == NONE_SUIT) continue;
//
//		cnt++;
//	}

	return cnt;
}