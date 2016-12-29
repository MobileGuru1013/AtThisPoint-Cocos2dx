//
//  FBBonusLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 5/6/16.
//
//

#include "FBBonusLayer.h"

#include "WebService.h"
#include "HelloWorldScene.h"

//define string
std::string g_strFBBonus[5][2] = {{"FACEBOOK LOGIN SUCCESS!", "HAS INICIADO SESIÓN CON ÉXITO"},
    {"CONGRATULATIONS!", "¡FELICIDADES!"},
    {"4,000 FREE POINTS!", "4,000 PUNTOS GRATIS"},
    {"FOR CONNECTING WITH YOUR FACEBOOK ACCOUNT!", "FOR CONECTARTE CON TU CUENTA DE FACEBOOK"},
    {"PLAY NOW", "JUGAR AHORA"}
};

Layer* FBBonusLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool FBBonusLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("FBBonusLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    auto panelMain = (Layout*)rootNode->getChildByName("main");
    panelMain->setScaleX(g_fScaleWH);
    
    lb_title[0] = (Text*)panelMain->getChildByName("title");
    lb_title[1] = (Text*)panelMain->getChildByName("text_0");
    lb_title[2] = (Text*)panelMain->getChildByName("point");
    lb_title[3] = (Text*)panelMain->getChildByName("text_1");
    
    //main buttons
    std::string strName = StringUtils::format("btn_%d", 0);
    auto btnMain = static_cast<Button*>(panelMain->getChildByName(strName));
    btnMain->addTouchEventListener(CC_CALLBACK_2(FBBonusLayer::onMainBtnClicked, this, 0));
    
    lb_title[4] = (Text*)btnMain->getChildByName("text");
    
    //effect node
    node_effect = (Node*)panelMain->getChildByName("effectNode");
    node_effect->setScaleX(g_fScaleWH);
    
    pNodeAnimation = CSLoader::createTimeline("SpinWinNode.csb");
    pNodeAnimation->retain();
    pNodeAnimation->gotoFrameAndPause(0);
    
    return true;
}

void FBBonusLayer::updateString2Lang()
{
    for (int i = 0; i < 5; i++) {
        
        lb_title[i]->setString(g_strFBBonus[i][g_nLangType]);
    }
}

void FBBonusLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {        
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        //g_mainUserInfo.points += 3000;
        //WebService::getInstance()->updatePoint();
        
        if(btn_id == 0) { //close
            
            g_mainUserInfo.points += FIRST_BONUSPOINT;
            WebService::getInstance()->updatePoint();
            
//          pDelegate->child_index = FBCONNECT_LAYER;
            pDelegate->onUpdatedDone();
            
            g_pHelloWorld->showFBInviteLayer();
            
            hideLayer();
        }
    }
}

void FBBonusLayer::showLayer()
{
    g_mainUserInfo.signup_flag = 0;
    
    node_effect->stopAllActions();
    node_effect->setVisible(true);
    node_effect->runAction(pNodeAnimation);
    pNodeAnimation->setTimeSpeed(0.3);
    pNodeAnimation->gotoFrameAndPlay(0, true);
    
    updateString2Lang();
    pCurLayer->setVisible(true);
}

void FBBonusLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}
