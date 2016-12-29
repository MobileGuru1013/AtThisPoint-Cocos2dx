
#ifndef __PointGame__LayerDelegate__
#define __PointGame__LayerDelegate__

#include "common.h"

class LayerDelegate
{
public:
    
    virtual void onUpdatedDone() = 0;
    
    virtual void onChatMessageDone(std::string msg){}
    virtual void onGetFreesSpinDone(){}
    virtual void onGiftSelectDone(int pos, int index){}
    virtual void onEmoticonSelectDone(int pos, int index){}
    
    int child_index = -1;
	int res_value = INVALID;
};

#endif /* defined(__PointGame__LayerDelegate__) */
