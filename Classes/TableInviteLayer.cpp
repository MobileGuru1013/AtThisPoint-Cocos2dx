//
//  TableInviteLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 5/12/16.
//
//

#include "TableInviteLayer.h"
#include "WebService.h"

//define string
std::string g_strTableInvite[7][2] = {{"INVITE FRIENDS", "INVITAR AMIGOS"},
    {"GET 1,500 FOR EACH NEW FRIENDS!", " OBTEN 1,500 POR CADA AMIGO REFERIDO"},
    {"Select All", "Seleccionar Todos"},
    {"INVITE", "INVITAR"		},
    {"There is no friends for inviting", "No hay amigos para invitar"},
    {"Search Friends by Name", "Buscar amigos por nombre"},
    {"Request has been sent", "Solictud enviada"}
};

Layer* TableInviteLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool TableInviteLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("FBInviteLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    auto panelMain = (Layout*)rootNode->getChildByName("main");
    //panelMain->setScaleX(g_fScaleWH);
    
    lb_title[0] = (Text*)panelMain->getChildByName("title");
    
    //main buttons
    std::string strName = StringUtils::format("btn_%d", 0);
    auto btnMain = static_cast<Button*>(panelMain->getChildByName(strName));
    btnMain->setScaleX(g_fScaleWH);
    btnMain->addTouchEventListener(CC_CALLBACK_2(TableInviteLayer::onMainBtnClicked, this, 0));
    
    lb_status = (Text*)panelMain->getChildByName("status");
    lb_status->setString("");
    
    panel_list = (Layout*)panelMain->getChildByName("panel_0");
    auto lb_text = (Text*)panel_list->getChildByName("text");
    lb_text->setVisible(false);
    lb_title[1] = lb_text;
    
    panel_search = (Layout*)panel_list->getChildByName("panel_search");
    panelCheck = (Layout*)panel_list->getChildByName("panel_check");
    cb_all = (CheckBox*)panelCheck->getChildByName("cb_all");
    cb_all->setScaleX(g_fScaleWH);
    panelCheck->addTouchEventListener(CC_CALLBACK_2(TableInviteLayer::onInviteItemClicked, this, 0, cb_all));
    
    lb_title[2] = (Text*)panelCheck->getChildByName("text");
    
    sv_list = (ScrollView*)panel_list->getChildByName("sv_friend");
    sv_list->setClippingEnabled(true);
    sv_list->setClippingType(Layout::ClippingType::SCISSOR);
    
    btn_invite = (Button*)panel_list->getChildByName("btn_invite");
    btn_invite->setScaleX(g_fScaleWH);
    btn_invite->addTouchEventListener(CC_CALLBACK_2(TableInviteLayer::onMainBtnClicked, this, 1));
    
    lb_title[3] = (Text*)btn_invite->getChildByName("text");
    
    //editbox
    auto panelEdit = (Layout*)panel_search->getChildByName("panel");
    Size szEdit = panelEdit->getBoundingBox().size;
    
    pEditBox = EditBox::create(Size(szEdit.width, szEdit.height), Scale9Sprite::create("transparent_bg.png"));
    pEditBox->setAnchorPoint(Vec2(0, 0));
    //pEditBox->setPosition(panelEdit->getPosition());
    pEditBox->setPlaceHolder(g_strTableInvite[5][g_nLangType].c_str());
    pEditBox->setPlaceholderFont("arial.ttf", 48);
    //pEditBox->setPlaceholderFontSize(24);
    pEditBox->setPlaceholderFontColor(Color3B::WHITE);
    //pEditBox->setFontSize(24);
    pEditBox->setFont("arial.ttf", 48);
    pEditBox->setFontColor(Color3B::WHITE);
    pEditBox->setMaxLength(20);
    
    pEditBox->setReturnType(EditBox::KeyboardReturnType::DONE);
    pEditBox->setInputMode(EditBox::InputMode::SINGLE_LINE);
    pEditBox->setDelegate(this);
    setDefaultActionFlag(false);
    
    panelEdit->addChild(pEditBox);
    
    //init alert layer
    pAlertLayer = new AlertLayer();
    pAlertLayer->createLayer(pCurLayer, 10, this);
    
    return true;
}

