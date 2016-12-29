//
//  RegisterLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 4/14/16.
//
//

#ifndef RegisterLayer_hpp
#define RegisterLayer_hpp

#include "common.h"
#include "LayerDelegate.h"
#include "AlertLayer.h"

class RegisterLayer : public cocos2d::Layer, public cocos2d::ui::EditBoxDelegate, public LayerDelegate
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate* pDelegate);
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
    void showLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(RegisterLayer);
    
private:
    
    virtual void onUpdatedDone(){}
    
    virtual void editBoxEditingDidBegin(EditBox* editBox);
    virtual void editBoxEditingDidEnd(EditBox* editBox);
    virtual void editBoxTextChanged(EditBox* editBox, std::string &text);
    virtual void editBoxReturn(EditBox * editBox);
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onCheckBtnClicked(Ref *sender, Widget::TouchEventType type, int cb_index);
    void onLangBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void updateString();
    
    void selectIndex(int index);
    
    Button *btn_Check[3];
    EditBox *pEditBox;
    Button *btn_lang[2];
    
    LayerDelegate *pParentDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    AlertLayer *pAlertLayer;
    
    int select_index;
    
    //
    Text *lb_title;
    Text *lb_selLang;
    Button* btn_main;
    
    Text* lb_langEn;
    Text* lb_langSp;
};


#endif /* RegisterLayer_hpp */
