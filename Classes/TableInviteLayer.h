//
//  TableInviteLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 5/12/16.
//
//

#ifndef TableInviteLayer_hpp
#define TableInviteLayer_hpp

#include "common.h"
#include "LayerDelegate.h"
#include "AlertLayer.h"

class TableInviteLayer : public cocos2d::Layer, public cocos2d::ui::EditBoxDelegate, public LayerDelegate
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    //    Layer* getLayer(){ return pCurLayer; };
    void showLayer(int pos);
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(TableInviteLayer);
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
    void onUpdatedDone(){}
    
    //EditBox Delegate
    virtual void editBoxEditingDidBegin(EditBox* editBox);
    virtual void editBoxEditingDidEnd(EditBox* editBox);
    virtual void editBoxTextChanged(EditBox* editBox, std::string &text);
    virtual void editBoxReturn(EditBox * editBox);
    
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onInviteItemClicked(Ref* pSender, Widget::TouchEventType type, int index, CheckBox *pItem);
    
    void showInvitePanel();
    
    void updateString2Lang();
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    Layout *panel_list;
    Layout *panel_search;
    Layout *panelCheck;
    
    CheckBox *cb_all;
    Text *lb_status;
    
    Text *lb_title[4];
    
    EditBox *pEditBox;
    
    Button *btn_invite;
    
    ScrollView *sv_list;
    
    int position;
    
    std::vector<FRIEND_INFO> v_InvitableFrineds;
    
    AlertLayer *pAlertLayer;
};

#endif /* TableInviteLayer_hpp */
