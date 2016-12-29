//
//  FBBonusLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 5/6/16.
//
//

#ifndef FBBonusLayer_hpp
#define FBBonusLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

class FBBonusLayer : public cocos2d::Layer
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    //    Layer* getLayer(){ return pCurLayer; };
    void showLayer();
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(FBBonusLayer);
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    
    void updateString2Lang();
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    Node *node_effect;
    ActionTimeline *pNodeAnimation;
    
    Text *lb_title[5];
};


#endif /* FBBonusLayer_hpp */
