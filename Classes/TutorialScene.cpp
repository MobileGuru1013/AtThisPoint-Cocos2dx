//
//  TutorialScene.cpp
//  PointGame
//
//  Created by TianXinxing on 5/2/16.
//
//

#include "TutorialScene.h"

#include "WebService.h"
#include "AppService.h"

#include "HelloWorldScene.h"
#include "GameScene_Online.h"



//define string
std::string g_strTutorialScene[23][2] = {{"Watch out", "¡Cuidado"},
    {"The magic animals\n escaped!", "¡Se escaparon los\n animales mágicos!"},
    {"We need to control\n them, before they\n destroy the theater", "Hay que dominarlos\nantes que destruyan\n el teatro"},
    {"We are gonna need\npoints. A lot of points.", "Vamos a necesitar\n puntos.\n Muchos puntos."},
    {"Yes, like those.\nThey are pure magic,\n very powerful.", "Sí, como esos.\nSon magia pura\n muy poderosa."},
    {"To win points you\n need to risk them", "Para ganar puntos\ntienes que arriesgar\n puntos"},
    {"Let's try !", "¡Hagamos un intento!"},
    {"First, take\na purple card", "Primero toma\nuna carta\n púrpura"},
    {"Now we play some\n points.", "Ahora juguemos\nalgunos puntos."},
    {"How many points\ndo you dare to risk?", "¿Cuántos puntos\nte atreves\na arriesgar?"},
    {"Well done!!!\nThe green card\nyou get is inside\nthe range of your\npurple card.", "¡Bien hecho!\nLa carta verde está\ndentro del rango de\nla carta púrpura"},
    {"If the green card\ndon't is inside the\nrange your risk points\n will going to the pot", "Si la carta verde no\nestá dentro del rango\nlo que arriesgues irá \ndirecto al pote"},
    {"You could pass\nyour turn if you don't\nlike your purple card", "Puedes pasar el\nturno cuando no te\nguste tu carta púrpura"},
    {"Control the magic\nanimals in every of\ntheir expressions", "Controla a los\nanimales mágicos y\ntodas sus\n expresiones"},
    {"It's time.\nYou are ready\nfor the challenge!!!", "Es tiempo ¡Ya\nestás listo para el\n reto!"},
    {"Move the\nselector to\nchoose the\npoints to risk", "Mueve el\nselector para\nescoger la\n cantidad"},
    {"Get my card and\nI take the points\nthat you risk", "Saca mi carta y me\nllevo los puntos que\narriesgaste al pote"},
    {"SKIP", "OMITIR"},
    {"FOLD", "PASAR"},
    {"BET", "ARRIESGAR"},
    {"ARE YOU UP TO THE CHALLENGE?", "¿ESTÁS LISTO PARA EL RETO?"},
    {"start with these 10,000 points", "Empieza con estos 10,000 puntos"},
    {"PLAY NOW", "JUGAR AHORA"}
};

std::string g_imgTutorialScene[2][2] = {
    {"text_win.png", "_text_win.png"},
    {"text_loose.png", "_text_loose.png"}
};

TutorialScene* g_TutorialLayer = NULL;

Scene* TutorialScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = TutorialScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

