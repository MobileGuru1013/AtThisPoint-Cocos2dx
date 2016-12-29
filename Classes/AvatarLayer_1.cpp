//
//  AvatarLayer_1.cpp
//  PointGame
//
//  Created by TianXinxing on 5/22/16.
//
//

#include "AvatarLayer_1.h"
#include "WebService.h"

//define string
std::string g_strAvatarLayer1[3][2] = {{"SELECT AVATAR", "DOMINA UN AVATAR"},
    {"New avatar is unlocked", "Has desbloqueado un avatar"},
    {"You don't have enough points", "No tienes suficientes puntos"}
};

Layer* AvatarLayer_1::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
{
    pParentLayer = parent;
    pDelegate = delegate;
    // 'layer' is an autorelease object
    pCurLayer = Layer::create();
    pCurLayer->setVisible(false);
    
    // add layer as a child to parent
    pParentLayer->addChild(pCurLayer, zorder);
    
    init();
    
    // return the scene
    return pCurLayer;
}

bool AvatarLayer_1::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("AvatarLayer_1.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    auto panelMain = (Layout*)rootNode->getChildByName("main");
    panelMain->setScaleX(g_fScaleWH);
    
    lb_title = (Text*)panelMain->getChildByName("title");
    
    //main buttons
    std::string strName = StringUtils::format("btn_%d", 0);
    auto btnMain = static_cast<Button*>(panelMain->getChildByName(strName));
    btnMain->addTouchEventListener(CC_CALLBACK_2(AvatarLayer_1::onMainBtnClicked, this, 0));
    
    sv_list = (ScrollView*)panelMain->getChildByName("sv_list");
    sv_list->setClippingEnabled(true);
    sv_list->setClippingType(Layout::ClippingType::SCISSOR);
    for (int i = 0; i < MAX_AVATARS; i++) {
        
        auto panel_item = (Layout*)sv_list->getChildByName(StringUtils::format("panel_%d", i));
        panel_item->addTouchEventListener(CC_CALLBACK_2(AvatarLayer_1::onMainBtnClicked, this, i+2));
        btn_avatar[i] = (Button*)panel_item->getChildByName("btn_price");
        btn_avatar[i]->addTouchEventListener(CC_CALLBACK_2(AvatarLayer_1::onMainBtnClicked, this, i+2));
        cb_avatar[i] = (Button*)panel_item->getChildByName("check");
        cb_avatar[i]->setTouchEnabled(false);
    }
    
    //init alert layer
    pAlertLayer = new AlertLayer();
    pAlertLayer->createLayer(pCurLayer, 100, this);
    
    //init facebook connect layer
    pFBConnectLayer = new FBConnectLayer();
    pFBConnectLayer->createLayer(pCurLayer, 100, this);
    
    return true;
}

void AvatarLayer_1::selectItem(int index)
{
    //check unlock status, first
    if (unlock_status[index] == 0) {
        
        if (g_mainUserInfo.points < g_nAvatarPoint[index]) {
            
            pAlertLayer->showLayer("", g_strAvatarLayer1[2][g_nLangType]);
            return;
        }
        else {
            
            g_mainUserInfo.points -= g_nAvatarPoint[index];
            unlock_status[index] = 1;
            
            setAvatarUnlock(index, 1);
            
            btn_avatar[index]->setVisible(false);
            cb_avatar[index]->setVisible(true);
            
            pAlertLayer->showLayer("", g_strAvatarLayer1[1][g_nLangType]);
            
            if (getLessGiftFlag() == false)
            {
                setLessGiftFlag(true);
                checkTotalPoint();
            }
        }
    }
    
    select_index = index;
    setSelectAvatar(index);
    //select current avatar
    for (int i = 0; i < MAX_AVATARS; i++) {
        
        if (i == index) {
            
            cb_avatar[i]->setBright(true);
        }
        else {
            
            cb_avatar[i]->setBright(false);
        }
    }
    
    if (g_bSound == true) {
        
        CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(StringUtils::format("avatar_%d.aac", select_index).c_str(), false);
    }
}

void AvatarLayer_1::onUpdatedDone()
{
    if (child_index == FBCONNECT_LAYER) {
        
        hideLayer();
        //Facebook::getInstance()->login();
    }
}

void AvatarLayer_1::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        if(btn_id == 0) { //close
            
            //check current status
            g_mainUserInfo.photo_id = StringUtils::format("%d", select_index);
            
            setSelectAvatar(select_index);
            WebService::getInstance()->updateAvatar();
            
            checkTotalPoint();
            
            pDelegate->child_index = AVATAR_LAYER;
            pDelegate->onUpdatedDone();
            hideLayer();
        }
        else {
            
            selectItem(btn_id-2);
        }
    }
}

void AvatarLayer_1::showLayer()
{
    select_index = getSelectAvatar();
    setAvatarUnlock(select_index, 1);
    
    for (int i = 0; i < MAX_AVATARS; i++) {
        
        unlock_status[i] = getAvatarUnlock(i);
        if (unlock_status[i] == 0) {
            
            cb_avatar[i]->setVisible(false);
            btn_avatar[i]->setVisible(true);
        }
        else {
            
            cb_avatar[i]->setVisible(true);
            btn_avatar[i]->setVisible(false);
        }
    }
    
    selectItem(select_index);
    
    lb_title->setString(g_strAvatarLayer1[0][g_nLangType]);
    
    pCurLayer->setVisible(true);
}

void AvatarLayer_1::hideLayer()
{
    pCurLayer->setVisible(false);
}
