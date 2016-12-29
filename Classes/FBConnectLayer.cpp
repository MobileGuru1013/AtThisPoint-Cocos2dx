//
//  FBConnectLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 5/4/16.
//
//

#include "FBConnectLayer.h"

#include "HelloWorldScene.h"

//define string
std::string g_strFBConnect[3][2] = {{"CONNECT TO FACEBOOK!", "CONÉCTATE A FACEBOOK!"},
    {"GET 4,000 FREE POINTS", "OBTÉN 4,000 PUNTOS GRATIS"},
    {"WE'LL NEVER POST WITHOUT YOUR PERMISSION", "NUNCA POSTEAREMOS SIN TU PERMISO"}
};

Layer* FBConnectLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool FBConnectLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("FBConnectLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    lb_title[2] = (Text*)rootNode->getChildByName("text");
    
    auto panelMain = (Layout*)rootNode->getChildByName("main");
    panelMain->setScaleX(g_fScaleWH);
    
    lb_title[0] = (Text*)panelMain->getChildByName("title");
    lb_title[1] = (Text*)panelMain->getChildByName("point");
    
    //main buttons
    for (int i = 0; i < 2; i++) {
        
        std::string strName = StringUtils::format("btn_%d", i);
        auto btnMain = static_cast<Button*>(panelMain->getChildByName(strName));
        btnMain->addTouchEventListener(CC_CALLBACK_2(FBConnectLayer::onMainBtnClicked, this, i));
    }
    
    //effect node
    node_effect = (Node*)panelMain->getChildByName("effectNode");
    node_effect->setScaleX(g_fScaleWH);
   
    pNodeAnimation = CSLoader::createTimeline("SpinWinNode.csb");
    pNodeAnimation->retain();
    pNodeAnimation->gotoFrameAndPause(0);
        
    updateString2Lang();
    
    return true;
}

void FBConnectLayer::updateString2Lang()
{
    for (int i = 0; i < 3; i++) {
        
        lb_title[i]->setString(g_strFBConnect[i][g_nLangType]);
    }
}

void FBConnectLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {        
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        //g_mainUserInfo.points += 3000;
        //WebService::getInstance()->updatePoint();
        
        if(btn_id == 0) { //close
            
            hideLayer();
        }
        else if(btn_id == 1){ //facebook login
            
            pDelegate->child_index = FBCONNECT_LAYER;
            pDelegate->onUpdatedDone();
            
            g_pHelloWorld->loginFacebook();
            
            hideLayer();
        }
    }
}

void FBConnectLayer::showLayer()
{
    node_effect->stopAllActions();
    node_effect->setVisible(true);
    node_effect->runAction(pNodeAnimation);
    pNodeAnimation->setTimeSpeed(0.3);
    pNodeAnimation->gotoFrameAndPlay(0, true);
    
    updateString2Lang();
    pCurLayer->setVisible(true);
}

void FBConnectLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}
