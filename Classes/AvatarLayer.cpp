//
//  AvatarLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 5/3/16.
//
//

#include "AvatarLayer.h"
#include "WebService.h"
#include "Facebook.h"

//define string
std::string g_strAvatarLayer[3][2] = {{"SELECT AVATAR", "SELECCIONAR AVATAR"},
    {"New avatar is unlocked", "Has desbloqueado un avatar"},
    {"You don't have enough points", "No tienes suficientes puntos"}
};

Layer* AvatarLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool AvatarLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("AvatarLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    auto panelMain = (Layout*)rootNode->getChildByName("main");
    panelMain->setScaleX(g_fScaleWH);
    
    lb_title = (Text*)panelMain->getChildByName("title");
    
    //main buttons
    std::string strName = StringUtils::format("btn_%d", 0);
    auto btnMain = static_cast<Button*>(panelMain->getChildByName(strName));
    btnMain->addTouchEventListener(CC_CALLBACK_2(AvatarLayer::onMainBtnClicked, this, 0));
    
    sv_list = (ScrollView*)panelMain->getChildByName("sv_list");
    sv_list->setClippingEnabled(true);
    sv_list->setClippingType(Layout::ClippingType::SCISSOR);
    for (int i = 0; i < MAX_AVATARS; i++) {
        
        auto panel_item = (Layout*)sv_list->getChildByName(StringUtils::format("panel_%d", i));
        panel_item->addTouchEventListener(CC_CALLBACK_2(AvatarLayer::onMainBtnClicked, this, i+2));
        btn_avatar[i] = (Button*)panel_item->getChildByName("btn_price");
        btn_avatar[i]->addTouchEventListener(CC_CALLBACK_2(AvatarLayer::onMainBtnClicked, this, i+2));
        cb_avatar[i] = (Button*)panel_item->getChildByName("check");
        cb_avatar[i]->setTouchEnabled(false);
    }
    
    auto panel_fb = (Layout*)sv_list->getChildByName("panel");
    img_fb = (ImageView*)panel_fb->getChildByName("fb_photo");
    img_fb->setTouchEnabled(true);
    img_fb->addTouchEventListener(CC_CALLBACK_2(AvatarLayer::onMainBtnClicked, this, 1));
    cb_fb = (Button*)panel_fb->getChildByName("fb_check");
    cb_fb->setTouchEnabled(false);
    
    //clipping node
    auto clippingNode = ClippingNode::create();
    clippingNode->setAlphaThreshold(MASKING_THRESHOLD);
    clippingNode->setPosition(0, 0);
    clippingNode->setAnchorPoint(Vec2(0.0, 0.0));
    panel_fb->addChild(clippingNode);
    
    auto mask = Sprite::createWithSpriteFrameName("img_mask.png");
    float scaleX = (img_fb->getBoundingBox().size.width) / (float)mask->getBoundingBox().size.width;
    float scaleY = (img_fb->getBoundingBox().size.height) / (float)mask->getBoundingBox().size.height;
    mask->setScale(scaleX, scaleY);
    mask->setPosition(img_fb->getPosition());
    clippingNode->setStencil(mask);
    
    img_photo = ImageView::create("img_mask.png", TextureResType::PLIST);
    img_photo->setVisible(false);
    img_photo->ignoreContentAdaptWithSize(false);
    img_photo->setScale(scaleX, scaleY);
    img_photo->setPosition(img_fb->getPosition());
    clippingNode->addChild(img_photo);
    
    auto imgBorder = ImageView::create("border.png", TextureResType::PLIST);
    imgBorder->setScale(scaleX, scaleY);
    imgBorder->setPosition(img_fb->getPosition());
    panel_fb->addChild(imgBorder);
    
    clippingNode->setLocalZOrder(5);
    cb_fb->setLocalZOrder(50);
    
    //init alert layer
    pAlertLayer = new AlertLayer();
    pAlertLayer->createLayer(pCurLayer, 100, this);
    
    //init facebook connect layer
    pFBConnectLayer = new FBConnectLayer();
    pFBConnectLayer->createLayer(pCurLayer, 100, this);
    
    return true;
}

void AvatarLayer::selectItem(int index)
{
    //check unlock status, first
    if (unlock_status[index] == 0) {
        
        if (g_mainUserInfo.points < g_nAvatarPoint[index]) {
            
            pAlertLayer->showLayer("", g_strAvatarLayer[2][g_nLangType]);
            return;
        }
        else {
            
            g_mainUserInfo.points -= g_nAvatarPoint[index];
            unlock_status[index] = 1;
            
            setAvatarUnlock(index, 1);
            
            btn_avatar[index]->setVisible(false);
            cb_avatar[index]->setVisible(true);
            
            pAlertLayer->showLayer("", g_strAvatarLayer[1][g_nLangType]);
            
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

void AvatarLayer::onUpdatedDone()
{
    if (child_index == FBCONNECT_LAYER) {
        
        hideLayer();
        //Facebook::getInstance()->login();
    }
}

void AvatarLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {        
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        if(btn_id == 0) { //close
            
            //check current status
            if (cb_fb->isBright() == false) {
                
                g_mainUserInfo.photo_id = StringUtils::format("%d", select_index);
            }
            else {
                
                g_mainUserInfo.photo_id = Facebook::getInstance()->getUserId();
            }
            
            setSelectAvatar(select_index);
            WebService::getInstance()->updateAvatar();
            
            checkTotalPoint();
            
            pDelegate->child_index = AVATAR_LAYER;
            pDelegate->onUpdatedDone();
            hideLayer();
        }
        else if(btn_id == 1) { //fb check
            
            bool flag = cb_fb->isBright();
            if (flag == true) {
                
                cb_fb->setBright(!cb_fb->isBright());
                return;
            }

            if(g_bLoginFB == false) {
                
                pFBConnectLayer->showLayer();
                //Facebook::getInstance()->login();
            }
            else {
                
                cb_fb->setBright(!cb_fb->isBright());
            }
        }
        else {
            
           selectItem(btn_id-2);
        }
    }
}

void AvatarLayer::showLayer()
{   
    cb_fb->setBright(false);
    if (g_mainUserInfo.photo_id.length() > 6) {
        
        WebService::getInstance()->setProfileImage(g_mainUserInfo.photo_id, img_photo);
        cb_fb->setBright(true);
    }
    else  {
    
//        int index = atoi(g_mainUserInfo.photo_id.c_str());
//        setSelectAvatar(index);
        if(g_bLoginFB == true) {
            
            WebService::getInstance()->setProfileImage(Facebook::getInstance()->getUserId(), img_photo);
        }
    }
    
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
    
    lb_title->setString(g_strAvatarLayer[0][g_nLangType]);
    pCurLayer->setVisible(true);
}

void AvatarLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}
