//
//  SettingLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 5/11/16.
//
//

#ifndef SettingLayer_hpp
#define SettingLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

class SettingLayer : public cocos2d::Layer
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    //    Layer* getLayer(){ return pCurLayer; };
    void showLayer();
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(SettingLayer);
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onLangBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onCheckBoxClicked(Ref *pSender, CheckBox::EventType type, int index);
    
    
    void updateString2Lang();
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    Node *layerCredits;
    ActionTimeline *pNodeAnimation;
    Text *lb_creditsTitle;
    Layout *panelCredits[2];
    
    Button *btn_lang[2];
    CheckBox *cb_setting[4];
    
    Text *lb_title[11];
};


#endif /* SettingLayer_hpp */