bool TutorialScene::init()
{   //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    //touch enable
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(TutorialScene::onTouchesBegan, this);
    listener->onTouchesEnded = CC_CALLBACK_2(TutorialScene::onTouchesEnded, this);
    listener->onTouchesMoved = CC_CALLBACK_2(TutorialScene::onTouchesMoved, this);
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    auto rootNode = CSLoader::createNode("TutorialScene.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    this->addChild(rootNode);
    
    for (int i = 0; i < 2; i++) {
        
        img_monkey[i] = (ImageView*)rootNode->getChildByName(StringUtils::format("monkey_%d", i));
        img_monkey[i]->setScaleX(g_fScaleWH);
        img_monkey[i]->setVisible(false);
        
        img_monkeybubble[i] = (ImageView*)rootNode->getChildByName(StringUtils::format("mbubble_%d", i));
        //img_monkeybubble[i]->setScaleX(g_fScaleWH);
        img_monkeybubble[i]->setVisible(false);
    }
    
    for (int i = 0; i < 4; i++) {
        
        auto btnMain = (Button*)rootNode->getChildByName(StringUtils::format("btn_%d", i));
        btnMain->setScaleX(g_fScaleWH);
        
        if (i == 3) {
            
            lbl_skip = (Text*)btnMain->getChildByName("text");
            
            
            btnMain->addTouchEventListener(CC_CALLBACK_2(TutorialScene::onMainBtnClicked, this, i));
            lb_title[0] = (Text*)btnMain->getChildByName("text");
        }
    }
    
    
    
    auto img_lights = (ImageView*)rootNode->getChildByName("lights_panel");
    for (int i = 0; i < 23; i++) {
        
        auto paricle_lights = (ParticleSystem*)img_lights->getChildByName(StringUtils::format("particle_%d", i));
        paricle_lights->stopSystem();
    }
    
    int i = 0;
    auto player_panel = static_cast<Layout*>(rootNode->getChildByName(StringUtils::format("panel_%d", i)));
    player_panel->setScaleX(g_fScaleWH);
    //player_panel->setVisible(false);
    
    panelProfile = player_panel;
    
    auto panelInfo = static_cast<Layout*>(player_panel->getChildByName("main"));
    //panelInfo->addTouchEventListener(CC_CALLBACK_2(GameScene_Online::onProfileClicked, this, i));
    panelInfo->setLocalZOrder(3);
    auto img_active = static_cast<ImageView*>(panelInfo->getChildByName("active"));
    //img_active[i]->setLocalZOrder(5);
    img_active->setVisible(false);
    auto img_level = static_cast<ImageView*>(panelInfo->getChildByName("level_mark"));
    //img_level->setLocalZOrder(1);
    auto img_dealer = static_cast<ImageView*>(panelInfo->getChildByName("dealer"));
    //img_dealer[i]->setLocalZOrder(1);
    img_dealer->setVisible(false);
    
    lb_name = static_cast<Text*>(panelInfo->getChildByName("name"));
    lb_name->setString("");
    //lb_name[i]->setLocalZOrder(1);
    lb_level = static_cast<Text*>(panelInfo->getChildByName("level"));
    lb_level->setString("");
    //lb_level[i]->setLocalZOrder(2);
    lb_point = static_cast<Text*>(panelInfo->getChildByName("point"));
    lb_point->setString("");
    //lb_point[i]->setLocalZOrder(1);
    
    auto img_invite = static_cast<ImageView*>(player_panel->getChildByName("bg"));
    img_invite->setVisible(false);
    
    //clipping node
    auto clippingNode = ClippingNode::create();
    clippingNode->setAlphaThreshold(MASKING_THRESHOLD);
    clippingNode->setPosition(0, 0);
    clippingNode->setAnchorPoint(Vec2(0, 0));
    rootNode->addChild(clippingNode);
    //clippingNode->setLocalZOrder(0);
    
    auto mask = Sprite::createWithSpriteFrameName("img_mask.png");
    float scaleX = (player_panel->getBoundingBox().size.width) / (float)mask->getBoundingBox().size.width;
    float scaleY = (player_panel->getBoundingBox().size.height) / (float)mask->getBoundingBox().size.height;
    mask->setScale(scaleX, scaleY);
    mask->setPosition(player_panel->getPosition());
    clippingNode->setStencil(mask);
    
    img_photo = ImageView::create("img_mask.png", TextureResType::PLIST);
    //img_photo->setVisible(false);
    float scaleX1 = (player_panel->getBoundingBox().size.width) / (float)img_photo->getBoundingBox().size.width;
    float scaleY1 = (player_panel->getBoundingBox().size.height) / (float)img_photo->getBoundingBox().size.height;
    img_photo->setScale(scaleX1, scaleY1);
    img_photo->setPosition(player_panel->getPosition());
    clippingNode->addChild(img_photo);
    
    auto imgBorder = ImageView::create("border.png", TextureResType::PLIST);
    //imgBorder->setVisible(false);
    imgBorder->setScale(scaleX, scaleY);
    imgBorder->setPosition(player_panel->getPosition());
    rootNode->addChild(imgBorder);
    
    clippingNode->setLocalZOrder(0);
    player_panel->setLocalZOrder(3);
    
    /////////////////////////////////////////////////
    //show cards
    for (int i = 0; i < 2; i++) {
        
        auto img_showcard = static_cast<ImageView*>(rootNode->getChildByName(StringUtils::format("pcard_%d", i)));
        posPCards[i] = img_showcard->getPosition();
        if(i == 1) {
            
            img_pcard = img_showcard;
            img_pcard->setPosition(posPCards[0]);
        }
        
        img_showcard = static_cast<ImageView*>(rootNode->getChildByName(StringUtils::format("gcard_%d", i)));
        posGCards[i] = img_showcard->getPosition();
        if(i == 1) {
            
            img_gcard = img_showcard;
            img_gcard->setPosition(posGCards[0]);
        }
    }
    
    //hands image
    img_hands = static_cast<ImageView*>(rootNode->getChildByName(StringUtils::format("handcard_%d", i)));
    img_hands->setVisible(false);
    
    img_chips = static_cast<ImageView*>(rootNode->getChildByName(StringUtils::format("chip_%d", i)));
    img_chips->setVisible(false);
    lb_chips = static_cast<Text*>(img_chips->getChildByName("point"));
    
    //deck image
    img_deck = static_cast<ImageView*>(rootNode->getChildByName("deck"));
    img_deck->setVisible(true);
    
    //back cards
    img_backCards = static_cast<ImageView*>(rootNode->getChildByName(StringUtils::format("deck_%d", i)));
    img_backCards->setVisible(true);
    
    //pot
    img_pot = static_cast<ImageView*>(rootNode->getChildByName("pot"));
    img_pot->setVisible(false);
    lb_pot = static_cast<Text*>(img_pot->getChildByName("point"));
    
    //init win and lose mark
    img_winmark = (ImageView*)rootNode->getChildByName("img_winner");
    img_winmark->setScaleX(g_fScaleWH);
    img_winmark->setLocalZOrder(3);
    img_winmark->setVisible(false);
    
    auto img_wintext = (ImageView*)img_winmark->getChildByName("label_mark");
    img_wintext->loadTexture(g_imgTutorialScene[0][g_nLangType], TextureResType::PLIST);
    
    for (int i = 0; i < 2; i++) {
        
        img_bubble[i] = (ImageView*)rootNode->getChildByName(StringUtils::format("bubble_%d", i+1));
        img_bubble[i]->setVisible(false);
        img_bubble[i]->setLocalZOrder(5);
        
        lb_imgbubble[i] = (Text*)img_bubble[i]->getChildByName("text");
    }
    
    for (int i =  0; i < 15; i++) {
        
        lb_bubbles[i] = (Text*)rootNode->getChildByName(StringUtils::format("text_%d", i));
        lb_bubbles[i]->setVisible(false);
        lb_bubbles[i]->setLocalZOrder(7);
    }
    
    //action panel
    panelAction = static_cast<Layout*>(rootNode->getChildByName("action_panel"));
    panelAction->setScaleX(g_fScaleWH);
    panelAction->setVisible(false);
    for (int i = 0; i < 2; i++) {
        
        std::string strName = StringUtils::format("btn_%d", i);
        auto btnMain = static_cast<Button*>(panelAction->getChildByName(strName));
        btnMain->addTouchEventListener(CC_CALLBACK_2(TutorialScene::onMainBtnClicked, this, i));
        
        btn_action[i] = btnMain;
        lb_title[i+1] = (Text*)btnMain->getChildByName("text");
        if (i == 0)
        {
            lbl_fold = lb_title[i+1];
            lb_title[i+1]->setString(g_strTutorialScene[18][g_nLangType]);
            cocos2d::log("%s", lb_title[i+1]->getString().c_str());
        }
        else if (i == 1)
        {
            lb_title[i+1]->setString(g_strTutorialScene[19][g_nLangType]);
            lbl_bet = lb_title[i+1];
        }
        
        particle_focus[i+2] = static_cast<ParticleSystem*>(panelAction->getChildByName(StringUtils::format("particle_focus%d", i)));
        //particle_focus[i]->setScaleX(g_fScaleWH);
        //particle_focus[i+2]->setLocalZOrder(40);
        particle_focus[i+2]->stopSystem();
    }
    
    //betting panel
    panelBetting = static_cast<Layout*>(rootNode->getChildByName("betting_panel"));
    panelBetting->setLocalZOrder(50);
    panelBetting->setVisible(false);
    slider_betting = static_cast<Slider*>(panelBetting->getChildByName("slider"));
    slider_betting->addEventListener(CC_CALLBACK_2(TutorialScene::onSliderEvent, this));
    auto imgBubble = static_cast<ImageView*>(panelBetting->getChildByName("bubble"));
    lb_betting = (Text*)(imgBubble->getChildByName("lb_value"));
    auto cb_betting = (Button*)(panelBetting->getChildByName("check"));
    cb_betting->addTouchEventListener(CC_CALLBACK_2(TutorialScene::onBettingChecked, this, 0));
    auto btn_maxbet = (Button*)(panelBetting->getChildByName("max_bet"));
    btn_maxbet->addTouchEventListener(CC_CALLBACK_2(TutorialScene::onBettingChecked, this, 1));
    
    //init chips for animation
    auto chipNode = CSLoader::createNode("ChipNode.csb");
    this->addChild(chipNode);
    imgAnimationChips = static_cast<ImageView*>(chipNode->getChildByName("chip"));
    imgAnimationChips->setScaleX(g_fScaleWH);
    imgAnimationChips->setVisible(false);
    lb_AnimationChips = static_cast<Text*>(imgAnimationChips->getChildByName("point"));
    
    for (int i = 0; i < 5; i++) {
        
        if(i == 2 || i == 3) continue;
        particle_focus[i] = static_cast<ParticleSystem*>(rootNode->getChildByName(StringUtils::format("particle_focus%d", i)));
        //particle_focus[i]->setScaleX(g_fScaleWH);
        particle_focus[i]->setLocalZOrder(40);
        particle_focus[i]->stopSystem();
    }
    
    for(int i = 0; i < 4; i++) {
        
        particle_chip[i] = static_cast<ParticleSystem*>(rootNode->getChildByName(StringUtils::format("particle_chip%d", i)));
        particle_chip[i]->setLocalZOrder(40);
        particle_chip[i]->stopSystem();
    }
    for(int i = 0; i < 4; i++) {
        
        particle_win[i] = static_cast<ParticleSystem*>(rootNode->getChildByName(StringUtils::format("particle_win%d", i)));
        particle_win[i]->setLocalZOrder(40);
        particle_win[i]->stopSystem();
    }
    
    //init bonus layer
    layerBonus = CSLoader::createNode("FirstBonusLayer.csb");
    layerBonus->setVisible(false);
    
    auto panel = (Layout*)layerBonus->getChildByName("main");
    
    auto lb_bonustitle = (Text*)panel->getChildByName("title");
    lb_bonustitle->setString(g_strTutorialScene[20][g_nLangType]);
    lbl_bonus_title = lb_bonustitle;
    
    auto lb_bonustext = (Text*)panel->getChildByName("text");
    lb_bonustext->setString(g_strTutorialScene[21][g_nLangType]);
    lbl_bonus_prize = lb_bonustext;
    
    auto btn_close = (Button*)panel->getChildByName("btn_0");
    btn_close->setTitleText(g_strTutorialScene[22][g_nLangType]);
    btn_bonus_playnow = btn_close;
    
    btn_close->addTouchEventListener(CC_CALLBACK_2(TutorialScene::onMainBtnClicked, this, 2));
    //rootNode->setScale(getScaleValX(), getScaleValY());
    this->addChild(layerBonus);
    
    //effect panel
    node_effect = (Node*)panel->getChildByName("effectNode");
    //node_effect->setScaleX(g_fScaleWH);

    pNodeAnimation = CSLoader::createTimeline("SpinWinNode.csb");
    pNodeAnimation->retain();
    pNodeAnimation->gotoFrameAndPause(0);
    
    for (int i = 0; i < 3; i++) {
        
        lb_title[i]->setString(g_strTutorialScene[i+17][g_nLangType]);
    }
    
    //check if first login
     g_TutorialLayer = this;
    if(g_mainUserInfo.signup_flag == 1) {
        
        g_mainUserInfo.signup_flag = 0;
        isNewRegister = true;
        pRegisterLayer = new RegisterLayer();
        pRegisterLayer->createLayer(this, 10, this);
        pRegisterLayer->showLayer();
        
        return true;
    }

    isNewRegister = false;
    showPlayer();
    
   
    
    return true;
}

void TutorialScene::updateString()
{
    
    g_TutorialLayer->lbl_skip->setString(g_strTutorialScene[17][g_nLangType]);
    g_TutorialLayer->lbl_fold->setString(g_strTutorialScene[18][g_nLangType]);
    g_TutorialLayer->lbl_bet->setString(g_strTutorialScene[19][g_nLangType]);
    
    g_TutorialLayer->lbl_bonus_title->setString(g_strTutorialScene[20][g_nLangType]);
    g_TutorialLayer->lbl_bonus_prize->setString(g_strTutorialScene[21][g_nLangType]);
    g_TutorialLayer->btn_bonus_playnow->setTitleText(g_strTutorialScene[22][g_nLangType]);

    
    /*
    auto rootNode = CSLoader::createNode("TutorialScene.csb");
    auto btnMain = (Button*)rootNode->getChildByName("btn_3");
    lbl_skip = (Text*)btnMain->getChildByName("text");
    lbl_skip->setString(g_strTutorialScene[17][g_nLangType]);
    
    Layout* pnl = static_cast<Layout*>(rootNode->getChildByName("action_panel"));
    btnMain = (Button*)pnl->getChildByName("btn_0");
    lbl_fold = (Text*)btnMain->getChildByName("text");
    lbl_fold->setString(g_strTutorialScene[18][g_nLangType]);
    
    btnMain = (Button*)pnl->getChildByName("btn_1");
    lbl_bet = (Text*)btnMain->getChildByName("text");
    lbl_bet->setString(g_strTutorialScene[19][g_nLangType]);*/
}
void TutorialScene::showFocusParticle(int index)
{
    for (int i = 0; i < 5; i++) {
        
        if(i == index) {
            
            particle_focus[i]->resetSystem();
        }
        else {
            
            particle_focus[i]->stopSystem();
        }
    }
}

void TutorialScene::onUpdatedDone()
{
    showPlayer();
}

void TutorialScene::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        if(btn_id == 0) { //fold
            
            
        }
        else if(btn_id == 1){ //betting
            
            //CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
            
            panelAction->setVisible(false);
            
            showFocusParticle(-1);
            
            cur_step++;
            run();
            
            this->runAction(Sequence::create( DelayTime::create(1.5f),
                                              CallFunc::create([=]()->void{
                
                                                max_betting = MIN(gamePlayers[MAIN_USER].points, game_pot);
                                                min_betting = MIN(max_betting, g_nPlayFee);
                                                
                                                cur_betting = min_betting;
                                                slider_betting->setPercent(0);
                                                
                                                updateBettingLabel();
                                                
                                                //img_bubble[1]->setVisible(false);
                                                //img_bubble[2]->setVisible(false);
                                                panelAction->setVisible(false);
                
                                                lb_imgbubble[0]->setString(g_strTutorialScene[15][g_nLangType]);
                                                img_bubble[0]->setVisible(true);
                                                panelBetting->setVisible(true);
                                            }),
                                            NULL));
            
        }
        else if(btn_id == 2) { //close
            
            AppService::showCircleProgressbar();
            
            g_nPlayFee = g_nFeeList[0];
            g_nBuyin = g_nBuyinList[0];
            /*
            g_nRoomType = FREE_ROOM;
            Scene *pScene = TransitionFade::create(0.5, GameScene_Online::createScene());
            Director::getInstance()->replaceScene(pScene);
            */
            Scene *pScene = TransitionFade::create(0.5, HelloWorld::createScene());
            Director::getInstance()->replaceScene(pScene);
        }
        else if(btn_id == 3) { //skip button
            
            if (isNewRegister == true) {
                
                showBonusLayer();
            }
            else {
                
                Scene *pScene = TransitionFade::create(0.5, HelloWorld::createScene());
                Director::getInstance()->replaceScene(pScene);
            }
        }
   }
}

