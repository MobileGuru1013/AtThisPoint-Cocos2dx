//
//  MsgLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 6/7/16.
//
//

#include "MsgLayer.h"
#include "extensions/cocos-ext.h"
USING_NS_CC_EXT;

Layer* MsgLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool MsgLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("MsgLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    auto mainPanel = static_cast<Layout*>(rootNode->getChildByName("main"));
    mainPanel->setScaleX(g_fScaleWH);
    
    auto btnClose = static_cast<Button*>(mainPanel->getChildByName(StringUtils::format("btn_%d", 0)));
    btnClose->addTouchEventListener(CC_CALLBACK_2(MsgLayer::onMainBtnClicked, this, 0));
    
    lb_title = static_cast<Text*>(mainPanel->getChildByName("title"));
    sv_msg = (cocos2d::ui::ScrollView*)mainPanel->getChildByName("sv_msg");
    lb_msg = static_cast<Text*>(sv_msg->getChildByName("text"));
    
    return true;
}

void MsgLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (btn_id == 0) { //close button
            
            if (g_bSound == true) {
                
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
            }
         
            hideLayer();
        }
    }
}
/*
 void MsgLayer::showLayer(std::string title, std::string msg)
{
    //parsing title
//    std::string strTmp = "";
//    int len = title.length();
//    while (len > 20) {
//        
//        strTmp += title.substr(0, 20);
//        strTmp += "\n";
//        title = title.substr(21);
//        
//        len = title.length();
//    }
//    strTmp += title;
//    lb_title->setString(strTmp);
//    
//    strTmp = "";
//    len = msg.length();
//    while (len > 25) {
//        
//        strTmp += msg.substr(0, 25);
//        strTmp += "\n";
//        msg = msg.substr(26);
//        
//        len = msg.length();
//    }
//    strTmp += msg;
//    lb_msg->setString("");
//    lb_msg->setString(strTmp);
    
    int sv_width = sv_msg->getBoundingBox().size.width;
    auto lb_tmp = Label::createWithSystemFont(msg, "arial.ttf", lb_msg->getFontSize(), Size(sv_width, 0));
    
    Size szScrollView = sv_msg->getInnerContainerSize();
    lb_title->setString(title);
    lb_title->setColor(Color3B(255, 255, 255));
    lb_msg->ignoreContentAdaptWithSize(false);
    lb_msg->setTextAreaSize(lb_tmp->getContentSize());
    lb_msg->setString(msg);
    lb_msg->setTextHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    lb_msg->setTextVerticalAlignment(cocos2d::TextVAlignment::TOP);
    TextField* fied;
    
    int sv_height = sv_msg->getBoundingBox().size.height;
    int lb_height = lb_msg->getContentSize().height;
    if (lb_height < sv_height) {
        
        lb_height = sv_height;
    }
    lb_msg->setPosition(Vec2( 10,  lb_height ));
    sv_msg->setInnerContainerSize(Size(szScrollView.width, lb_height));
    sv_msg->jumpToTop();
    
    pCurLayer->setVisible(true);
}*/
void MsgLayer::showLayer(std::string title, std::string msg)
{
    lb_title->setString(title);
    Size winSize = sv_msg->getBoundingBox().size;
    auto *scroll = cocos2d::extension::ScrollView::create(winSize);
    // 縦方向だけにスクロール
    scroll->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL);
    sv_msg->getParent()->addChild(scroll);
    lb_msg->setString("");
    auto label = LabelTTF::create(msg, "arial.ttf", 60.0f);
    label->setColor(Color3B::WHITE);
    scroll->setPosition(sv_msg->getPosition());
    
    // 文字の開始位置を画面の上に合わせる
    // 文字データは、一番左上から表示させたいので、widthは0
    // heightはコンテンツサイズから画面縦を引いた負数にする
    label->setDimensions(Size(winSize.width,0));
    label->setDimensions(Size(label->getContentSize().width, label->getContentSize().height));
    // 左寄せにする
    label->setHorizontalAlignment(TextHAlignment::LEFT);
    
    // スクロールされるラベルの調整
    scroll->setContainer(label);
    scroll->setContentOffset(Point(0, 0 - (label->getContentSize().height - winSize.height)));
    
    sv_msg->setVisible(false);
    lb_msg->setVisible(false);
     pCurLayer->setVisible(true);
}
void MsgLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}


