//
//  RegisterLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 4/14/16.
//
//

#include "RegisterLayer.h"
#include "WebService.h"
#include "TutorialScene.h"

//define string
std::string g_strRegister[6][2] = {
    {"Choose one of them", "Elige el peor"},
    {"Please enter your name", "Por favor ingresa tu nombre"},
    {"PLAY NOW", "JUGAR AHORA"},
    {"SELECT LANGUAGE", "SELECCIONAR LENGUAJE"},
    {"ENGLISH", "INGLÉS"},
    {"SPANISH", "ESPAÑOL"}
    };

Layer* RegisterLayer::createLayer(Layer *parent, int zorder, LayerDelegate* pDelegate)
{
    pParentLayer = parent;
    pParentDelegate = pDelegate;
    
    // 'layer' is an autorelease object
    pCurLayer = Layer::create();
    pCurLayer->setVisible(false);
    
    // add layer as a chil  d to parent
    pParentLayer->addChild(pCurLayer, zorder);
    
    init();
    
    // return the scene
    return pCurLayer;
}

bool RegisterLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("RegisterLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    lb_title = (Text*)rootNode->getChildByName("title");
    lb_title->setString(g_strRegister[0][g_nLangType]);
  
    btn_main = static_cast<Button*>(rootNode->getChildByName("btn_0"));
    btn_main->addTouchEventListener(CC_CALLBACK_2(RegisterLayer::onMainBtnClicked, this, 0));
    
    btn_main->setTitleText(g_strRegister[2][g_nLangType]);
    
    //avatar image and checkbox
    for (int i = 0; i < 3; i++) {
        
        auto imgPhotoPanel = (Layout*)(rootNode->getChildByName(StringUtils::format("panel_%d", i)));
        imgPhotoPanel->addTouchEventListener(CC_CALLBACK_2(RegisterLayer::onCheckBtnClicked, this, i));
        imgPhotoPanel->setScaleX(g_fScaleWH);
        imgPhotoPanel->setLocalZOrder(10);
        
        btn_Check[i] = (Button*)(imgPhotoPanel->getChildByName("check"));
        //btn_Check[i]->addTouchEventListener(CC_CALLBACK_2(RegisterLayer::onCheckBtnClicked, this, i));
        
        auto image = (ImageView*)(imgPhotoPanel->getChildByName("photo"));
        image->setVisible(false);
        
        //clipping node
        auto clippingNode = ClippingNode::create();
        clippingNode->setAlphaThreshold(MASKING_THRESHOLD);
        clippingNode->setPosition(0, 0);
        clippingNode->setAnchorPoint(Vec2(0.0, 0.0));
        rootNode->addChild(clippingNode);
        clippingNode->setLocalZOrder(5);
        
        auto mask = Sprite::createWithSpriteFrameName("img_mask.png");
        float scaleX = (imgPhotoPanel->getBoundingBox().size.width) / (float)mask->getBoundingBox().size.width;
        float scaleY = (imgPhotoPanel->getBoundingBox().size.height) / (float)mask->getBoundingBox().size.height;
        mask->setScale(scaleX, scaleY);
        mask->setPosition(imgPhotoPanel->getPosition());
        clippingNode->setStencil(mask);
        
        auto img_photo = ImageView::create(StringUtils::format("avatar_%d.jpg", i), TextureResType::PLIST);
        img_photo->setScale(scaleX, scaleY);
        img_photo->setPosition(imgPhotoPanel->getPosition());
        clippingNode->addChild(img_photo);
        
        auto imgBorder = ImageView::create("border.png", TextureResType::PLIST);
        imgBorder->setScale(scaleX, scaleY);
        imgBorder->setPosition(imgPhotoPanel->getPosition());
        rootNode->addChild(imgBorder);
        imgBorder->setLocalZOrder(12);
    }
    
    //editbox
    auto panelName = static_cast<ImageView*>(rootNode->getChildByName("edit_bg"));
    Size szEdit = panelName->getBoundingBox().size;
    
    pEditBox = EditBox::create(Size(szEdit.width, szEdit.height), Scale9Sprite::create("transparent_bg.png"));
    //pEditBox->setAnchorPoint(Vec2(0, 0));
    pEditBox->setPosition(panelName->getPosition());
    pEditBox->setFontSize(48);
    pEditBox->setFontColor(Color3B::WHITE);
    pEditBox->setMaxLength(10);
    
    pEditBox->setReturnType(EditBox::KeyboardReturnType::DONE);
    pEditBox->setInputMode(EditBox::InputMode::SINGLE_LINE);
    pEditBox->setDelegate(this);
    pCurLayer->addChild(pEditBox);
    
    //init alert layer
    pAlertLayer = new AlertLayer();
    pAlertLayer->createLayer(pCurLayer, 100, this);
    
    selectIndex(0);
    
    g_mainUserInfo.user_name = "";
    
    
    //lang radio buttons
    lb_selLang = (Text*)rootNode->getChildByName("text_4");
    lb_selLang->setString(g_strRegister[3][g_nLangType]);
    for (int i = 0; i < 2; i++) {
        
        auto panel_lang = (Layout*)rootNode->getChildByName(StringUtils::format("panel_lang%d", i));
        btn_lang[i] = (Button*)panel_lang->getChildByName("btn_lang");
        btn_lang[i]->setScaleX(g_fScaleWH);
        btn_lang[i]->addTouchEventListener(CC_CALLBACK_2(RegisterLayer::onLangBtnClicked, this, i));
        panel_lang->addTouchEventListener(CC_CALLBACK_2(RegisterLayer::onLangBtnClicked, this, i));
        
        if (i == 0)
            lb_langEn = (Text*)panel_lang->getChildByName("text");
        else if (i == 1)
            lb_langSp = (Text*)panel_lang->getChildByName("text");
    }
    
    lb_langEn->setString(g_strRegister[4][g_nLangType]);
    lb_langSp->setString(g_strRegister[5][g_nLangType]);
    btn_lang[g_nLangType]->setBright(true);
    btn_lang[1-g_nLangType]->setBright(false);
    
    return true;
}
void RegisterLayer::onLangBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        g_nLangType = btn_id;
        btn_lang[g_nLangType]->setBright(true);
        btn_lang[1-g_nLangType]->setBright(false);
        updateString();
        //updateString2Lang();
        //g_pHelloWorld->updateString2Lang();
    }
}
void RegisterLayer::updateString()
{
    lb_title->setString(g_strRegister[0][g_nLangType]);
    btn_main->setTitleText(g_strRegister[2][g_nLangType]);
    lb_selLang->setString(g_strRegister[3][g_nLangType]);
    lb_langEn->setString(g_strRegister[4][g_nLangType]);
    lb_langSp->setString(g_strRegister[5][g_nLangType]);
    
    ((TutorialScene*)this->getParent())->updateString();
}
void RegisterLayer::selectIndex(int index)
{
    for(int i = 0; i < 3; i++) {
        
        if(i == index) {
            
            btn_Check[i]->setBright(true);
        }
        else {
            
            btn_Check[i]->setBright(false);
        }
    }
    
    select_index = index;
}

