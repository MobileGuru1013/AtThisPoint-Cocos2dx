//
//  CustomTableLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 5/12/16.
//
//

#include "CustomTableLayer.h"

#include "AppService.h"
#include "GameScene_Online.h"

//define string
std::string g_strCustomTable[4][2] = {{"CUSTOM TABLE", "PERSONALIZAR MESA"},
    {"MAX POINTS", "PUNTOS MÁXIMOS"},
    {"STARTING RISK", "RIESGO INICIAL"},
    {"START", "EMPEZAR"}    
};

Layer* CustomTableLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool CustomTableLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("CustomTableLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    auto panelMain = (Layout*)rootNode->getChildByName("main");
    panelMain->setScaleX(g_fScaleWH);
    
    lb_title[0] = (Text*)panelMain->getChildByName("title");
    for (int i = 0; i < 2; i++) {
        
        lb_title[i+1] = (Text*)panelMain->getChildByName(StringUtils::format("comment_%d", i));
        lb_value[i] = (Text*)lb_title[i+1]->getChildByName("value");
    }
    
    //main buttons
    for (int i = 0; i < 2; i++) {
        
        std::string strName = StringUtils::format("btn_%d", i);
        auto btnMain = static_cast<Button*>(panelMain->getChildByName(strName));
        btnMain->addTouchEventListener(CC_CALLBACK_2(CustomTableLayer::onMainBtnClicked, this, i));
        
        if (i == 1) {
            
            lb_title[3] = (Text*)btnMain->getChildByName("text");
        }
    }
    
//    //value
//    for (int i = 0; i < 2; i++) {
//        
//        lb_value[i] = (Text*)panelMain->getChildByName(StringUtils::format("value_%d", i));
//    }
    //slider
    for (int i = 0; i < 2; i++) {
        
        pSlider[i] = (Slider*)panelMain->getChildByName(StringUtils::format("slider_%d", i));
        pSlider[i]->addEventListener(CC_CALLBACK_2(CustomTableLayer::onSliderEvent, this, i));
    }
    
    return true;
}

void CustomTableLayer::updateString2Lang()
{
    for (int i = 0; i < 4; i++) {
        
        lb_title[i]->setString(g_strCustomTable[i][g_nLangType]);
    }
}

void CustomTableLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
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
        else if(btn_id == 1){ //start game
            
            hideLayer();
            
            g_nBuyin = value[0];
            g_nPlayFee = value[1];
            
            pDelegate->child_index = CUSTOMTABLE_LAYER;
            pDelegate->onUpdatedDone();
        }
    }
}

void CustomTableLayer::onSliderEvent(Ref* sender, Slider::EventType type, int index)
{
    if (type == Slider::EventType::ON_PERCENTAGE_CHANGED) {
        
        float percent = 0.01f*pSlider[index]->getPercent();
        
        if (index == 0) {
            
            value[index] = 1000 + 99000 * percent;
        }
        else {
            
            value[index] = 20 + 9980 * percent;
        }
        
        lb_value[index]->setString(getNormalText2Number(value[index]));
    }
}

void CustomTableLayer::showLayer()
{
    pSlider[0]->setPercent(0);
    pSlider[1]->setPercent(0);
   
    value[0] = 1000;
    value[1] = 20;
    
    for (int i = 0; i < 2; i++) {
        
        lb_value[i]->setString(getNormalText2Number(value[i]));
    }
    
    updateString2Lang();
    pCurLayer->setVisible(true);
}

void CustomTableLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}
