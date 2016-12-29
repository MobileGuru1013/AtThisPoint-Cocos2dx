//
//  CustomTableLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 5/12/16.
//
//

#ifndef CustomTableLayer_hpp
#define CustomTableLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

class CustomTableLayer : public cocos2d::Layer
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    //    Layer* getLayer(){ return pCurLayer; };
    void showLayer();
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(CustomTableLayer);
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onSliderEvent(Ref* sender, Slider::EventType type, int index);
    
    void updateString2Lang();
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    Slider *pSlider[2];
    Text *lb_value[2];
    
    Text *lb_title[4];
    
    int value[2];
};

#endif /* CustomTableLayer_hpp */