void TutorialScene::showPlayer()
{
    game_pot = 5000;
    
    gamePlayers[MAIN_USER].flagEmpty = false;
    gamePlayers[MAIN_USER].user_id = g_mainUserInfo.user_id;
    gamePlayers[MAIN_USER].name = g_mainUserInfo.user_name;
    gamePlayers[MAIN_USER].photo_id = g_mainUserInfo.photo_id;
    gamePlayers[MAIN_USER].level = g_mainUserInfo.level;
    gamePlayers[MAIN_USER].points = 5000;
    
    std::string strName = gamePlayers[MAIN_USER].name.substr(0, 10);
    getUppercaseString(strName);
    lb_name->setString(strName);
    lb_level->setString(StringUtils::format("%d", gamePlayers[MAIN_USER].level+1));
    lb_point->setString(getNormalText2Number(gamePlayers[MAIN_USER].points));
    
    WebService::getInstance()->setProfileImage(gamePlayers[MAIN_USER].photo_id, img_photo);
    
    img_monkey[0]->setVisible(true);
    img_monkeybubble[0]->setVisible(true);
    
    lb_bubbles[0]->setString(StringUtils::format("%s\n %s !",  g_strTutorialScene[0][g_nLangType].c_str(), gamePlayers[MAIN_USER].name.c_str()));
    lb_bubbles[0]->setVisible(true);
    
    cur_step = 0;
    
    scheduleUpdate();
}