void TableInviteLayer::updateString2Lang()
{
    for (int i = 0; i < 4; i++) {
        
        lb_title[i]->setString(g_strTableInvite[i][g_nLangType]);
    }
    pEditBox->setPlaceHolder(g_strTableInvite[5][g_nLangType].c_str());
}

void TableInviteLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        //g_mainUserInfo.points += 3000;
        //WebService::getInstance()->updatePoint();
        
        if(btn_id == 0) { //close
            
            
            //            g_mainUserInfo.points += 25000;
            //            WebService::getInstance()->updatePoint();
            //
            //            //          pDelegate->child_index = FBCONNECT_LAYER;
            //            pDelegate->onUpdatedDone();
            //
            //            g_pHelloWorld->showTableInviteLayer();
            
            hideLayer();
        }
        else if(btn_id == 1) { //invite
            
            std::string str_ids = "";
            int cnt = sv_list->getChildrenCount();
            for (int i = 0; i < cnt; i++) {
                
                Layout *panelItem = (Layout*)sv_list->getChildByTag(i);
                auto cb_check = (CheckBox*)panelItem->getChildByName("check");
                if (cb_check->isSelected() == false) continue;
                
                WebService::getInstance()->inviteFriend2Table(v_InvitableFrineds[i].user_id, g_roomId, position, v_InvitableFrineds[i].user_name);
            }
            
            pAlertLayer->showLayer("", g_strTableInvite[6][g_nLangType]);
        }
    }
}

void TableInviteLayer::onInviteItemClicked(Ref* pSender, Widget::TouchEventType type, int index, CheckBox *pItem)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        bool flag = pItem->isSelected();
        pItem->setSelected(!flag);
        
        int cnt = sv_list->getChildrenCount();
        if(index == 0) { //select all
            
            if(flag == false) {
                
                for (int i = 0; i < cnt; i++) {
                    
                    Layout *panelItem = (Layout*)sv_list->getChildByTag(i);
                    auto cb_check = (CheckBox*)panelItem->getChildByName("check");
                    cb_check->setSelected(true);
                }
                btn_invite->setVisible(true);
            }
            else {
                
                for (int i = 0; i < cnt; i++) {
                    
                    Layout *panelItem = (Layout*)sv_list->getChildByTag(i);
                    auto cb_check = (CheckBox*)panelItem->getChildByName("check");
                    cb_check->setSelected(false);
                }
                btn_invite->setVisible(false);
            }
        }
        else {
            
            bool flag = false;
            for (int i = 0; i < cnt; i++) {
                
                Layout *panelItem = (Layout*)sv_list->getChildByTag(i);
                auto cb_check = (CheckBox*)panelItem->getChildByName("check");
                if (cb_check->isSelected() == true) {
                    
                    flag = true;
                    break;
                }
            }
            
            if (flag == true) {
                
                btn_invite->setVisible(true);
            }
            else {
                
                btn_invite->setVisible(false);
            }
        }
    }
}

