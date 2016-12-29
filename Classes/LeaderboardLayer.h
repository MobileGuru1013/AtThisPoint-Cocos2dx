//
//  LeaderboardLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 5/1/16.
//
//

#ifndef LeaderboardLayer_hpp
#define LeaderboardLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

class LeaderboardLayer : public cocos2d::Layer
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    //    Layer* getLayer(){ return pCurLayer; };
    void showLayer();
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(LeaderboardLayer);
    
    void updateString2Lang();
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    
    void selectTab(int index);
    
    void showFriendLeaderboard();
    void showGlobalLeaderboard();
    
    void getGlobalRankingList();
    void getFriendRankingList();
    
    void onGlobalRankingCompleted(HttpClient* sender, HttpResponse* response);
    void onFriendRankingCompleted(HttpClient* sender, HttpResponse* response);
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    Layout *panel_main[2];
    ListView *lv_list[2];
   
    Button *btn_tab[2];
    
    Text *lb_title[4];
    
    int select_index;
};

#endif /* LeaderboardLayer_hpp */