void TutorialScene::onSliderEvent(Ref* sender, Slider::EventType type)
{
    if (type == Slider::EventType::ON_PERCENTAGE_CHANGED) {
        
        float percent = 0.01f*slider_betting->getPercent();
        cur_betting = (int)(min_betting + (max_betting-min_betting)*percent);
        gamePlayers[MAIN_USER].betting = cur_betting;
        
        updateBettingLabel();
    }
}

void TutorialScene::onBettingChecked(Ref *sender, Widget::TouchEventType type, int btn_id)
{
    //if(timer_index != MAIN_USER) return;
    
    if (type == Widget::TouchEventType::ENDED) {
        
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        
        if (btn_id == 1) { //if max betting
            
            cur_betting = max_betting;
        }
        
        gamePlayers[MAIN_USER].betting = cur_betting;
        //sendInfo();
        
        //stopTimerCountdown();
        img_bubble[0]->setVisible(false);
        panelBetting->setVisible(false);
        
        setChipTextureWithPoint(imgAnimationChips, gamePlayers[MAIN_USER].betting);
        setChipTextureWithPoint(img_chips, gamePlayers[MAIN_USER].betting);
        imgAnimationChips->setPosition(panelProfile->getPosition());
        imgAnimationChips->setVisible(true);
        lb_AnimationChips->setString(StringUtils::format("%.0f", gamePlayers[MAIN_USER].betting));
        
        Point tar_pos = img_chips->getPosition();
        imgAnimationChips->runAction(Sequence::create( DelayTime::create(0.05),
                                                                    MoveTo::create(0.5f, tar_pos),
                                                                    CallFunc::create([=]()->void{
                                                                        imgAnimationChips->setVisible(false);
                                                                        img_chips->setVisible(true);
                                                                        lb_chips->setString(StringUtils::format("%.0f", gamePlayers[MAIN_USER].betting));
            
                                                                        cur_step++;
                                                                        run();
                                                                    }),
                                                                    NULL));

    }
}

