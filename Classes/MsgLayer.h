//
//  MsgLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 6/7/16.
//
//

#ifndef MsgLayer_hpp
#define MsgLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

class MsgLayer : public cocos2d::Layer
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    Layer* getLayer(){ return pCurLayer; };
    void showLayer(std::string title, std::string msg);
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(MsgLayer);
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    Text *lb_msg;
    Text *lb_title;
    
    ScrollView *sv_msg;
};

#endif /* MsgLayer_hpp */