void TableInviteLayer::showInvitePanel()
{
    btn_invite->setVisible(false);
    ///////////////////////////    scroll view module begin    //////////////////////////////////
    sv_list->removeAllChildrenWithCleanup(true);
    
    int cntViewCol = 2; //view colum number
    int cntViewRow = 4; //view row number
    int cntTotalItems = v_InvitableFrineds.size(); //total item number
    
    if (cntTotalItems == 0) {
        
        panelCheck->setVisible(false);
        return;
    }
    else {
        
        panel_search->setVisible(true);
        panelCheck->setVisible(true);
        cb_all->setSelected(false);
    }
    
    int cntRow = cntTotalItems/cntViewCol;
    if ((cntTotalItems%cntViewCol) > 0) {
        
        cntRow++;
    }
    int offsetNum = 0;
    if (cntRow < cntViewRow) {
        
        offsetNum = cntViewRow - cntRow;
    }
    
    for (int i = 0; i < cntTotalItems; i++) {
        
        auto itemNode = CSLoader::createNode("InviteCellNode2.csb");
        auto panelMain = (Layout*)itemNode->getChildByName("panel");
        panelMain->setTag(i);
        
        //photo
        auto panelPhoto = (Layout*)panelMain->getChildByName("panel_0");
        panelPhoto->setScaleX(g_fScaleWH);
        
        //clipping node
        auto clippingNode = ClippingNode::create();
        clippingNode->setAlphaThreshold(MASKING_THRESHOLD);
        clippingNode->setPosition(0, 0);
        clippingNode->setAnchorPoint(Vec2(0.0, 0.0));
        panelMain->addChild(clippingNode);
        
        auto mask = Sprite::createWithSpriteFrameName("img_mask.png");
        float scaleX = (panelPhoto->getBoundingBox().size.width) / (float)mask->getBoundingBox().size.width;
        float scaleY = (panelPhoto->getBoundingBox().size.height) / (float)mask->getBoundingBox().size.height;
        mask->setScale(scaleX, scaleY);
        mask->setPosition(panelPhoto->getPosition());
        clippingNode->setStencil(mask);
        
        ImageView *img_photo = ImageView::create("img_mask.png", TextureResType::PLIST);
        img_photo->ignoreContentAdaptWithSize(false);
        img_photo->setScale(scaleX, scaleY);
        img_photo->setPosition(panelPhoto->getPosition());
        clippingNode->addChild(img_photo);
        
        auto imgBorder = ImageView::create("border.png", TextureResType::PLIST);
        imgBorder->setScale(scaleX, scaleY);
        imgBorder->setPosition(panelPhoto->getPosition());
        panelMain->addChild(imgBorder);
        
        WebService::getInstance()->setProfileImage(v_InvitableFrineds[i].photo_id, img_photo);
        
        //name
        auto lb_name = (Text*)panelMain->getChildByName("name");
        lb_name->setString(v_InvitableFrineds[i].user_name);
        
        //check box
        auto cb_item = (CheckBox*)panelMain->getChildByName("check");
        cb_item->setScaleX(g_fScaleWH);
        
        panelMain->addTouchEventListener(CC_CALLBACK_2(TableInviteLayer::onInviteItemClicked, this, i+1, cb_item));
        
        itemNode->removeChild(panelMain, true);
        
        addItem2ScrollView(panelMain, i, cntViewCol, cntRow+offsetNum, sv_list);
    }
    
    float wItemView = sv_list->getBoundingBox().size.width;
    float hItemView = (sv_list->getBoundingBox().size.height)*cntRow/(float)cntViewRow;
    sv_list->setInnerContainerSize(Size(wItemView, hItemView));
    sv_list->jumpToTop();
    
    sv_list->setVisible(true);
    /////////////////////////////   scroll view module end   ////////////////////////////////////
}

void TableInviteLayer::showLayer(int pos)
{
    position = pos;
    
    v_InvitableFrineds.clear();
    for (int i = 0; i < g_vFriends.size(); i++) {
        
        v_InvitableFrineds.push_back(g_vFriends[i]);
    }
    
    if (v_InvitableFrineds.size() > 0) {
        
        lb_status->setString("");
        showInvitePanel();
    }
    else {
        
        panel_list->setVisible(false);
        lb_status->setString(g_strTableInvite[4][g_nLangType]);
    }
    
    updateString2Lang();
    pCurLayer->setVisible(true);
}

void TableInviteLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}

//////////////////////// editbox /////////////////////////////
////////  virtual functions ----------------
void TableInviteLayer::editBoxEditingDidBegin(EditBox *editBox)
{
    //    Sequence * act2 = Sequence::create(
    //                                       DelayTime::create(0.1),
    //                                       MoveTo::create(0.3, Vec2(0, getScreenHeight()*0.3)),
    //                                       NULL);
    //    vipLayer->runAction(act2);
    
    //vipLayer->setPosition(0, getScreenHeight()*0.3);
}

void TableInviteLayer::editBoxEditingDidEnd(EditBox *editBox)
{
    const char *strTxt = editBox->getText();
    
    v_InvitableFrineds.clear();
    if (strlen(strTxt) == 0) {
        
        //total facebook friends
        for (int i = 0; i < g_vFriends.size(); i++) {
            
            v_InvitableFrineds.push_back(g_vFriends[i]);
        }
    }
    else {
        
        //find only firends with name
        for (int i = 0; i < g_vFriends.size(); i++) {
            
            if(g_vFriends[i].user_name.find(strTxt) == std::string::npos) continue;
            
            v_InvitableFrineds.push_back(g_vFriends[i]);
        }
    }
    
    showInvitePanel();
}

void TableInviteLayer::editBoxTextChanged(EditBox *editBox, std::string &text) {
    
    bool flag = true;
}

void TableInviteLayer::editBoxReturn(EditBox *editBox) {
    
    bool flag = true;
}