void TutorialScene::updatePot()
{
    if(game_pot > 0) {
        
        lb_pot->setString(getNormalText2Number(game_pot));
        
        int index = 0;
        if(game_pot > 15000) {
            
            index = 8;
        }
        else if(game_pot > 10000) {
            
            index = 7;
        }
        else if(game_pot > 7000) {
            
            index = 6;
        }
        else if(game_pot > 4000) {
            
            index = 5;
        }
        else if(game_pot > 2000) {
            
            index = 4;
        }
        else if(game_pot > 1000) {
            
            index = 3;
        }
        else if(game_pot > 500) {
            
            index = 2;
        }
        else if(game_pot > 200) {
            
            index = 1;
        }
        img_pot->loadTexture(StringUtils::format("pot_%d.png", index), TextureResType::PLIST);
        img_pot->setVisible(true);
        return;
    }
    
    img_pot->setVisible(false);
}

void TutorialScene::updateBettingLabel()
{
    lb_betting->setString(getNormalText2Number(cur_betting));
}

void TutorialScene::setChipTextureWithPoint(ImageView *img_chip, float point)
{
    std::string file;
    int index = 0;
    if(point > 1000) {
        
        index = 6;
    }
    else if(point > 100) {
        
        index = 5;
    }
    else if(point > 50) {
        
        index = 4;
    }
    else if(point > 20) {
        
        index = 3;
    }
    else if(point > 10) {
        
        index = 2;
    }
    else if(point > 5) {
        
        index = 1;
    }
    
    img_chip->loadTexture(StringUtils::format("chip_%d.png", index), TextureResType::PLIST);
}

