//
//  GiftBonusLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 5/4/16.
//
//

#include "GiftBonusLayer.h"

#include "WebService.h"
#include "AppService.h"
#include "GameScene_Online.h"

//define string
std::string g_strGiftBonus[3][2] = {{"FREE POINTS FOR YOU!", "PUNTOS GRATIS PARA TI!"},
    {"10,000 FREE POINTS!", "10,000 PUNTOS GRATIS"},
    {"PLAY NOW", "JUEGA AHORA"}
};

Layer* GiftBonusLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool GiftBonusLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("GiftBonusLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    auto panelMain = (Layout*)rootNode->getChildByName("main");
    panelMain->setScaleX(g_fScaleWH);
    
    lb_title[0] = (Text*)panelMain->getChildByName("title");
    lb_title[1] = (Text*)panelMain->getChildByName("point");
    
    //main buttons
    for (int i = 0; i < 2; i++) {
        
        std::string strName = StringUtils::format("btn_%d", i);
        auto btnMain = static_cast<Button*>(panelMain->getChildByName(strName));
        btnMain->addTouchEventListener(CC_CALLBACK_2(GiftBonusLayer::onMainBtnClicked, this, i));
        
        if (i == 1) {
            
            lb_title[2] = (Text*)btnMain->getChildByName("text");
        }
    }
    
    //effect node
    node_effect = (Node*)panelMain->getChildByName("effectNode");
    node_effect->setScaleX(g_fScaleWH);
    auto panel_effect = (Layout*)node_effect->getChildByName("panel_effect");
    auto img_mark = (ImageView*)panel_effect->getChildByName("icon");
    img_mark->loadTexture("gift_mark.png", TextureResType::PLIST);
    
    pNodeAnimation = CSLoader::createTimeline("SpinWinNode.csb");
    pNodeAnimation->retain();
    pNodeAnimation->gotoFrameAndPause(0);
    
    updateString2Lang();
    
    return true;
}

void GiftBonusLayer::updateString2Lang()
{
    for (int i = 0; i < 3; i++) {
        
        lb_title[i]->setString(g_strGiftBonus[i][g_nLangType]);
    }
}

void GiftBonusLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {        
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        
        g_mainUserInfo.points += 10000; //FIRST_BONUSPOINT;
        WebService::getInstance()->updatePoint();
        
        if(btn_id == 0) { //close
            
            pDelegate->onUpdatedDone();
            
            hideLayer();
        }
        else if(btn_id == 1){ //play now            
            
            g_nBuyin = g_nBuyinList[0];
            //            if(g_mainUserInfo.points < g_nBuyin) {
            //
            //                pAlertLayer->showLayer("", "You have not enough points");
            //                //cocos2d::MessageBox("You have not enough points", "");
            //                return;
            //            }
            
            AppService::showCircleProgressbar();
            
            g_nPlayFee = g_nFeeList[0];
            g_nRoomType = FREE_ROOM;
            Scene *pScene = TransitionFade::create(0.5, GameScene_Online::createScene());
            Director::getInstance()->replaceScene(pScene);
        }
    }
}

void GiftBonusLayer::showLayer()
{
    node_effect->stopAllActions();
    node_effect->setVisible(true);
    node_effect->runAction(pNodeAnimation);
    pNodeAnimation->setTimeSpeed(0.3);
    pNodeAnimation->gotoFrameAndPlay(0, true);
    
    updateString2Lang();
    pCurLayer->setVisible(true);
}

void GiftBonusLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}
