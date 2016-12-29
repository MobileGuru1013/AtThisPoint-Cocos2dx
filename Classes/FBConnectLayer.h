//
//  FBConnectLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 5/4/16.
//
//

#ifndef FBConnectLayer_hpp
#define FBConnectLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

class FBConnectLayer : public cocos2d::Layer
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    //    Layer* getLayer(){ return pCurLayer; };
    void showLayer();
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(FBConnectLayer);
    
    void updateString2Lang();
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    Node *node_effect;
    ActionTimeline *pNodeAnimation;
    
    Text *lb_title[3];
};

#endif /* FBConnectLayer_hpp */