void TutorialScene::actionAnimation2Pot(int index)
{
    imgAnimationChips->setVisible(false);
    
    game_pot += g_nPlayFee;
    updatePot();
    lb_point->setString(getNormalText2Number(gamePlayers[index].points));
    
    if(g_bSound == true)
    {
        //CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("risking.aac", false);
    }
}

void TutorialScene::risking()
{
    g_nPlayFee = 100;
    gamePlayers[MAIN_USER].points -= g_nPlayFee;
    
    Vec2 tar_pos = img_pot->getPosition();
    
    lb_AnimationChips->setString(StringUtils::format("%d", g_nPlayFee));
    setChipTextureWithPoint(imgAnimationChips, g_nPlayFee);
    imgAnimationChips->setPosition(panelProfile->getPosition());
    imgAnimationChips->setOpacity(255);
    imgAnimationChips->setVisible(true);
    
    imgAnimationChips->runAction(Sequence::create( DelayTime::create(0.05),
                                                     MoveTo::create(0.5f, tar_pos),
                                                     //FadeOut::create(0.0),
                                                     CallFunc::create(CC_CALLBACK_0(TutorialScene::actionAnimation2Pot, this, MAIN_USER)),
                                                     NULL));
//    if(g_bSound == true)
//    {
//        //CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
//        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("chip collection.aac", false, 1, 0, 10);
//    }
}

void TutorialScene::dealing()
{
    int cnt = 0;
    Point tar_pos = img_hands->getPosition();
    img_backCards->runAction(Sequence::create(
         DelayTime::create(cnt*0.3),
         MoveTo::create(0.4f, tar_pos),
         //FadeOut::create(0.0),
         CallFunc::create([=]()->void{
        
                img_backCards->setVisible(false);
                img_hands->setVisible(true);
        
                if (g_bSound) {
                    
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("pcard_deal.aac", false);
                }
            }),
     //CallFunc::create(CC_CALLBACK_0(GameScene_Online::FlipActionEffect, this, img_hands[i], file)),
     NULL));
    
    DelayTime *delayTime = CCDelayTime::create(0.2);
    CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(TutorialScene::showPurpleCacrd, this, MAIN_USER));
    Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
    this->runAction(delayAct);
}

void TutorialScene::actionSetCardTexture(ImageView *imgCard, std::string file)
{
    imgCard->loadTexture(file, TextureResType::PLIST);
}

void TutorialScene::FlipActionEffect(ImageView *imgCard, std::string file)
{
    imgCard->setVisible(true);
    float scaleX = imgCard->getScaleX();
    float scaleY = imgCard->getScaleY();
    Sequence * act1 = Sequence::create(
                                       ScaleTo::create(0.2f, 0, scaleY),
                                       CallFunc::create(CC_CALLBACK_0(TutorialScene::actionSetCardTexture, this, imgCard, file)),
                                       ScaleTo::create(0.2f, scaleX, scaleY),
                                       NULL);
    imgCard->runAction(act1);
}

void TutorialScene::showPurpleCacrd(int pos)
{
    if (g_bSound == true) {
        
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("pcard_flip.aac", false);
    }
    //show card on table
    int pcard_index = 9; //gamePlayers[pos].purple_card%10;
    std::string file = StringUtils::format("pcard_%d.png", pcard_index);
    FlipActionEffect(img_hands, file);
    
    //show top card
    file = StringUtils::format("pscard_%d.png", pcard_index);
    img_pcard->loadTexture(file, TextureResType::PLIST);
    img_pcard->runAction(Sequence::create(
                                          DelayTime::create(0.1),
                                          MoveTo::create(0.2f, posPCards[1]),
                                          NULL));
    showFocusParticle(1);
    //img_bubble[0]->setVisible(true);
    
//    cur_step++;
//    lb_bubbles[cur_step]->setVisible(true);
}

