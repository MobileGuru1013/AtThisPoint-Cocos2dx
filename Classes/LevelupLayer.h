//
//  LevelupLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 4/28/16.
//
//

#ifndef LevelupLayer_hpp
#define LevelupLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

class LevelupLayer : public cocos2d::Layer
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    //    Layer* getLayer(){ return pCurLayer; };
    void showLayer(int level);
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(LevelupLayer);
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    Layout *panelMain;
    ImageView *img_spin;
    Text *lb_spin;
    Text *lb_level;
    
    Text *lb_title[3];
    
    Point posOrigin;
    Size szPanel;
};


#endif /* LevelupLayer_hpp */
