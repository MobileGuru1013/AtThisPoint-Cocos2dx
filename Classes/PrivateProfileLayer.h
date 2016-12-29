//
//  PrivateProfileLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 4/27/16.
//
//

#ifndef PrivateProfileLayer_hpp
#define PrivateProfileLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

#include "AvatarLayer.h"
#include "FBConnectLayer.h"

class PrivateProfileLayer : public cocos2d::Layer, public LayerDelegate
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    //    Layer* getLayer(){ return pCurLayer; };
    void showLayer();
    void hideLayer();
  
    // implement the "static create()" method manually
    CREATE_FUNC(PrivateProfileLayer);
    
    void updateString2Lang();
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
    void onUpdatedDone();
    
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    
    void updatInfo();
    void getMyRank();
    void onGetMyRankDone(HttpClient* sender, HttpResponse* response);
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    ListView *lv_archives;
    
    ImageView *img_photo;
    
    Text *lb_title[6];
    
    Text *lb_id;
    Text *lb_name;
    Text *lb_level;
    Text *lb_levelpoint;
    Text *lb_point;
    Text *lb_fbtitle;
    
    Text *lb_rank;
    Text *lb_rankpoint;
    Text *lb_maxearning;
    Text *lb_tourplay;
    Text *lb_winpro;
    
    Text *lb_achivTitle[MAX_ARCHIVEMENTS];
    Text *lb_achivComment[MAX_ARCHIVEMENTS];
    Text *lb_archivLevel[MAX_ARCHIVEMENTS];
    LoadingBar *archiv_levelbar[MAX_ARCHIVEMENTS];
    ImageView *img_achivemark[MAX_ARCHIVEMENTS];
    
    LoadingBar *level_bar;
    
    AvatarLayer *pAvatarLayer;
    FBConnectLayer *pFBConnectLayer;
};

#endif /* PrivateProfileLayer_hpp */