void TutorialScene::showGreenCacrd()
{
    //show card on table
    int gcard_index = 3;
//    int green_card = 3;
//    if(green_card > 39) {
//        
//        gcard_index = 0;
//    }
//    else {
//        
//        gcard_index = (green_card%10) + 1;
//    }
    
    std::string file = StringUtils::format("gscard_%d.png", gcard_index);

    //show top card
    img_gcard->loadTexture("gscard_back.png", TextureResType::PLIST);
    img_gcard->runAction(Sequence::create(
                                          DelayTime::create(0.1),
                                          MoveTo::create(0.2f, posGCards[1]),
                                          CallFunc::create(CC_CALLBACK_0(TutorialScene::FlipActionEffect, this, img_gcard, file)),
                                          NULL));
    
    DelayTime *delayTime = DelayTime::create(0.2);
    CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(TutorialScene::actionPlayDone, this));
    Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
    this->runAction(delayAct);
}

void TutorialScene::showMonkeyCacrd()
{
    int gcard_index = 0;
    std::string file = StringUtils::format("gscard_%d.png", gcard_index);
    
    //show top card
    img_gcard->loadTexture(file, TextureResType::PLIST);
    img_gcard->setPosition(posGCards[1]);
    
    lb_imgbubble[1]->setString(g_strTutorialScene[16][g_nLangType]);
    img_bubble[1]->setVisible(true);
    showFocusParticle(4);
//    img_gcard->loadTexture("gscard_back.png", TextureResType::PLIST);
//    img_gcard->runAction(Sequence::create(
//                                          DelayTime::create(0.1),
//                                          MoveTo::create(0.2f, posGCards[1]),
//                                          CallFunc::create(CC_CALLBACK_0(TutorialScene::FlipActionEffect, this, img_gcard, file)),
//                                          NULL));
}

void TutorialScene::actionPlayDone()
{
    img_chips->setVisible(false);
    
    //winner mark
    img_winmark->setPosition(panelProfile->getPosition());
    img_winmark->setVisible(true);
    
    //particles
    //chip animation
    for(int i = 0; i < 4; i++) {
        
        particle_chip[i]->resetSystem();
    }
    //shine aniamtion
    particle_win[0]->resetSystem();
    
    //move chip from pot to player
    Vec2 tar_pos = panelProfile->getPosition();
    
    imgAnimationChips->setPosition(img_pot->getPosition());
    imgAnimationChips->setOpacity(255);
    imgAnimationChips->setVisible(true);
    lb_AnimationChips->setString(StringUtils::format("%.0f", gamePlayers[MAIN_USER].betting));
    
    imgAnimationChips->runAction(Sequence::create( DelayTime::create(0.05),
                                                                MoveTo::create(0.8f, tar_pos),
                                                                //FadeOut::create(0.0),
                                                                CallFunc::create(CC_CALLBACK_0(TutorialScene::actionWinAnimation, this)),
                                                                NULL));
    
    if(g_bSound == true) {
        
        int pcard_index = 9; //gamePlayers[MAIN_USER].purple_card%10;
        
        if ((game_pot - gamePlayers[MAIN_USER].betting) < 0) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("clean_pot.aac", false);
        }
        else if (gamePlayers[MAIN_USER].betting > 3000) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("win_3000.aac", false);
        }
        else if (pcard_index < 5) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("win_1.aac", false);
        }
        else {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("win_2.aac", false);
        }
    }
}

void TutorialScene::initTextAnimation()
{
    curValue = 0;
    deltaValue = 0;
    cntTextTimer = 0;
    posTextAnimation = INVALID;
    flagTextAnimation = false;
}

void TutorialScene::actionWinAnimation()
{
    imgAnimationChips->setVisible(false);
    
    //set parameter for text animation
    initTextAnimation();
    curValue = gamePlayers[MAIN_USER].points;
    deltaValue = gamePlayers[MAIN_USER].betting/100.0f;
    posTextAnimation = MAIN_USER;
    flagTextAnimation = true;
    /////////////////////////////////////////////
    
    game_pot -= gamePlayers[MAIN_USER].betting;
    updatePot();
    
    gamePlayers[MAIN_USER].points += gamePlayers[MAIN_USER].betting;
    gamePlayers[MAIN_USER].betting = 0;
    //updatePlayerPointLabel(pos);
    
    DelayTime *delayTime = DelayTime::create(3);
    CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(TutorialScene::actionPlayCompleted, this));
    Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
    this->runAction(delayAct);
}

