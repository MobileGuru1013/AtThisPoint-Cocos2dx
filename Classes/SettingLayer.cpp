//
//  SettingLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 5/11/16.
//
//

#include "SettingLayer.h"

#include "HelloWorldScene.h"
#include "WebService.h"

//define string
std::string g_strSetting[11][2] = {{"SETTINGS", "AJUSTES"},
    {"SOUND", "SONIDO"},
    {"BACKGROUND MUSIC", "MÚSICA DE FONDO"},
    {"NOTIFICATION", "NOTIFICACIONES"},
    {"QUICK PLAY", "JUGADA RÁPIDA"},
    {"SELECT LANGUAGE", "SELECCIONAR LENGUAJE"},
    {"ENGLISH", "INGLÉS"},
    {"SPANISH", "ESPAÑOL"},
    {"TERMS & PRIVACY", "TÉRMINOS Y PRIVACIDAD"},
    {"WEBSITE", "SITIO WEB"},
    {"CREDITS", "CRÉDITOS"}
};

Layer* SettingLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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
	
bool
 SettingLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("SettingLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    lb_title[0] = (Text*)rootNode->getChildByName("title");
    for (int i = 0; i < 5; i++) {
        
        lb_title[i+1] = (Text*)rootNode->getChildByName(StringUtils::format("text_%d", i));
    }
    
    //main buttons
    for (int i = 0; i < 4; i++) {
        
        std::string strName = StringUtils::format("btn_%d", i);
        auto btnMain = static_cast<Button*>(rootNode->getChildByName(strName));
        btnMain->setScaleX(g_fScaleWH);
        btnMain->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onMainBtnClicked, this, i));
        
        if(i > 0) {
            
            lb_title[i+7] = (Text*)btnMain->getChildByName("text");
        }
    }
    
    //lang radio buttons
    for (int i = 0; i < 2; i++) {
        
        auto panel_lang = (Layout*)rootNode->getChildByName(StringUtils::format("panel_lang%d", i));
        btn_lang[i] = (Button*)panel_lang->getChildByName("btn_lang");
        btn_lang[i]->setScaleX(g_fScaleWH);
        btn_lang[i]->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onLangBtnClicked, this, i));
        panel_lang->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onLangBtnClicked, this, i));
        
        lb_title[i+6] = (Text*)panel_lang->getChildByName("text");
    }
    
    for (int i = 0; i < 4; i++) {
        
        cb_setting[i] = (CheckBox*)rootNode->getChildByName(StringUtils::format("check_%d", i));
        cb_setting[i]->setScaleX(g_fScaleWH);
        cb_setting[i]->addEventListener(CC_CALLBACK_2(SettingLayer::onCheckBoxClicked, this, i));
    }
    
    //init credits layer
    layerCredits = CSLoader::createNode("CreditsLayer.csb");
    pCurLayer->addChild(layerCredits, 10);
    layerCredits->setVisible(false);
    
    lb_creditsTitle = (Text*)layerCredits->getChildByName("title");
    for (int i = 0; i < 2; i++) {
        
        panelCredits[i] = (Layout*)layerCredits->getChildByName(StringUtils::format("panel_%d", i));
     }
    
    pNodeAnimation = CSLoader::createTimeline("CreditsLayer.csb");
    pNodeAnimation->retain();
    pNodeAnimation->gotoFrameAndPause(0);

    auto btnClose = (Button*)layerCredits->getChildByName("btn_0");
    btnClose->addTouchEventListener(CC_CALLBACK_2(SettingLayer::onMainBtnClicked, this, 4));
    
    return true;
}

void SettingLayer::updateString2Lang()
{
    for (int i = 0; i < 11; i++) {
        
        lb_title[i]->setString(g_strSetting[i][g_nLangType]);
    }
}

void SettingLayer::onCheckBoxClicked(Ref *pSender, CheckBox::EventType type, int index)
{
    if (g_bSound == true) {
        
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
    }
    if (type == CheckBox::EventType::SELECTED) {
        
        if (index == 0) { //sound
            
            g_bSound = true;
            
        }
        else if (index == 1) { //music
            
            g_bMusic = true;
            if(g_pHelloWorld != NULL) {
                
                g_pHelloWorld->playMusic();
            }
        }
        else if (index == 2) { //push
            
            g_mainUserInfo.push_flag = 1;
        }
        else if (index == 3) { //quick play
            
            g_bQplay = true;
        }
    }
    else {
        
        if (index == 0) { //sound
            
            g_bSound = false;
        }
        else if (index == 1) { //music
            
            g_bMusic = false;
            //CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
            CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        }
        else if (index == 2) { //push
            
            g_mainUserInfo.push_flag = 1;
        }
        else if (index == 3) { //quick play
            
            g_bQplay = false;
        }
    }
}

void SettingLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        if(btn_id == 0) { //close
            
            cocos2d::UserDefault::getInstance()->setBoolForKey(SOUND, g_bSound);
            cocos2d::UserDefault::getInstance()->setBoolForKey(MUSIC, g_bMusic);
            cocos2d::UserDefault::getInstance()->setBoolForKey(QPLAY, g_bQplay);
            cocos2d::UserDefault::getInstance()->setIntegerForKey(LANG, g_nLangType);
            
            WebService::getInstance()->updatePush();
            hideLayer();
        }
        else if(btn_id == 1){ //terms & privacy
            
            if (g_nLangType == 0) {
            
                cocos2d::Application::getInstance()->openURL("http://www.thegameatthepoint.com/privacy");
            } else {
            
                cocos2d::Application::getInstance()->openURL("http://www.thegameatthepoint.com/es/privacidad");
            }
        }
        else if(btn_id == 2) { //website
            
            cocos2d::Application::getInstance()->openURL("http://www.thegameatthepoint.com");
        }
        else if(btn_id == 3) { //credits
            
            if (g_nLangType == 0) {
                
                lb_creditsTitle->setString("CREDITS");
            } else {
                lb_creditsTitle->setString("CRÉDITOS");
            }
            panelCredits[g_nLangType]->setVisible(true);
            panelCredits[1-g_nLangType]->setVisible(false);
            
            layerCredits->stopAllActions();
            layerCredits->setVisible(true);
            layerCredits->runAction(pNodeAnimation);
            pNodeAnimation->setTimeSpeed(0.1);
            pNodeAnimation->gotoFrameAndPlay(0, true);
            
            layerCredits->setVisible(true);
        }
        else if(btn_id == 4) { //credits
            
            layerCredits->setVisible(false);
        }
    }
}

void SettingLayer::onLangBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        g_nLangType = btn_id;
        btn_lang[g_nLangType]->setBright(true);
        btn_lang[1-g_nLangType]->setBright(false);
        
        updateString2Lang();
        g_pHelloWorld->updateString2Lang();
    }
}

void SettingLayer::showLayer()
{
    //sound
    cb_setting[0]->setSelected(g_bSound);
    //music
    cb_setting[1]->setSelected(g_bMusic);
    //push
    cb_setting[2]->setSelected(g_mainUserInfo.push_flag);
    //quick play
    cb_setting[3]->setSelected(g_bQplay);
    
    btn_lang[g_nLangType]->setBright(true);
    btn_lang[1-g_nLangType]->setBright(false);
    
    updateString2Lang();
    pCurLayer->setVisible(true);
}

void SettingLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}
