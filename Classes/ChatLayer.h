//
//  ChatLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 4/27/16.
//
//

#ifndef ChatLayer_hpp
#define ChatLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

class ChatLayer : public cocos2d::Layer, public cocos2d::ui::EditBoxDelegate
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
//    Layer* getLayer(){ return pCurLayer; };
    void showLayer();
    void hideLayer();
    
    void addNewMessage(std::string name, std::string text);
    
    // implement the "static create()" method manually
    CREATE_FUNC(ChatLayer);
    
    
    void update(float delta);
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
    virtual void editBoxEditingDidBegin(EditBox* editBox);
    virtual void editBoxEditingDidEnd(EditBox* editBox);
    virtual void editBoxTextChanged(EditBox* editBox, std::string &text);
    virtual void editBoxReturn(EditBox * editBox);
    
    virtual void editboxKeyboardWillShow(IMEKeyboardNotificationInfo& info);
    virtual void editboxKeyboardWillHide(IMEKeyboardNotificationInfo& info);
    
private:
    
    void onChatWordClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    
    void addDefaultWords();
    void updateHistoryBox(std::string msg);
    
    void addNewMessage2MainUser(std::string text);
    
    Point posOrigin;
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    Layout *panelMain;
    EditBox *pEditBox;
    ListView *lv_words;
    ListView *lv_chat;
    
    float durationTimer = 0;
};

#endif /* ChatLayer_hpp */