void TutorialScene::actionPlayCompleted()
{
    hideShowCard();
    img_winmark->setVisible(false);
    for(int i = 0; i < 4; i++) {
        
        particle_chip[i]->stopSystem();
    }
    //shine aniamtion
    particle_win[0]->stopSystem();
    
//    if (g_bMusic) {
//        
//        CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
//        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("lobby.aac", true);
//    }
    
    isTouchEnable = true;
//    cur_step++;
//    run();
}

void TutorialScene::hideShowCard()
{
    img_pcard->setPosition(posPCards[0]);
    img_gcard->setPosition(posGCards[0]);
    img_hands->setVisible(false);
}

void TutorialScene::showBonusLayer()
{
    node_effect->stopAllActions();
    node_effect->setVisible(true);
    node_effect->runAction(pNodeAnimation);
    pNodeAnimation->gotoFrameAndPlay(0, true);
    
    layerBonus->setVisible(true);
}

void TutorialScene::run()
{
    if (cur_step == 13) {
        
        panelAction->setVisible(false);
        showMonkeyCacrd();
        return;
    }
    
    //swap monkey
    if(cur_step == 6) {
        
        img_monkey[0]->setVisible(false);
        img_monkeybubble[0]->setVisible(false);
        
        img_monkey[1]->setVisible(true);
        img_monkeybubble[1]->setVisible(true);
    }
    
    for (int i = 0; i < 15; i++) {
        
        if (i == cur_step) {
        
            lb_bubbles[i]->setString(g_strTutorialScene[i][g_nLangType]);
            lb_bubbles[i]->setVisible(true);
        }
        else {
            
            lb_bubbles[i]->setVisible(false);
        }
    }
    
    if (cur_step == 4) { //show pot
        
        updatePot();
        showFocusParticle(0);
    }
    else if (cur_step == 5) { //risking fee
        
        showFocusParticle(-1);
        risking();
    }
    else if (cur_step == 7) { //dealing
        
        dealing();
    }
//    else if (cur_step == 8) { //bubble for purple card
//        
//        //img_monkeybubble[1]->setVisible(false);
//        //img_bubble[0]->setVisible(true);
//    }
    else if (cur_step == 8) { //action panel
        
        isTouchEnable = false;
        
        //img_bubble[0]->setVisible(false);
        this->runAction(Sequence::create( DelayTime::create(1.5f),
                                         CallFunc::create([=]()->void{
            
                                                //img_bubble[0]->setVisible(false);
                                                //img_monkeybubble[1]->setVisible(true);
            
                                                showFocusParticle(2);
            
                                                //img_bubble[1]->setVisible(true);
                                                //img_bubble[2]->setVisible(true);
                                                panelAction->setVisible(true);
                                         }),
//                                         DelayTime::create(0.5f),
//                                         CallFunc::create([=]()->void{
//            
//                                            img_bubble[1]->setVisible(true);
//                                         }),
                                         NULL));
    }
    else if (cur_step == 10) { //show green card
        
        showGreenCacrd();
    }
    else if (cur_step == 12) { //show fold button
        
        for (int i = 0; i < 2; i++) {
            
            btn_action[i]->setEnabled(false);
            panelAction->setVisible(true);
        }
        showFocusParticle(3);
    }
    else if (cur_step == 14) {
        
        img_gcard->setVisible(false);
        img_bubble[1]->setVisible(false);
        showFocusParticle(-1);
    }
    else if (cur_step >= 15 ) {
        
        if (isNewRegister == true) {
            
            showBonusLayer();
        }
        else {
            
            Scene *pScene = TransitionFade::create(0.5, HelloWorld::createScene());
            Director::getInstance()->replaceScene(pScene);
        }
    }
    
//    switch (gameStatus) {
//            
//        case BEGIN_STATUS:
//            
//            resetGame();
//            break;
//            
//        case RISK_STATUS:
//            
//            risking();
//            break;
//            
//        case DEAL_STATUS:
//            
//            dealing();
//            break;
//            
//        case PLAY_STATUS:
//            
//            play();
//            break;
//            
//        default:
//            break;
//    }
}

void TutorialScene::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *pEvent)
{
    
}

void TutorialScene::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *pEvent)
{
    
}

void TutorialScene::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *pEvent)
{
    Touch *touch = touches[0];
    Point location = touch->getLocation();

    if (isTouchEnable == false) {
        
        return;
    }

//    if (cur_step == 8 || cur_step == 9 || cur_step > 15) {
//        
//        return;
//    }
    
    cur_step++;
    run();
}

void TutorialScene::update(float time)
{
    if(flagTextAnimation == true) {
        
        if(cntTextTimer == 100) {
            
            lb_point->setString(getNormalText2Number(gamePlayers[posTextAnimation].points));
            initTextAnimation();
        }
        else {
            
            curValue += 2*deltaValue;
            lb_point->setString(getNormalText2Number(curValue));
            cntTextTimer += 2;
        }
    }
}