//
//  SpinLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 4/29/16.
//
//

#include "SpinLayer.h"
#include "WebService.h"

#include "MyIAPListner.h"

//define string
std::string g_strSpin[8][2] = {{"FORTUNE WHEEL", "RUEDA DE LA FORTUNA"},
    {"YOU HAVE 1 FREE SPIN", "TIENES UNA GIRADA GRATIS"},
    {"WIN UP TO", "GANA HASTA"},
    {"450K POINTS", "450 MIL PUNTOS"},
    {"ONLY  $1.99", "POR SÓLO $1.99"},
    {"YOU WON", "GANASTE"},
    {"SPIN", "GIRA"},
    {"RESPIN", "GIRA DE NUEVO"}
};

Layer* SpinLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool SpinLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("SpinLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    lb_title[0] = (Text*)rootNode->getChildByName("title");
    
    //monkey
    auto img_mark = (ImageView*)rootNode->getChildByName("monkey");
    img_mark->setScaleX(g_fScaleWH);
    
    //bottom mask
    img_bottomMask = (ImageView*)rootNode->getChildByName("bottom_mask");
    
    //chip bg
    img_chipbg = (ImageView*)rootNode->getChildByName("chip_bg");
    
    //effect panel
    node_effect = (Node*)rootNode->getChildByName("effectNode");
    //node_effect->setScaleX(g_fScaleWH);
    auto panel_effect = (Layout*)node_effect->getChildByName("panel_effect");
    lb_winpoint = (Text*)panel_effect->getChildByName("point");
    lb_winpoint->setVisible(true);
    auto lb_text = (Text*)panel_effect->getChildByName("text_0");
    lb_text->setVisible(true);
    lb_title[5] = lb_text;
//    for (int i = 0; i < 2; i++) {
//        
//        particle_win[i] = (ParticleSystem*)panelEffect->getChildByName(StringUtils::format("particle_%d", i+1));
//    }
    
    //spin panel
    auto panelSpin = (Layout*)rootNode->getChildByName("panel_frame");
    //panelSpin->setScaleX(g_fScaleWH);
    panelFrame = panelSpin;
    
    //mask slice
    for (int i = 0; i < 12; i++) {
        
        panel_mask[i] = (Layout*)panelSpin->getChildByName(StringUtils::format("mask_%d", i));
        sznMask[i] = panel_mask[i]->getContentSize();
    }
    
    //sliding panel
    auto panel_sliding = (Layout*)panelSpin->getChildByName("panel_sliding");
    particle_sliding = (ParticleSystem*)panel_sliding->getChildByName("particle");
    particle_sliding->stopSystem();
    
    //onlight panel
    panel_onlight = (Layout*)panelSpin->getChildByName("panel_onlight");
    
    posSpinOrigin = panelSpin->getPosition();
    
    img_spin = (ImageView*)panelSpin->getChildByName("frame");
    auto img_lights = (ImageView*)panelSpin->getChildByName("light");
    img_light = (ImageView*)img_lights->getChildByName("lights");
    for (int i = 0; i < 16; i++) {
        
        particle_lights[i] = (ParticleSystem*)img_lights->getChildByName(StringUtils::format("particle_%d", i));
        particle_lights[i]->stopSystem();
    }
    
    for (int i = 0; i < 2; i++) {
        
        lb_spintitle[i] = (Text*)panelSpin->getChildByName(StringUtils::format("title_%d", i));
    }
    
    //main buttons
    for (int i = 0; i < 2; i++) {
        
        std::string strName = StringUtils::format("btn_%d", i);
        auto btnMain = static_cast<Button*>(rootNode->getChildByName(strName));
        btnMain->addTouchEventListener(CC_CALLBACK_2(SpinLayer::onMainBtnClicked, this, i));
        btnMain->setScaleX(g_fScaleWH);
        
        panelText[i] = (Layout*)rootNode->getChildByName(StringUtils::format("panel_%d", i+1));
        
        if (i == 1) {
            
            btn_spin = btnMain;
            lb_title[6] = (Text*)btn_spin->getChildByName("text");
            lb_title[1] = (Text*)panelText[i]->getChildByName("text");
        }
        else {
            
            for (int k = 0; k < 3; k++) {
                
                lb_title[k+2] = (Text*)panelText[i]->getChildByName(StringUtils::format("text_%d", k));
            }
        }
    }
    
    //animation
    pAnimation = CSLoader::createTimeline("SpinLayer.csb");
    pAnimation->setFrameEventCallFunc(CC_CALLBACK_1(SpinLayer::onAnimationEvent, this));
    pAnimation->retain();
    pAnimation->gotoFrameAndPause(0);
    pCurLayer->stopAllActions();
    
    pNodeAnimation = CSLoader::createTimeline("SpinWinNode.csb");
    pNodeAnimation->retain();
    pNodeAnimation->gotoFrameAndPause(0);

    updateString2Lang();
    //panelFrame->setScaleX(g_fScaleWH);
    
    return true;
}

