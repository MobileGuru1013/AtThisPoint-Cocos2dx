
#include <stdlib.h>
#include "Player.h"
#include "Table.h"

Player::Player()
{
    init();
}

Player::~Player()
{
    
}

void Player::init()
{
    flagEmpty = true;
    activeStatus = INVALID;
    
    user_id = "";
    name = "";
    photo_id = "";
    points = 0;
    level = 0;
    
    emoticon_timer = 0;
    
    emoticon_id = INVALID;
    gift_id = INVALID;
    
    position = INVALID;
    purple_card = INVALID;
    call_type = INVALID;
}
