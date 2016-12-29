//
//  PublicProfileLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 4/28/16.
//
//

#ifndef PublicProfileLayer_hpp
#define PublicProfileLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

class PublicProfileLayer : public cocos2d::Layer
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    //    Layer* getLayer(){ return pCurLayer; };
    void showLayer(USER_INFO info, int pos);
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(PublicProfileLayer);
    
    int showing_pos;
    USER_INFO user_info;
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onEmoticonBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    
    void updateEmoticons();
    void updateString2Lang();
    
    void getUserRank();
    void onGetUserRankDone(HttpClient* sender, HttpResponse* response);
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    ImageView *img_photo;
    ImageView *img_emoticonPanel;
    ImageView *img_emoticons[12];
    
    Text *lb_name;
    Text *lb_level;
    Text *lb_point;
    
    Text *lb_maxearning;
    Text *lb_tourplay;
    Text *lb_winpro;
    Text *lb_toprank;
    Text *lb_bantitle;
    
    Text *lb_text;
    Text *lb_title[8];
    
    Button *btn_action[4];

    int send_type;
};

#endif /* PublicProfileLayer_hpp */