void SpinLayer::updateString2Lang()
{
    for (int i = 0; i < 7; i++) {
        
        lb_title[i]->setString(g_strSpin[i][g_nLangType]);
    }
}

int g_nSpinPoint[12] = {450000, 15000, 15000, 40000, 15000, 15000, 15000, 150000, 15000, 450000, 100000, 15000};


void SpinLayer::startMaskAnimation()
{
    for (int i = 0; i < 12; i++) {
        
        panel_mask[i]->runAction(Sequence::create( DelayTime::create(i*0.15),
                                                     //MoveTo::create(0.6f, tar_pos),
                                                     CallFunc::create(CC_CALLBACK_0(SpinLayer::actionMaskAnimationDone, this, i)),
                                                     NULL));
    }
}

void SpinLayer::actionMaskAnimationDone(int index)
{
    Size szPanel = panel_mask[index]->getContentSize();
    szPanel.height -= 100;
    if (szPanel.height < 0) {
        
        szPanel.height = 0;
    }
    panel_mask[index]->setContentSize(szPanel);
    
    if (szPanel.height > 0) {
        
        panel_mask[index]->runAction(Sequence::create( DelayTime::create(0.1),
                                                      //MoveTo::create(0.6f, tar_pos),
                                                      CallFunc::create(CC_CALLBACK_0(SpinLayer::actionMaskAnimationDone, this, index)),
                                                      NULL));
    }
    else {
        
        cntMask++;
        if (cntMask == 12) {
            
            particle_sliding->resetSystem();
            pCurLayer->runAction(Sequence::create( DelayTime::create(0.3),
                                                  CallFunc::create([=]()->void{
                    
                                                    img_bottomMask->setVisible(true);
                                        
                                                    panelText[spin_type]->setVisible(true);
                                                    panelText[1-spin_type]->setVisible(false);
                                                    btn_spin->setVisible(true);
                                                  }),
                                              NULL));
        }
    }
}

void SpinLayer::onAnimationEvent(Frame *frame)
{
    //EventFrame* eFrame = dynamic_cast<EventFrame *>(frame);
    //auto eName = eFrame->getEvent();
    
    int index = frame->getFrameIndex();
    
    CCLOG("frame event: %d", index);
    
    if (index == 45) { //spin is shown
        
        startMaskAnimation();
        //if (cur_status == LAYER_START)
        {
//            img_bottomMask->setVisible(true);
//            
//            panelText[spin_type]->setVisible(true);
//            panelText[1-spin_type]->setVisible(false);
//            btn_spin->setVisible(true);
        }
    }
    else if(index == 189) { //spin rotation is stop
        
        select_index = rand()%12;
        
        float angle = 30*select_index + 360;
        angle += img_spin->getRotation();
        
        float delay = angle*0.0023;
        
        float beta = 30*select_index + 360;
        beta += panel_onlight->getRotation();
        
        panel_onlight->stopAllActions();
        panel_onlight->runAction(Sequence::create( EaseCircleActionOut::create(RotateTo::create(delay, angle)),
                                             //CallFunc::create(CC_CALLBACK_0(SpinLayer::actionSpinStopDone, this)),
                                             NULL));

        img_spin->runAction(Sequence::create( EaseCircleActionOut::create(RotateTo::create(delay, angle)),
                                              CallFunc::create(CC_CALLBACK_0(SpinLayer::actionSpinStopDone, this)),
                                               NULL));
        
    }
    else if (index == 193) { //lose end
        
        //cur_status = SPIN_START;
    }
}

void SpinLayer::actionSpinStopDone()
{
    int index = select_index;
    if(g_nSpinPoint[index] > 0) { //if win
        
        panel_onlight->setRotation(0);
        panel_onlight->setVisible(false);
        img_light->setVisible(true);
        
        //turn on lights
        for (int i = 0; i < 16; i++) {
            
            particle_lights[i]->resetSystem();
        }
        
        Point tar_pos = img_chipbg->getPosition();
        tar_pos.y = -700;
        lb_winpoint->setString(StringUtils::format("%s", getNormalText2Number(g_nSpinPoint[index]).c_str()));
        
        g_mainUserInfo.points += g_nSpinPoint[index];
        
        pDelegate->child_index = SPIN_LAYER;
        pDelegate->onUpdatedDone();
        
        WebService::getInstance()->updatePoint();
        
        //showWin();
        img_chipbg->runAction(Sequence::create( DelayTime::create(2),
                                               MoveTo::create(0.6f, tar_pos),
                                               CallFunc::create(CC_CALLBACK_0(SpinLayer::showWin, this)),
                                               NULL));
    }
    else { //spin again
        
        btn_spin->setTitleText("RESPIN");
        btn_spin->setVisible(true);
        //showLose();
    }

    CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
}

void SpinLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        if(btn_id == 0) { //close
            
            hideLayer();
        }
        else if(btn_id == 1){ //spin
            
            if(spin_type == 0) {
                
                sdkbox::IAP::purchase("buy_7");
            }
            else
            {
                
                actionRotateSpin();
            }
        }
    }
}

