#ifndef __PointGame__Table__
#define __PointGame__Table__

#include "common.h"
#include "Card.h"

class Table
{
public:

	Table();
	~Table();

	static Table* getInstance();

	void init();	
	int addCard(Card &new_card);
	int countCommonCards();
	int findCard(int suit, int number);

	int countTotal();
	int countPlayedNumber(int number);
	int countNumber(int number);
		
	int total_score[2];
	int round_score[2];

	int last_index;

	std::vector<Card> vTakenPile[2];

private:

	static Table*_instance;
};

#endif /* defined(__PointGame__Table__) */
