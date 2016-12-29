#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "common.h"
#include "RegisterLayer.h"
#include "AlertLayer.h"

#include "PrivateProfileLayer.h"
#include "SocialLayer.h"
#include "LeaderboardLayer.h"
#include "SpinLayer.h"
#include "StoreLayer.h"
#include "GiftBonusLayer.h"
#include "FBConnectLayer.h"
#include "FBBonusLayer.h"
#include "FBInviteLayer.h"

#include "CarouselLayer.h"
#include "SettingLayer.h"

class HelloWorld : public cocos2d::Layer, public LayerDelegate
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    void onFBConnectDone();
    
    void showProfileInfo();
    void loginFacebook();
    void logoutFacebook();
    
    void showFBBonusLayer();
    void showFBInviteLayer();
    
    void playMusic();
    
    void actionNewMsg(std::string msg);
    void actionNewGift(std::string msg);
    void actionNewFriend(std::string msg);
    void actionNewBonus(std::string msg);
    void actionNewFreeSpin(std::string msg);
    
    void showAlerMsg(std::string title, std::string msg){
        
        pAlertLayer->showLayer(title, msg);
    }
    
    void updateString2Lang();
    
    void updatePushMark();
    
    
    virtual void updateFacebookLogin(float dt);
    void autoFacebookLogin();
    
private:
    
    void onUpdatedDone();
    void onGetFreesSpinDone();
    
    void update(float delta);
    
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event);
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    
    void showBonusNode();
    
    void arrangeTables();
    
//    void loadFBProfileImg(std::string user_id);
//    void onImgDownloadCompleted(HttpClient* sender, HttpResponse* response);
  
    Node *bonus_node;
    ActionTimeline *pNodeAnimation;
    
    Button *btn_fb;
    
    ImageView *img_photo;
    ImageView *img_levelbar;
    Text *lb_name;
    Text *lb_level;
    Text *lb_points;
    LoadingBar *level_bar;
    
    Layout *panel_profile;
    Layout *panel_table;
    Layout *panel_bonus;
    
    ImageView *img_bonustimer;
    TextBMFont *lb_bonustime;
    
    Text *lb_title[3];
    ImageView *img_tables[5];
    
    ImageView *img_soon;
    ImageView *img_push;
    
    AlertLayer *pAlertLayer;
    RegisterLayer *pRegisterLayer;
    
    PrivateProfileLayer *pPrivateProfileLayer;
    SocialLayer *pSocialLayer;
    LeaderboardLayer *pLeaderboardLayer;
    SpinLayer *pSpinLayer;
    StoreLayer *pStoreLayer;
    GiftBonusLayer *pGiftBonusLayer;
    FBConnectLayer *pFBConnectLayer;
    FBBonusLayer *pFBBonusLayer;
    FBInviteLayer *pFBInviteLayer;
    
    CarouselLayer *pCarouselLayer;
    SettingLayer *pSettingLayer;
    
    Node*         pAdsLayer;
    ImageView*    pAdsImageView;
    Layout*       pAdsDlg;
    void doAdvertisment();
    
    
    Text* lb_ribbon_1;
    Text* lb_ribbon_2;
};

#endif // __HELLOWORLD_SCENE_H__
