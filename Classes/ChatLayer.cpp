//
//  ChatLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 4/27/16.
//
//

#include "ChatLayer.h"

static std::mutex       g_msgMutex;
static std::queue<std::string> g_msgQueue;

//define string
std::string g_strChat[13][2] = {{"You:", "Tu:"},
    {"Enter your text here", "Introduce tu texto aquí…"},
    {"Nice Game!", "Buen Juego!"},
    {"Good Luck!", "Buena Suerte!"},
    {"Well Played!", "Bien Jugado!"},
    {"Hi All!", "Hola A Todos!"},
    {"Thanks!", "Gracias!"},
    {"Hurry up!", "Date Prisa!"},
    {"Nice Hand!", "Buena Mano!"},
    {"Hahaha", "Jajaja"},
    {"Love it!", "Lo amé!"}
};

Layer* ChatLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool ChatLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("ChatLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    panelMain = static_cast<Layout*>(rootNode->getChildByName("panel"));
    //    mainPanel->setScaleX(g_fScaleWH);
    posOrigin = panelMain->getPosition();
    
    auto img_bg = (ImageView*)panelMain->getChildByName("bg");
    lv_chat = (ListView*)img_bg->getChildByName("lv_chat");
    lv_chat->setItemsMargin(15);
    lv_chat->setDefaultAlign(false);
    lv_words = (ListView*)img_bg->getChildByName("lv_words");
    
    //editbox
    auto panelEdit = (Layout*)img_bg->getChildByName("input_panel");
    Size szEdit = panelEdit->getBoundingBox().size;
    
    pEditBox = EditBox::create(Size(szEdit.width, szEdit.height), Scale9Sprite::create("transparent_bg.png"));
    pEditBox->setAnchorPoint(Vec2(0, 0));
    //pEditBox->setPosition(panelEdit->getPosition());
    pEditBox->setPlaceHolder(g_strChat[1][g_nLangType].c_str());
    pEditBox->setPlaceholderFont("arial.ttf", 48);
    //pEditBox->setPlaceholderFontSize(24);
    pEditBox->setPlaceholderFontColor(Color3B(179, 107, 201));
    //pEditBox->setFontSize(24);
    pEditBox->setFont("arial.ttf", 48);
    pEditBox->setFontColor(Color3B::WHITE);
    pEditBox->setMaxLength(120);
    
    pEditBox->setReturnType(EditBox::KeyboardReturnType::DONE);
    pEditBox->setInputMode(EditBox::InputMode::SINGLE_LINE);
    pEditBox->setDelegate(this);
    setDefaultActionFlag(false);
    
    panelEdit->addChild(pEditBox);
    
    addDefaultWords();
    
    //clear data
    g_msgQueue = std::queue<std::string>();

    //this->scheduleUpdate();

    return true;
}

void ChatLayer::addDefaultWords()
{
    //adding default node
    for (int i = 0; i < MAX_DEFAULTCHATWORDS; i++) {
      
        auto cellNode = CSLoader::createNode("ChatWordCellNode.csb");
        auto panel = static_cast<Layout*>(cellNode->getChildByName("panel"));
        auto lb_word = static_cast<Text*>(panel->getChildByName("text"));
        lb_word->setString(g_strChat[i+2][g_nLangType]);
        
        panel->addTouchEventListener(CC_CALLBACK_2(ChatLayer::onChatWordClicked, this, i));
        
        cellNode->removeChild(panel, true);
        lv_words->pushBackCustomItem(panel);
        //lv_Friends->addChild(panel);
    }
    
    lv_words->jumpToTop();
}

void ChatLayer::onChatWordClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        g_msgMutex.lock();
        
        g_msgQueue.push(g_strChat[btn_id+2][g_nLangType]);
        
        g_msgMutex.unlock();
        //addNewMessage2MainUser(g_strDefaultChatWords[btn_id]);
        //pDelegate->onChatMessageDone(g_strDefaultChatWords[btn_id]);
        
        hideLayer();
    }
}

void ChatLayer::showLayer()
{
    pEditBox->setText("");
    lv_chat->jumpToBottom();
    lv_chat->refreshView();
    pCurLayer->setVisible(true);
}

void ChatLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}

void ChatLayer::addNewMessage(std::string name, std::string text)
{
    int num = lv_chat->getItems().size();
    for (int i = 0;  i < num-MAX_CHATCELL; i++) {
        
        lv_chat->removeItem(0);
    }
    
    auto _richText = RichText::create();
    //_richText->ignoreContentAdaptWithSize(false);
    //Size szScrollView = lv_chat->getBoundingBox().size;
    //_richText->setSize(szScrollView);

    RichElementText* re1 = RichElementText::create(1, Color3B(251, 205, 247), 255, name+":", "arial", 42);
    RichElementText* re2 = RichElementText::create(2, Color3B::WHITE, 255, text, "arial", 42);

    _richText->pushBackElement(re1);
    _richText->pushBackElement(re2);
    _richText->formatText();    

    LinearLayoutParameter* linearLayoutParameter = (LinearLayoutParameter*)(_richText->getLayoutParameter());
    if (!linearLayoutParameter)
    {
        linearLayoutParameter = LinearLayoutParameter::create();
    }
    linearLayoutParameter->setGravity(LinearLayoutParameter::LinearGravity::LEFT);
    _richText->setLayoutParameter(linearLayoutParameter);
    
    lv_chat->pushBackCustomItem(_richText);
    lv_chat->jumpToBottom();
    
    lv_chat->refreshView();
}

