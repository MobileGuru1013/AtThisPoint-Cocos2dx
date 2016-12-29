//
//  SocialLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 4/30/16.
//
//

#ifndef SocialLayer_hpp
#define SocialLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

#include "FBConnectLayer.h"
#include "PublicProfileLayer.h"
#include "AlertLayer.h"
#include "InviteBonusLayer.h"

#include "MsgLayer.h"

//#include "MulColTableView.h"

class SocialLayer : public cocos2d::Layer, public cocos2d::ui::EditBoxDelegate, public LayerDelegate
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    //    Layer* getLayer(){ return pCurLayer; };
    void showLayer();
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(SocialLayer);
    
    void actionNewMsg();
    void actionNewGift();
    void actionNewFriend();
    
    void updateString2Lang();
    void updatePushMark();
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
    //EditBox Delegate
    virtual void editBoxEditingDidBegin(EditBox* editBox);
    virtual void editBoxEditingDidEnd(EditBox* editBox);
    virtual void editBoxTextChanged(EditBox* editBox, std::string &text);
    virtual void editBoxReturn(EditBox * editBox);
    
    virtual void onUpdatedDone();
    
   
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onFriendsItemClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onGiftItemClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onSearchItemClicked(Ref* pSender, Widget::TouchEventType type, int btn_id, Button *pItem);
    void onMessageItemClicked(Ref* pSender, Widget::TouchEventType type, int index, int response);
    void onInviteItemClicked(Ref* pSender, Widget::TouchEventType type, int index, CheckBox *pItem);
    void onMessagePanelClicked(Ref* pSender, Widget::TouchEventType type, int index);
    
    void setInvitableFriendPhoto(std::string url, ImageView *img_photo);
    
    void selectTab(int index);
    
    void showFriendsPanel();
    void showInvitePanel();
    void showGiftPanel();
    void showMessagePanel();
    void showSearchPanel();
    
    void searchUserList(std::string name);
    void onUserListDownloadCompleted(HttpClient* sender, HttpResponse* response);
    
    void getGiftList();
    void onGiftListDownloadCompleted(HttpClient* sender, HttpResponse* response);
    
    void getMsgList();
    void onMsgListDownloadCompleted(HttpClient* sender, HttpResponse* response);
    
    void onGetPublicProfileDone(HttpClient* sender, HttpResponse* response, int pos);
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    Layout *panelCheck;
    Layout *panel_search;
    Layout *panel_fb;
    Layout *panel_main[5];    
    Button *btn_tab[5];
    Button *btn_invite;
    
    Node *node_effect;
    ActionTimeline *pNodeAnimation;
    
    ImageView *img_giftpush;
    ImageView *img_msgpush;
    
    ImageView *img_search;
    Text *lb_status;
    
    Text *lb_title[8];
    
    ScrollView *sv_invite;
    ScrollView *sv_friends;
    ListView *lv_gifts;
    ListView *lv_messages;
    ScrollView *sv_searches;
    
    CheckBox *cb_all;
    
    EditBox *pEditBox;
    
    int cur_tabIndex;
    
    std::vector<FRIEND_INFO> v_InvitableFrineds;
    
    FBConnectLayer *pFBConnectLayer;
    PublicProfileLayer *pPublicProfileLayer;
    AlertLayer *pAlertLayer;
    InviteBonusLayer *pInviteBonusLayer;
    MsgLayer *pMsgLayer;
};

#endif /* SocialLayer_hpp */
