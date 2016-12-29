//
//  ConnectingLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 5/26/16.
//
//

#ifndef ConnectingLayer_hpp
#define ConnectingLayer_hpp


#include "common.h"
#include "LayerDelegate.h"

class ConnectingLayer : public cocos2d::Layer
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    void showLayer();
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(ConnectingLayer);
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
private:
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    Text *lb_title;
    
    ActionTimeline *pAnimation;
};

#endif /* ConnectingLayer_hpp */