void ChatLayer::addNewMessage2MainUser(std::string text)
{
    int num = lv_chat->getItems().size();
    for (int i = 0;  i < num-MAX_CHATCELL-1; i++) {
        
        lv_chat->removeItem(0);
    }
    //name
    auto _richName = RichText::create();
    RichElementText* re1 = RichElementText::create(1, Color3B(251, 205, 247), 255, g_strChat[0][g_nLangType], "arial", 42);
    
    _richName->pushBackElement(re1);
    _richName->formatText();
    
    LinearLayoutParameter* linearLayoutParameter = (LinearLayoutParameter*)(_richName->getLayoutParameter());
    if (!linearLayoutParameter)
    {
        linearLayoutParameter = LinearLayoutParameter::create();
    }
    
    linearLayoutParameter->setGravity(LinearLayoutParameter::LinearGravity::RIGHT);
    _richName->setLayoutParameter(linearLayoutParameter);
    
    lv_chat->pushBackCustomItem(_richName);

    //chat text
    auto _richText = RichText::create();
    //_richText->ignoreContentAdaptWithSize(false);
    //Size szScrollView = lv_chat->getBoundingBox().size;
    //_richText->setSize(szScrollView);
    
    RichElementText* re2 = RichElementText::create(2, Color3B::WHITE, 255, text, "arial", 42);
    
    _richText->pushBackElement(re2);
    _richText->formatText();
    
    linearLayoutParameter = (LinearLayoutParameter*)(_richText->getLayoutParameter());
    if (!linearLayoutParameter)
    {
        linearLayoutParameter = LinearLayoutParameter::create();
    }

    linearLayoutParameter->setGravity(LinearLayoutParameter::LinearGravity::RIGHT);
    _richText->setLayoutParameter(linearLayoutParameter);
   
    lv_chat->pushBackCustomItem(_richText);
    lv_chat->jumpToBottom();
    
    lv_chat->refreshView();
}

void ChatLayer::updateHistoryBox(std::string msg)
{
    
}

//////////////////////// editbox /////////////////////////////
////////  virtual functions ----------------
void ChatLayer::editBoxEditingDidBegin(EditBox *editBox)
{
    //    Sequence * act2 = Sequence::create(
    //                                       DelayTime::create(0.1),
    //                                       MoveTo::create(0.3, Vec2(0, getScreenHeight()*0.3)),
    //                                       NULL);
    //    vipLayer->runAction(act2);
    
    //vipLayer->setPosition(0, getScreenHeight()*0.3);
}

void ChatLayer::editBoxEditingDidEnd(EditBox *editBox)
{
    const char *strTxt = editBox->getText();
    if (strlen(strTxt) == 0) {
        
        return;
    }
    
    std::vector<std::string> vMessages = getNormalMessages(strTxt);
    
    if(g_msgQueue.empty() == true) {
        
        durationTimer = 5.0;
    }
    
    g_msgMutex.lock();
    for (int i = 0; i < vMessages.size(); i++) {
        
        g_msgQueue.push(vMessages[i]);
        //addNewMessage2MainUser(vMessages[i]);
    }
    g_msgMutex.unlock();
    
    hideLayer();
    //pDelegate->onChatMessageDone(strTxt);
}

void ChatLayer::editBoxTextChanged(EditBox *editBox, std::string &text) {
    
    bool flag = true;
}

void ChatLayer::editBoxReturn(EditBox *editBox) {
    
    bool flag = true;
}

void ChatLayer::editboxKeyboardWillShow(IMEKeyboardNotificationInfo& info)
{
//    Rect rectTracked = getRect(panelMain);
//    // some adjustment for margin between the keyboard and the edit box.
//    rectTracked.origin.y -= 4;
    
    // if the keyboard area doesn't intersect with the tracking node area, nothing needs to be done.
//    if (!rectTracked.intersectsRect(info.end))
//    {
//        CCLOG("needn't to adjust view layout.");
//        return;
//    }
    
    // assume keyboard at the bottom of screen, calculate the vertical adjustment.
    Point tar_pos = posOrigin;
    float offsetY = info.end.getMaxY() - posOrigin.y;
    if(offsetY >= 0) {
        
        tar_pos.y += (offsetY+4);
    }
    
    panelMain->runAction(MoveTo::create(info.duration, tar_pos));
}

void ChatLayer::editboxKeyboardWillHide(IMEKeyboardNotificationInfo& info)
{
    panelMain->runAction(MoveTo::create(info.duration, posOrigin));
}

void ChatLayer::update(float delta)
{
    durationTimer += delta;
    if(durationTimer > 2.2) {
        
        if(g_msgQueue.empty() == true) return;
        
        durationTimer = 0;
        g_msgMutex.lock();
        
        std::string res_msg;
        std::string msg = g_msgQueue.front();
        //addNewMessage2MainUser(msg);
        g_msgQueue.pop();
        
        res_msg = msg;
        
        if (g_msgQueue.empty() == false) {
            
            msg = g_msgQueue.front();
            //addNewMessage2MainUser(msg);
            g_msgQueue.pop();
            
            res_msg += ' ';
            res_msg += msg;
        }
        g_msgMutex.unlock();
        
        addNewMessage2MainUser(res_msg);
        pDelegate->onChatMessageDone(res_msg);
    }
}


