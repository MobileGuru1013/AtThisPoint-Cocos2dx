//
//  InviteBonusLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 5/7/16.
//
//

#include "InviteBonusLayer.h"

#include "WebService.h"
#include "HelloWorldScene.h"

//define string
std::string g_strInviteBonus[4][2] = {{"CONGRATULATIONS!", "¡FELICIDADES!"},
    {"YOU GOT 4,000 REFERRAL POINTS", "HAS OBTENIDO 4,000 PUNTOS DE REFERIDO"},
    {"GET 4,000 FOR EACH NEW FRIENDS!", "OBTEN 4,000 POR CADA AMIGO REFERIDO"},
    {"INVITE FRIENDS", "INVITAR AMIGOS"}
};

Layer* InviteBonusLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool InviteBonusLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("InviteBonusLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    auto panelMain = (Layout*)rootNode->getChildByName("main");
    panelMain->setScaleX(g_fScaleWH);
    
    lb_title[0] = (Text*)panelMain->getChildByName("title");
    lb_title[1] = (Text*)panelMain->getChildByName("point");
    lb_title[2] = (Text*)panelMain->getChildByName("text");
    
    //main buttons
    for (int i = 0; i < 2; i++) {
        
        std::string strName = StringUtils::format("btn_%d", i);
        auto btnMain = static_cast<Button*>(panelMain->getChildByName(strName));
        btnMain->addTouchEventListener(CC_CALLBACK_2(InviteBonusLayer::onMainBtnClicked, this, i));
        
        if (i == 1) {
            
            lb_title[3] = (Text*)panelMain->getChildByName("text");
        }
    }
    
    //effect node
    node_effect = (Node*)panelMain->getChildByName("effectNode");
    node_effect->setScaleX(g_fScaleWH);
    
    pNodeAnimation = CSLoader::createTimeline("SpinWinNode.csb");
    pNodeAnimation->retain();
    pNodeAnimation->gotoFrameAndPause(0);
    
    return true;
}

void InviteBonusLayer::updateString2Lang()
{
    for (int i = 0; i < 4; i++) {
        
        lb_title[i]->setString(g_strInviteBonus[i][g_nLangType]);
    }
}

void InviteBonusLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
                
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        
        if(btn_id == 0) { //close
            
            hideLayer();
        }
        else {
            
            pDelegate->child_index = INVITEBONUS_LAYER;
            pDelegate->onUpdatedDone();
            hideLayer();
        }
    }
}

void InviteBonusLayer::showLayer()
{
    node_effect->stopAllActions();
    node_effect->setVisible(true);
    node_effect->runAction(pNodeAnimation);
    pNodeAnimation->setTimeSpeed(0.3);
    pNodeAnimation->gotoFrameAndPlay(0, true);
    
    updateString2Lang();
    pCurLayer->setVisible(true);
}

void InviteBonusLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}
