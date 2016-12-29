//
//  AvatarLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 5/3/16.
//
//

#ifndef AvatarLayer_hpp
#define AvatarLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

#include "AlertLayer.h"
#include "FBConnectLayer.h"

class AvatarLayer : public cocos2d::Layer, public LayerDelegate
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    //    Layer* getLayer(){ return pCurLayer; };
    void showLayer();
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(AvatarLayer);
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    void onUpdatedDone();
    
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    
    void selectItem(int index);
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    ImageView *img_photo;
    ImageView *img_fb;
    Button *cb_fb;
    
    Button *btn_avatar[MAX_AVATARS];
    Button *cb_avatar[MAX_AVATARS];
    
    Text *lb_title;
    
    int select_index;
    int unlock_status[MAX_AVATARS];
    
    ScrollView *sv_list;
    
    AlertLayer *pAlertLayer;
    FBConnectLayer *pFBConnectLayer;
    
};

#endif /* AvatarLayer_hpp */