void RegisterLayer::onCheckBtnClicked(Ref *sender, Widget::TouchEventType type, int btn_index)
{
    if (type == Widget::TouchEventType::ENDED) {
    
        selectIndex(btn_index);
    }
}

void RegisterLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) { //close
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        
        if(g_mainUserInfo.user_name.empty() == true) {
            
            //cocos2d::MessageBox("You should input name", "");
            
            pAlertLayer->showLayer("", g_strRegister[1][g_nLangType]);
            return;
        }
        
        setAvatarUnlock(select_index, 1);
        setSelectAvatar(select_index);
//        srand(time(NULL));
//        //generate random id
//        int digits[5] = {0,};
//        for (int i = 0; i < 5; i++) {
//            
//            digits[i] = rand()%10;
//        }
//        g_mainUserInfo.user_id = StringUtils::format("%d%d%d%d%d", digits[0], digits[1], digits[2], digits[3], digits[4]);
        
        g_mainUserInfo.photo_id = StringUtils::format("%d", select_index);
        //g_mainUserInfo.points = 3000;
        WebService::getInstance()->updateAvatar();
        
        pParentDelegate->child_index = REGISTER_LAYER;
        pParentDelegate->onUpdatedDone();
        
        pCurLayer->setVisible(false);
    }
}

void RegisterLayer::showLayer()
{
    pParentDelegate->res_value = 0;
    pCurLayer->setVisible(true);
}

//////////////////////// editbox /////////////////////////////
////////  virtual functions ----------------
void RegisterLayer::editBoxEditingDidBegin(EditBox *editBox)
{
    //    Sequence * act2 = Sequence::create(
    //                                       DelayTime::create(0.1),
    //                                       MoveTo::create(0.3, Vec2(0, getScreenHeight()*0.3)),
    //                                       NULL);
    //    vipLayer->runAction(act2);
    
    //vipLayer->setPosition(0, getScreenHeight()*0.3);
}

void RegisterLayer::editBoxEditingDidEnd(EditBox *editBox)
{
    const char *strTxt = editBox->getText();
    if (strlen(strTxt) == 0) {
        
        pEditBox->setText(g_mainUserInfo.user_name.c_str());
    }
    else {
       
        g_mainUserInfo.user_name = pEditBox->getText();
    }
}

void RegisterLayer::editBoxTextChanged(EditBox *editBox, std::string &text) {
    
    bool flag = true;
}

void RegisterLayer::editBoxReturn(EditBox *editBox) {
    
    bool flag = true;
}






