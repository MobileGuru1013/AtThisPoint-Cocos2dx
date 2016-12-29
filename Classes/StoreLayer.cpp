//
//  StoreLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 5/3/16.
//
//

#include "StoreLayer.h"
#include "HelloWorldScene.h"
#include "WebService.h"

#include "MyIAPListner.h"

//define string
std::string g_strStore[15][2] = {
    {"STORE", "TIENDA"},
    {"SPIN FOR JUST $1.99", "GIRA POR SOLO $1.99"},
    {"GET A NEW AVATAR", "DOMINA UN AVATAR"},
    {"WIN UP TO 450,000", "GANA HASTA 450,000"},
    {"15,000 POINTS", "15,000 PUNTOS"},
    {"40,000 POINTS", "40,000 PUNTOS"},
    {"100,000 POINTS", "100,000 PUNTOS"},
    {"150,000 POINTS", "150,000 PUNTOS"},
    {"450,000 POINTS", "450,000 PUNTOS"},
    {"1,500,000 POINTS", "1,500,000 PUNTOS"},
    {"33% More", "33% Más"},
    {"66% More", "66% Más"},
    {"99% More", "99% Más"},
    {"199% More", "199% Más"},
    {"298% More", "298% Más"}
};

Layer* StoreLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool StoreLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("StoreLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    lb_title[0] = (Text*)rootNode->getChildByName("title");
    
    auto btn_close = (Button*)rootNode->getChildByName("btn_0");
    btn_close->setScaleX(g_fScaleWH);
    btn_close->addTouchEventListener(CC_CALLBACK_2(StoreLayer::onMainBtnClicked, this, 0));
    
    auto panelMain = (Layout*)rootNode->getChildByName("main");
    panelMain->setScaleX(g_fScaleWH);
    //main buttons
    for (int i = 0; i < 7; i++) {
        
        std::string strName = StringUtils::format("btn_%d", i+1);
        auto btnMain = static_cast<Button*>(panelMain->getChildByName(strName));
        btnMain->addTouchEventListener(CC_CALLBACK_2(StoreLayer::onMainBtnClicked, this, i+1));
        
        if (i == 6) {
            
            lb_title[2] = (Text*)btnMain->getChildByName("text");
        } else {
         
            lb_title[i+4] = (Text*)btnMain->getChildByName("point");
            if (i > 0) {
            
                lb_title[i+9] = (Text*)btnMain->getChildByName("text");
            }
        }
    }
    
    auto img_spin = (ImageView*)panelMain->getChildByName("mark");
    img_spin->setTouchEnabled(true);
    lb_title[3] = (Text*)img_spin->getChildByName("text");
    
    auto panel_spin1 = (Layout*)panelMain->getChildByName("panel_spin1");
    panel_spin1->addTouchEventListener(CC_CALLBACK_2(StoreLayer::onMainBtnClicked, this, 8));
    
    auto panel_spin = (Layout*)panelMain->getChildByName("panel_spin");
    panel_spin->addTouchEventListener(CC_CALLBACK_2(StoreLayer::onMainBtnClicked, this, 8));
    
    lb_title[1] = (Text*)panel_spin->getChildByName("text");
    
    //effect node
    node_effect = (Node*)panel_spin->getChildByName("effectNode");
    node_effect->setScaleX(g_fScaleWH);
    auto panel_effect = (Layout*)node_effect->getChildByName("panel_effect");
    auto img_mark = (ImageView*)panel_effect->getChildByName("icon");
    img_mark->setVisible(false);
    
    pNodeAnimation = CSLoader::createTimeline("SpinWinNode.csb");
    pNodeAnimation->retain();
    pNodeAnimation->gotoFrameAndPause(0);
    
    //init alert layer
    pAlertLayer = new AlertLayer();
    pAlertLayer->createLayer(pCurLayer, 100, this);
    
    //init avatar layer
    pAvatarLayer = new AvatarLayer_1();
    pAvatarLayer->createLayer(pCurLayer, 100, this);
    
    updateString2Lang();
    
    return true;
}

void StoreLayer::updateString2Lang()
{
    for (int i = 0; i < 15; i++) {
        
        lb_title[i]->setString(g_strStore[i][g_nLangType]);
    }
}

void StoreLayer::onUpdatedDone()
{
    pDelegate->child_index = STORE_LAYER;
    pDelegate->onUpdatedDone();
}

void StoreLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
                
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        if(btn_id == 0) { //close
            
            hideLayer();
        }
        else if(btn_id == 1){ //3000 point
            
            sdkbox::IAP::purchase("buy_1");
            
//            g_mainUserInfo.points += 3000;
//            WebService::getInstance()->updatePoint();
//            
//            pAlertLayer->showLayer("", "You get 3,000 Points");
//            
//            pDelegate->onUpdatedDone();
        }
        else if(btn_id == 2){ //6500 point
            
            sdkbox::IAP::purchase("buy_2");
//            g_mainUserInfo.points += 6500;
//            WebService::getInstance()->updatePoint();
//            
//            pAlertLayer->showLayer("", "You get 6,500 Points");
//            
//            pDelegate->onUpdatedDone();
        }
        else if(btn_id == 3){ //14000 point
            
            sdkbox::IAP::purchase("buy_3");
//            g_mainUserInfo.points += 14000;
//            WebService::getInstance()->updatePoint();
//            
//            pAlertLayer->showLayer("", "You get 14,000 Points");
//            
//            pDelegate->onUpdatedDone();
        }
        else if(btn_id == 4){ //18000 point
            
            sdkbox::IAP::purchase("buy_4");
//            g_mainUserInfo.points += 18000;
//            WebService::getInstance()->updatePoint();
//            
//            pAlertLayer->showLayer("", "You get 18,000 Points");
//            
//            pDelegate->onUpdatedDone();
        }
        else if(btn_id == 5){ //38000 point
            
            sdkbox::IAP::purchase("buy_5");
//            g_mainUserInfo.points += 38000;
//            WebService::getInstance()->updatePoint();
//            
//            pAlertLayer->showLayer("", "You get 38,000 Points");
//            
//            pDelegate->onUpdatedDone();
        }
        else if(btn_id == 6){ //100000 point
            
            sdkbox::IAP::purchase("buy_6");
//            g_mainUserInfo.points += 100000;
//            WebService::getInstance()->updatePoint();
//            
//            pAlertLayer->showLayer("", "You get 100,000 Points");
//            
//            pDelegate->onUpdatedDone();
        }
        else if(btn_id == 7) { //avatar
            
            pAvatarLayer->showLayer();
        }
        else if(btn_id == 8){ //spin
            
            // sdkbox::IAP::purchase("buy_7");
            hideLayer();
            g_pSpinLayer->showLayer();
            
//            pDelegate->onGetFreesSpinDone();
//            hideLayer();
        }
    }
}

void StoreLayer::actionPurchaseDone()
{
    pDelegate->onUpdatedDone();
}

void StoreLayer::showLayer()
{
    node_effect->stopAllActions();
    node_effect->setVisible(true);
    node_effect->runAction(pNodeAnimation);
    pNodeAnimation->setTimeSpeed(0.3);
    pNodeAnimation->gotoFrameAndPlay(0, true);
    
    updateString2Lang();
    pCurLayer->setVisible(true);
}

void StoreLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}