void SpinLayer::actionRotateSpin()
{
    img_light->setVisible(false);
    if (node_effect->isVisible() == true) {
        
        node_effect->stopAllActions();
        node_effect->setVisible(false);
        for (int i = 0; i < 16; i++) {
            
            particle_lights[i]->stopSystem();
        }
        
        panelFrame->setPosition(posSpinOrigin);
        panelFrame->runAction(Sequence::create(  MoveTo::create(0.6f, posSpinCenter),
                                               CallFunc::create(CC_CALLBACK_0(SpinLayer::rotateSpin, this)),
                                               NULL));
    }
    else {
        
        particle_sliding->stopSystem();
        rotateSpin();
    }
}

void SpinLayer::showWin()
{
    posSpinCenter = panelFrame->getPosition();
    
    Point tar_pos = posSpinOrigin;
    tar_pos.y = 1536 - tar_pos.y;
    panelFrame->runAction(MoveTo::create(0.6, tar_pos));
    
    node_effect->stopAllActions();
    node_effect->setVisible(true);
    node_effect->runAction(pNodeAnimation);
    pNodeAnimation->gotoFrameAndPlay(0, true);
    
    //g_nFreeSpin--;
    g_nFreeSpin = 0;
    //if (g_nFreeSpin < 0) {
        
    //    g_nFreeSpin = 0;
    //}
    
    WebService::getInstance()->updateFreeSpin();
    
    spin_type = 0;
    //if (g_nFreeSpin > 0) {
        
    //    spin_type = 1;
    //}
    
    lb_spintitle[0]->setVisible(true);
    lb_spintitle[1]->setVisible(false);
    
    panelText[spin_type]->setVisible(true);
    panelText[1-spin_type]->setVisible(false);
    //btn_spin->setTitleText("RESPIN");
    
    lb_title[6]->setString(g_strSpin[7][g_nLangType]);
    btn_spin->setVisible(true);
}

void SpinLayer::showLose()
{
    pCurLayer->stopAllActions();
    pCurLayer->runAction(pAnimation);
    pAnimation->setTimeSpeed(1);
    pAnimation->play("lose", false);
}

void SpinLayer::showSpin()
{
    for (int i = 0; i < 16; i++) {
        
        particle_lights[i]->stopSystem();
    }
    node_effect->setVisible(false);
    img_bottomMask->setVisible(false);
    
    pCurLayer->stopAllActions();
    pCurLayer->runAction(pAnimation);
    pAnimation->setTimeSpeed(0.3);
    pAnimation->play("start", false);
    
    panelFrame->setScaleX(g_fScaleWH);
}

void SpinLayer::rotateSpin()
{
    btn_spin->setVisible(false);
    panelText[0]->setVisible(false);
    panelText[1]->setVisible(false);
    
    pCurLayer->stopAllActions();
    pCurLayer->runAction(pAnimation);
    pAnimation->setTimeSpeed(1);
    pAnimation->play("rotate", false);
    
    panel_onlight->setVisible(true);
    panel_onlight->runAction(RotateTo::create(10.0f, 360*20));
    
    if (g_bSound == true) {
        
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("spin.aac", false);
    }
}

void SpinLayer::showLayer()
{
    flagActive = true;
    
    cntMask = 0;
    if (g_nFreeSpin > 0) {
        
        spin_type = 1;
    } else {
    
        spin_type = 0;
    }
    
    if (spin_type == 0) { //buy spin
        
        lb_spintitle[0]->setVisible(true);
        lb_spintitle[1]->setVisible(false);
    }
    else { //free spin
        
        lb_spintitle[0]->setVisible(false);
        lb_spintitle[1]->setVisible(true);
    }
    
    panel_onlight->setVisible(false);
    panelText[0]->setVisible(false);
    panelText[0]->setVisible(false);
    
    img_light->setVisible(false);
    img_bottomMask->setVisible(false);
    
    //btn_spin->setTitleText("SPIN");
    
    for (int i = 0; i < 12; i++) {
        
        panel_mask[i]->setContentSize(sznMask[i]);
        panel_mask[i]->setVisible(true);
    }
    for (int i = 0; i < 16; i++) {
        
        particle_lights[i]->stopSystem();
    }
    particle_sliding->stopSystem();
    node_effect->setVisible(false);
    
    cur_status = LAYER_START;
    
    pAnimation->gotoFrameAndPause(0);
    pCurLayer->stopAllActions();
    
    DelayTime *delayTime = DelayTime::create(0.3);
    CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(SpinLayer::showSpin, this));
    Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
    pCurLayer->runAction(delayAct);
    
    updateString2Lang();
    pCurLayer->setVisible(true);
}

void SpinLayer::hideLayer()
{
    flagActive = false;
    pAnimation->gotoFrameAndPause(0);
    pCurLayer->stopAllActions();
    pCurLayer->setVisible(false);
}
