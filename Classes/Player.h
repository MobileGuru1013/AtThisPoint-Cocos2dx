
#ifndef __PointGame__Player__
#define __PointGame__Player__

#include <stdio.h>

#include "Card.h"
#include "common.h"

class Player
{
public:
    
    Player();
    ~Player();
    
    void init();

	std::string user_id;
    std::string name;
    std::string photo_id;
    int level;
    
    int gift_id;
    int emoticon_id;
    
    float emoticon_timer;
    
    float points;
    float betting;
    
    int call_type;
    
	int position;
    
    bool flagEmpty;
    int activeStatus;

    //int player_id;
    
    int purple_card;
    
    USER_INFO info;
};

#endif /* defined(__PointGame__Player__) */
