//
//  StoreLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 5/3/16.
//
//

#ifndef StoreLayer_hpp
#define StoreLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

#include "AlertLayer.h"
#include "AvatarLayer_1.h"

class StoreLayer : public cocos2d::Layer, public LayerDelegate
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    //    Layer* getLayer(){ return pCurLayer; };
    void showLayer();
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(StoreLayer);
    
    void actionPurchaseDone();
    void updateString2Lang();
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
    void onUpdatedDone();
    
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    AlertLayer *pAlertLayer;
    
    Node *node_effect;
    ActionTimeline *pNodeAnimation;
    
    Text *lb_title[15];
    
    AvatarLayer_1 *pAvatarLayer;
};

#endif /* StoreLayer_hpp */
