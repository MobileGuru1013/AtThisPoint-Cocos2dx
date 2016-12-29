//
//  LevelupLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 4/28/16.
//
//

#include "LevelupLayer.h"
#include "WebService.h"
#include "AppService.h"

//define string
std::string g_strLevelup[3][2] = {{"CONGRATULATIONS!", "¡FELICIDADES!"},
    {"YOU HAVE REACHED LEVEL", "HAZ ALCANZADO EL NIVEL"},
    {"YOU GOT ONE FREE SPIN", "HAS OBTENIDO UN GIRO GRATIS"}
};

Layer* LevelupLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool LevelupLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("LevelupLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    panelMain = (Layout*)rootNode->getChildByName("panel");
    //imgPhotoPanel->addTouchEventListener(CC_CALLBACK_2(RegisterLayer::onCheckBtnClicked, this, i));
    panelMain->setScaleX(g_fScaleWH);
    
    posOrigin = panelMain->getPosition();
    szPanel = panelMain->getBoundingBox().size;
    
//    //main buttons
//    for (int i = 0; i < 5; i++) {
//        
//        std::string strName = StringUtils::format("btn_%d", i);
//        auto btnMain = static_cast<Button*>(rootNode->getChildByName(strName));
//        btnMain->addTouchEventListener(CC_CALLBACK_2(LevelupLayer::onMainBtnClicked, this, i));
//        btnMain->setScaleX(g_fScaleWH);
//    }
    
    img_spin = (ImageView*)panelMain->getChildByName("spin_mark");
    img_spin->setTouchEnabled(true);
    img_spin->addTouchEventListener(CC_CALLBACK_2(LevelupLayer::onMainBtnClicked, this, 1));
    
    lb_spin = (Text*)panelMain->getChildByName("text_2");
    lb_level = (Text*)panelMain->getChildByName("level");
    
    for (int i = 0; i < 3; i++) {
        
        lb_title[i] = (Text*)rootNode->getChildByName(StringUtils::format("text_%d", i));
    }
    
    return true;
}

void LevelupLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if(btn_id == 0) { //close
            
            hideLayer();
        }
        else if(btn_id == 1){ //spin mark
            
            pDelegate->onGetFreesSpinDone();
            hideLayer();
        }
    }
}

void LevelupLayer::showLayer(int level)
{
    lb_level->setString(StringUtils::format("%d", level+1));
    lb_title[0]->setString(g_strLevelup[0][g_nLangType]);
    lb_title[2]->setString(g_strLevelup[2][g_nLangType]);
    lb_title[0]->setString(StringUtils::format("%s %d", g_strLevelup[2][g_nLangType].c_str(), level+1));
    
    if((level+1)%3 == 0) {
        
        g_nFreeSpin++;
        time(&g_timeLastFreeSpin);
        WebService::getInstance()->updateFreeSpin();
        
        img_spin->setVisible(true);
        lb_spin->setVisible(true);
        
        AppService::startFreeSpinNotification();
    }
    else {
        
        img_spin->setVisible(false);
        lb_spin->setVisible(false);
    }
    
    panelMain->setPosition(Vec2(posOrigin.x, posOrigin.y + szPanel.height));
    
    Point tar_pos = posOrigin;
    panelMain->runAction(MoveTo::create(0.3f, tar_pos));

    pCurLayer->setVisible(true);
}

void LevelupLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}
