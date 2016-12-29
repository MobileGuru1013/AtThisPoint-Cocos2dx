//
//  SocialLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 4/30/16.
//
//

#include "SocialLayer.h"

#include "Facebook.h"
#include "WebService.h"
#include "AppService.h"
#include "cJSON.h"

#include "GameScene_Online.h"
#include "HelloWorldScene.h"

//define string
std::string g_strSocial[24][2] = {{"FRIENDS", "AMIGOS"},
    {"INVITE", "INVITA"},
    {"GIFTS", "REGALOS"},
    {"MESSAGE", "MENSAJES"},
    {"SEARCH", "BÚSQUEDA"},
    {"CONNECT TO FACEBOOK AND INVITE YOUR FRIENDS", "CONÉCTATE A FACEBOOK E INVITA A TUS AMIGOS"},
    {"GET 4,000 FREE POINTS PER REFERRAL", "OBTÉN 4,000 PUNTOS POR CADA AMIGO QUE INVITES"},
    {"ADD", "AGREGAR"},
    {"There is no gift", "No hay regalos"},
    {"There is no message", "No hay mensajes"},
    {"There is no friend", "No hay amigos"},
    {"There is no facebook friend for inviting", "No hay amigos para invitar"},
    {"Not found", "No encontrado"},
    {"Search Friends by name", "Buscar amigos por nombre"},
    {"Accept", "Aceptar"},
    {"sent you", "te envía"},
    {"You got", "Has ganado"},
    {"refferal points\n for invite friends", "puntos\n por invitar amigos"},
    {"Admin says : ", "El admin dice : "},
    {"points", "puntos"},
    {" Sent you friend request", " Te ha enviado una solicitud de amistad"},
    {" Sent you request to join table", " Te ha enviado una solicitud para unirte a una mesa"},
    {"%d minutes ago", "hace %d minutos"},
    {"You get %.0f points", "Has obtenido %.0f puntos"}
};

Layer* SocialLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool SocialLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("SocialLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    lb_status = (Text*)rootNode->getChildByName("status");
    lb_status->setString("");
    
    auto btnMain = (Button*)rootNode->getChildByName("btn_0");
    btnMain->setScaleX(g_fScaleWH);
    btnMain->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onMainBtnClicked, this, 0));
    
    for (int i = 0; i < 5; i++) {
        
        panel_main[i] = (Layout*)rootNode->getChildByName(StringUtils::format("panel_%d", i+1));
    }
    
    auto panel_tab = (Layout*)rootNode->getChildByName("panel_tab");
    for (int i = 0; i < 5; i++) {
        
        btn_tab[i] = (Button*)panel_tab->getChildByName(StringUtils::format("btn_%d", i));
        btn_tab[i]->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onMainBtnClicked, this, i+1));
        
        lb_title[i] = (Text*)btn_tab[i]->getChildByName("text");
        
        if (i == 2) {
            
            img_giftpush = (ImageView*)btn_tab[i]->getChildByName("push_mark");
            img_giftpush->setVisible(false);
        }
        else if (i == 3) {
            
            img_msgpush = (ImageView*)btn_tab[i]->getChildByName("push_mark");
            img_msgpush->setVisible(false);
        }
    }
    
    //fb panel
    panel_fb = (Layout*)rootNode->getChildByName("panel_fb");
    panel_fb->setScaleX(g_fScaleWH);
    panel_fb->setVisible(false);
    
    lb_title[5] = (Text*)panel_fb->getChildByName("title");
    lb_title[6] = (Text*)panel_fb->getChildByName("text");
    
    auto btn_fb = (Button*)panel_fb->getChildByName("btn_0");
    btn_fb->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onMainBtnClicked, this, 6));
    
    //effect node
    node_effect = (Node*)panel_fb->getChildByName("effectNode");
    node_effect->setScaleX(g_fScaleWH);
    
    pNodeAnimation = CSLoader::createTimeline("SpinWinNode.csb");
    pNodeAnimation->retain();
    pNodeAnimation->gotoFrameAndPause(0);
    
    //init frieds panel
    sv_friends = (ScrollView*)panel_main[0]->getChildByName("sv_list");
    sv_friends->setTouchEnabled(true);
    sv_friends->setClippingEnabled(true);
    sv_friends->setClippingType(Layout::ClippingType::SCISSOR);
    sv_friends->setVisible(false);
    
    //init invite panel
    panelCheck = (Layout*)panel_main[1]->getChildByName("panel_check");
    panelCheck->setVisible(false);
    
    lb_title[7] = (Text*)panelCheck->getChildByName("text");
    
    cb_all = (CheckBox*)panelCheck->getChildByName("cb_all");
    cb_all->setScaleX(g_fScaleWH);
    panelCheck->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onInviteItemClicked, this, 0, cb_all));
    
    btn_invite = (Button*)panel_main[1]->getChildByName("btn_invite");
    btn_invite->setScaleX(g_fScaleWH);
    btn_invite->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onMainBtnClicked, this, 7));
    btn_invite->setVisible(false);
    
    sv_invite = (ScrollView*)panel_main[1]->getChildByName("sv_friend");
    sv_invite->setTouchEnabled(true);
    sv_invite->setClippingEnabled(true);
    sv_invite->setClippingType(Layout::ClippingType::SCISSOR);
    sv_invite->setVisible(false);
    
    //init gift panel
    lv_gifts = (ListView*)panel_main[2]->getChildByName("lv_list");
    lv_gifts->setTouchEnabled(true);
    lv_gifts->setClippingEnabled(true);
    lv_gifts->setClippingType(Layout::ClippingType::SCISSOR);
    lv_gifts->setVisible(true);
    
    //init message panel
    lv_messages = (ListView*)panel_main[3]->getChildByName("lv_list");
    lv_messages->setTouchEnabled(true);
    lv_messages->setClippingEnabled(true);
    lv_messages->setClippingType(Layout::ClippingType::SCISSOR);
    lv_messages->setVisible(true);
    
    //init search panel
    sv_searches = (ScrollView*)panel_main[4]->getChildByName("sv_list");
    sv_searches->setTouchEnabled(true);
    sv_searches->setClippingEnabled(true);
    sv_searches->setClippingType(Layout::ClippingType::SCISSOR);
    sv_searches->setVisible(false);
    
    //auto panelList = (Layout*)panel_main[1]->getChildByName("panel_list");
    //Size szPanel = panelList->getBoundingBox().size;
    
    //editbox
    panel_search = (Layout*)rootNode->getChildByName("panel_search");
    auto panelEdit = (Layout*)panel_search->getChildByName("panel");
    Size szEdit = panelEdit->getBoundingBox().size;
    
    pEditBox = EditBox::create(Size(szEdit.width, szEdit.height), Scale9Sprite::create("transparent_bg.png"));
    pEditBox->setAnchorPoint(Vec2(0, 0));
    //pEditBox->setPosition(panelEdit->getPosition());
    pEditBox->setPlaceHolder(g_strSocial[13][g_nLangType].c_str());
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
    
    //init facebook connect layer
    pFBConnectLayer = new FBConnectLayer();
    pFBConnectLayer->createLayer(pCurLayer, 50, this);
    
    //init public profile layer
    pPublicProfileLayer = new PublicProfileLayer();
    pPublicProfileLayer->createLayer(pCurLayer, 50, this);
    
    //init alert layer
    pAlertLayer = new AlertLayer();
    pAlertLayer->createLayer(pCurLayer, 60, this);
    
    //init invite bonus layer
    pInviteBonusLayer = new InviteBonusLayer();
    pInviteBonusLayer->createLayer(pCurLayer, 60, this);
    
    //init msge layer
    pMsgLayer = new MsgLayer();
    pMsgLayer->createLayer(pCurLayer, 60, this);
  
    selectTab(0);

    return true;
}

void SocialLayer::updatePushMark()
{
    int newGift = 0, newMsg = 0;
    
    int cntGift = g_vGifts.size();
    if (cntGift > 0) {
        
        newGift = g_vGifts[cntGift-1].index - g_mainUserInfo.last_gift;
    }
    
    int cntMsg = g_vMessages.size();
    if (cntMsg > 0) {
        
        newMsg = g_vMessages[cntMsg-1].index - g_mainUserInfo.last_msg;
    }
    
    if (newGift > 0) {
        
        img_giftpush->setVisible(true);
    }
    else {
        
        img_giftpush->setVisible(false);
    }
    
    if (newMsg > 0) {
        
        img_msgpush->setVisible(true);
    }
    else {
        
        img_msgpush->setVisible(false);
    }
}

void SocialLayer::updateString2Lang()
{
    for (int i = 0; i < 7; i++) {
        
        lb_title[i]->setString(g_strSocial[i][g_nLangType]);
    }
    
    if (g_nLangType == 0) {
        
        lb_title[7]->setString("Select all");
        btn_invite->setTitleText("INVITE");
    }
    else {
        
        lb_title[7]->setString("Seleccionar todos");
        btn_invite->setTitleText("INVITAR");
    }
    
    pEditBox->setPlaceHolder(g_strSocial[13][g_nLangType].c_str());
}

void SocialLayer::onUpdatedDone()
{
    if (child_index == INVITEBONUS_LAYER) {
        
        selectTab(1);
    }
}

void SocialLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        if(btn_id == 0) { //close
            
           g_pHelloWorld->updatePushMark();
           hideLayer();
        }
        else if (btn_id == 6) { //fb connect
            
            Facebook::getInstance()->login();
            hideLayer();
        }
        else if(btn_id == 7) { //invite
            
            std::string str_ids = "";
            int index = INVALID;
            int cnt = sv_invite->getChildrenCount();
            for (int i = 0; i < cnt; i++) {
                
                Layout *panelItem = (Layout*)sv_invite->getChildByTag(i);
                auto cb_check = (CheckBox*)panelItem->getChildByName("check");
                if (cb_check->isSelected() == false) continue;
                
                index++;
                
                if (index > 0) {
                    
                    str_ids += ",";
                }
                str_ids += v_InvitableFrineds[i].fb_id;
            }
            
            if (index != INVALID) {
                
                Facebook::getInstance()->sendInviteRequest(str_ids);
            }

        }
        else { //tab buttons
            
            selectTab(btn_id-1);
        }
   }
}

void SocialLayer::selectTab(int index)
{
    lb_status->setString("");
    pEditBox->setVisible(false);
    panel_search->setVisible(false);
    
    panel_fb->setVisible(false);
    
    cur_tabIndex = index;
    for (int i = 0; i < 5; i++) {
        
        if (i == index) { //if selected tab
            
            btn_tab[i]->setBright(true);
            panel_main[i]->setVisible(true);
        }
        else {
            
            btn_tab[i]->setBright(false);
            panel_main[i]->setVisible(false);
        }
    }
    
    if(index == 0) { //friend tab
    
        if (g_vFriends.size() > 0) {
            
            showFriendsPanel();
        }
        else {
            
            lb_status->setString(g_strSocial[10][g_nLangType]);
        }
    }
    else if(index == 1) { //invite tab
        
        if (g_bLoginFB == false) {
            
            node_effect->stopAllActions();
            node_effect->setVisible(true);
            node_effect->runAction(pNodeAnimation);
            pNodeAnimation->setTimeSpeed(0.3);
            pNodeAnimation->gotoFrameAndPlay(0, true);
            
            panel_fb->setVisible(true);
        }
        else {
         
            if (g_vFBFriends.size() == 0) {
                
                lb_status->setString(g_strSocial[11][g_nLangType]);
            }
            else {
             
                v_InvitableFrineds.clear();
                for (int i = 0; i < g_vFBFriends.size(); i++) {
                    
                    v_InvitableFrineds.push_back(g_vFBFriends[i]);
                }
                showInvitePanel();
            }
        }
    }
    else if(index == 2) { //gift tab
        
        if (g_vGifts.size() > 0) {
           
            img_giftpush->setVisible(false);
            g_mainUserInfo.last_gift = g_vGifts[g_vGifts.size()-1].index;
            WebService::getInstance()->updateMsgGiftInfo();
            showGiftPanel();
        }
        else {
            
            getGiftList();
        }
    }
    else if(index == 3) { //message tab
        
        if (g_vMessages.size() > 0) {
            
            img_msgpush->setVisible(false);
            g_mainUserInfo.last_msg = g_vMessages[g_vMessages.size()-1].index;
            WebService::getInstance()->updateMsgGiftInfo();
            showMessagePanel();
        }
        else {
            
            getMsgList();
        }
    }
    else { //search tab
        
        pEditBox->setVisible(true);
        panel_search->setVisible(true);
        showSearchPanel();
    }
}

void SocialLayer::onInviteItemClicked(Ref* pSender, Widget::TouchEventType type, int index, CheckBox *pItem)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        bool flag = pItem->isSelected();
        pItem->setSelected(!flag);
        
        int cnt = sv_invite->getChildrenCount();
        if(index == 0) { //select all
            
            if(flag == false) {
                
                for (int i = 0; i < cnt; i++) {
                    
                    Layout *panelItem = (Layout*)sv_invite->getChildByTag(i);
                    auto cb_check = (CheckBox*)panelItem->getChildByName("check");
                    cb_check->setSelected(true);
                }
                btn_invite->setVisible(true);
            }
            else {
                
                for (int i = 0; i < cnt; i++) {
                    
                    Layout *panelItem = (Layout*)sv_invite->getChildByTag(i);
                    auto cb_check = (CheckBox*)panelItem->getChildByName("check");
                    cb_check->setSelected(false);
                }
                btn_invite->setVisible(false);
            }
        }
        else {
            
            bool flag = false;
            for (int i = 0; i < cnt; i++) {
                
                Layout *panelItem = (Layout*)sv_invite->getChildByTag(i);
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

void SocialLayer::showInvitePanel()
{
    btn_invite->setVisible(false);
    ///////////////////////////    scroll view module begin    //////////////////////////////////
    sv_invite->removeAllChildrenWithCleanup(true);
    
    int cntViewCol = 3; //view colum number
    int cntViewRow = 6; //view row number
    int cntTotalItems = v_InvitableFrineds.size(); //total item number
    
    if (cntTotalItems == 0) {
        
        panelCheck->setVisible(false);
        return;
    }
    else {
     
        pEditBox->setVisible(true);
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
        
        auto itemNode = CSLoader::createNode("InviteCellNode.csb");
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
 
        setImageFromUrl(v_InvitableFrineds[i].photo_id, img_photo);
        
        //name
        auto lb_name = (Text*)panelMain->getChildByName("name");
        lb_name->setString(getProperName(v_InvitableFrineds[i].user_name.c_str(), MAX_NAMELENGTH));
        
        //check box
        auto cb_item = (CheckBox*)panelMain->getChildByName("check");
        cb_item->setScaleX(g_fScaleWH);
        
        panelMain->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onInviteItemClicked, this, i+1, cb_item));
        
        itemNode->removeChild(panelMain, true);
        
        addItem2ScrollView(panelMain, i, cntViewCol, cntRow+offsetNum, sv_invite);
    }
    
    float wItemView = sv_invite->getBoundingBox().size.width;
    float hItemView = 0;//(sv_invite->getBoundingBox().size.height)*cntRow/(float)cntViewRow;
    if (cntTotalItems % 3 == 0)
    {
        hItemView = 156.0 * (cntTotalItems / 3);
    }
    else{
        hItemView = 156.0 * ((cntTotalItems / 3) + 1);
    }
    sv_invite->setInnerContainerSize(Size(wItemView, hItemView));
    sv_invite->jumpToTop();
    
    sv_invite->setVisible(true);
    /////////////////////////////   scroll view module end   ////////////////////////////////////
}

void SocialLayer::onFriendsItemClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        
        USER_INFO info;
        info.user_id = g_vFriends[btn_id].user_id;
        info.user_name = g_vFriends[btn_id].user_name;
        info.photo_id = g_vFriends[btn_id].photo_id;
        info.level = g_vFriends[btn_id].level;
        info.points = g_vFriends[btn_id].points;
        info.total_play = g_vFriends[btn_id].total_play;
        info.win_play = g_vFriends[btn_id].win_play;
        info.win_streak = g_vFriends[btn_id].win_streak;
        info.max_earning = g_vFriends[btn_id].max_earning;
        
        pPublicProfileLayer->showLayer(info, INVALID);
        
        //get profile data
        std::string strURL = StringUtils::format("http://%s:8080/sys/member/mobile/getUserInfo.do", WEBSERVICE_HOST);
        
        HttpRequest* request = new HttpRequest();
        request->setRequestType(HttpRequest::Type::POST);
        request->setUrl(strURL.c_str());
        request->setResponseCallback(CC_CALLBACK_2(SocialLayer::onGetPublicProfileDone, this, btn_id));
        
        // write the post data
        std::string strData = StringUtils::format("USER_ID=%s&FB_ID=%s",
                                                  info.user_id.c_str(),
                                                  info.user_id.c_str()
                                                  );
        request->setRequestData(strData.c_str(), strData.length());
        
        request->setTag("get Public Profile");
        //HttpClient::getInstance()->send(request);
        HttpClient::getInstance()->sendImmediate(request);
        
        request->release();
    }
}

void SocialLayer::onGetPublicProfileDone(HttpClient* sender, HttpResponse* response, int pos)
{
    if (response->isSucceed() == false){
        
        return;
    }
    
    std::vector<char>buffer = *response->getResponseData();
    std::string str_msg = &buffer[0];
    
    //parsing data
    cJSON *json = cJSON_Parse(str_msg.c_str());
    if (json == NULL) {
        
        return;
    }
    
    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
    if (jsonRes->valueint != 0) {
        
        return;
    }
    
    cJSON* data = cJSON_GetObjectItem(json, "user");
    
    USER_INFO userInfo;
    
    userInfo.friend_flag = cJSON_GetObjectItem(data, "friend")->valueint;
    
    userInfo.user_id = cJSON_GetObjectItem(data, "userid")->valuestring;
    userInfo.photo_id = cJSON_GetObjectItem(data, "photo_id")->valuestring;
    userInfo.user_name = cJSON_GetObjectItem(data, "name")->valuestring;
    userInfo.points = cJSON_GetObjectItem(data, "point")->valuedouble;
    userInfo.level = cJSON_GetObjectItem(data, "level")->valueint;
    userInfo.level_point = cJSON_GetObjectItem(data, "levelpoint")->valueint;
    userInfo.total_play = cJSON_GetObjectItem(data, "totalplay")->valueint;
    userInfo.win_play = cJSON_GetObjectItem(data, "winplay")->valueint;
    
    userInfo.max_earning = cJSON_GetObjectItem(data, "max_earning")->valueint;
    userInfo.win_streak = cJSON_GetObjectItem(data, "win_streak")->valueint;
    userInfo.total_hour = cJSON_GetObjectItem(data, "total_hour")->valueint;
    userInfo.total_cleanpot = cJSON_GetObjectItem(data, "total_cleanpot")->valueint;
    userInfo.total_earning = cJSON_GetObjectItem(data, "total_earning")->valueint;
    userInfo.total_tourplay = cJSON_GetObjectItem(data, "total_tournament")->valueint;
    userInfo.total_emoticon = cJSON_GetObjectItem(data, "total_emoticon")->valueint;
    userInfo.total_invite = cJSON_GetObjectItem(data, "total_invite")->valueint;
    userInfo.total_avatars = cJSON_GetObjectItem(data, "total_avatars")->valueint;
    
    cJSON_Delete(json);
    
    if(g_vFriends[pos].user_id.compare(userInfo.user_id) != 0) return;
    
    g_vFriends[pos].level = userInfo.level;
    g_vFriends[pos].points = userInfo.points;
    g_vFriends[pos].total_play = userInfo.total_play;
    g_vFriends[pos].win_play = userInfo.win_play;
    g_vFriends[pos].win_streak = userInfo.win_streak;
    g_vFriends[pos].max_earning = userInfo.max_earning;
    
    if (pPublicProfileLayer->isVisible() == true && pPublicProfileLayer->user_info.user_id.compare(userInfo.user_id) == 0) {
        
        pPublicProfileLayer->showLayer(userInfo, INVALID);
    }
}

void SocialLayer::showFriendsPanel()
{
    ///////////////////////////    scroll view module begin    //////////////////////////////////
    sv_friends->removeAllChildrenWithCleanup(true);
    
    int cntViewCol = 2; //view colum number
    int cntViewRow = 4; //view row number
    int cntTotalItems = g_vFriends.size(); //total item number
    
    int cntRow = cntTotalItems/cntViewCol;
    if ((cntTotalItems%cntViewCol) > 0) {
        
        cntRow++;
    }
    int offsetNum = 0;
    if (cntRow < cntViewRow) {
        
        offsetNum = cntViewRow - cntRow;
    }
    
    for (int i = 0; i < cntTotalItems; i++) {
        
        auto itemNode = CSLoader::createNode("FriendCellNode.csb");
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
        float scaleX1 = (panelPhoto->getBoundingBox().size.width-10) / (float)img_photo->getBoundingBox().size.width;
        float scaleY1 = (panelPhoto->getBoundingBox().size.height-10) / (float)img_photo->getBoundingBox().size.height;
        img_photo->ignoreContentAdaptWithSize(false);
        img_photo->setScale(scaleX1, scaleY1);
        img_photo->setPosition(panelPhoto->getPosition());
        clippingNode->addChild(img_photo);
        
        auto imgBorder = ImageView::create("border.png", TextureResType::PLIST);
        imgBorder->setScale(scaleX, scaleY);
        imgBorder->setPosition(panelPhoto->getPosition());
        panelMain->addChild(imgBorder);
        
        WebService::getInstance()->setProfileImage(g_vFriends[i].photo_id, img_photo);
        //setImageFromUrl(v_InvitableFrineds[i].photo_id, img_photo);
        
        //name
        auto lb_name = (Text*)panelMain->getChildByName("name");
        lb_name->setString(g_vFriends[i].user_name);
        //level
        auto lb_level = (Text*)panelMain->getChildByName("level");
        lb_level->setString(StringUtils::format("Level %d", g_vFriends[i].level+1));
        //point
        auto lb_point = (Text*)panelMain->getChildByName("point");
        lb_point->setString(getNormalText2Number(g_vFriends[i].points));
        //chip mark
        auto mark = (ImageView*)panelMain->getChildByName("mark");
        mark->setScaleX(g_fScaleWH);
        //button
        auto btn_info = (Button*)panelMain->getChildByName("btn_0");
        btn_info->setScaleX(g_fScaleWH);
        btn_info->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onFriendsItemClicked, this, i));
        
        //panelMain->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onInviteItemClicked, this, i+1, cb_item));
        
        itemNode->removeChild(panelMain, true);
        
        addItem2ScrollView(panelMain, i, cntViewCol, cntRow+offsetNum, sv_friends);
    }
    
    float wItemView = sv_friends->getBoundingBox().size.width;
    float hItemView = (sv_friends->getBoundingBox().size.height)*cntRow/(float)cntViewRow;
    sv_friends->setInnerContainerSize(Size(wItemView, hItemView));
    sv_friends->jumpToTop();
    
    sv_friends->setVisible(true);
    /////////////////////////////   scroll view module end   ////////////////////////////////////
}

void SocialLayer::showGiftPanel()
{
    panel_search->setVisible(false);
    ///////////////////////////    List view module begin    //////////////////////////////////
    lv_gifts->removeAllChildrenWithCleanup(true);
    
    int cntTotalItems = g_vGifts.size(); //total item number
    if (cntTotalItems == 0) {
     
        lb_status->setString(g_strSocial[8][g_nLangType]);
        return;
    }
    
    for (int i = 0; i < cntTotalItems; i++) {
        
        auto itemNode = CSLoader::createNode("GiftCellNode.csb");
        auto panelMain = (Layout*)itemNode->getChildByName("panel");
        panelMain->setTag(i);
        
        //photo
        auto panelPhoto = (ImageView*)panelMain->getChildByName("photo");
        panelPhoto->setScaleX(g_fScaleWH);
        
      
        //panel for admin
        auto panel_0 = (Layout*)panelMain->getChildByName("panel_0");
        auto panel_1 = (Layout*)panelMain->getChildByName("panel_1");
        
        //button
        auto btn_accept = (Button*)panelMain->getChildByName("btn_0");
        btn_accept->setTitleText(g_strSocial[14][g_nLangType]);
        btn_accept->setScaleX(g_fScaleWH);
        btn_accept->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onGiftItemClicked, this, i));
        
        if (g_vGifts[i].type == 0) { //admin gift for all users
            
            panel_1->setVisible(false);
            auto lb_text = (Text*)panel_0->getChildByName("text_2");
            lb_text->setString(g_strSocial[15][g_nLangType]);
            auto img_mark = (ImageView*)panel_0->getChildByName("mark");
            img_mark->setScaleX(g_fScaleWH);
            auto lb_point = (Text*)panel_0->getChildByName("point");
            lb_point->setString(StringUtils::format("%s %s", getNormalText2Number(g_vGifts[i].bonus).c_str(), g_strSocial[19][g_nLangType].c_str()));
         }
        else { //bonus for friend invite
            
            panel_0->setVisible(false);
            auto img_mark = (ImageView*)panel_1->getChildByName("mark");
            img_mark->setScaleX(g_fScaleWH);
            auto lb_text = (Text*)panel_1->getChildByName("text_1");
            lb_text->setString(g_strSocial[16][g_nLangType]);
            auto lb_point = (Text*)panel_1->getChildByName("point");
            lb_point->setString(StringUtils::format("%s %s", getNormalText2Number(g_vGifts[i].bonus).c_str(), g_strSocial[17][g_nLangType].c_str()));
        }
        //panelMain->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onInviteItemClicked, this, i+1, cb_item));
        
        itemNode->removeChild(panelMain, true);
        
        lv_gifts->pushBackCustomItem(panelMain);
    }
    lv_gifts->setVisible(true);
    /////////////////////////////   List view module end   ////////////////////////////////////
}

void SocialLayer::onGiftItemClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        
        WebService::getInstance()->acceptGiftBonus(g_vGifts[btn_id].index);
        
        g_mainUserInfo.points += g_vGifts[btn_id].bonus;
        
        if (g_vGifts[btn_id].type == 1) { //if friend bonus
            
            pInviteBonusLayer->showLayer();
        }
        else {
        
            pAlertLayer->showLayer("", StringUtils::format(g_strSocial[23][g_nLangType].c_str(), g_vGifts[btn_id].bonus));
        }
        
        WebService::getInstance()->updatePoint();
        
        g_vGifts.erase(g_vGifts.begin() + btn_id);
        
        if (cur_tabIndex == 2) {
         
            showGiftPanel();
        }
        
        pDelegate->onUpdatedDone();
    }
}

//void SocialLayer::showMessagePanel()
//{
//    ///////////////////////////    List view module begin    //////////////////////////////////
//    lv_messages->removeAllChildrenWithCleanup(true);
//    
//    int cntTotalItems = g_vMessages.size(); //total item number
//    if (cntTotalItems == 0) {
//        
//        lb_status->setString(g_strSocial[9][g_nLangType]);
//        return;
//    }
//    
//    for (int i = 0; i < cntTotalItems; i++) {
//        
//        auto itemNode = CSLoader::createNode("MessageCellNode.csb");
//        auto panelMain = (Layout*)itemNode->getChildByName("panel");
//        panelMain->setTag(i);
//        
//        //photo
//        auto panelPhoto = (ImageView*)panelMain->getChildByName("photo");
//        panelPhoto->setScaleX(g_fScaleWH);
//        
//        auto panelTitle = (ListView*)panelMain->getChildByName("panel_title");
//        auto lb_name = (Text*)panelTitle->getChildByName("text_0");
//        auto lb_title = (Text*)panelTitle->getChildByName("text_1");
//        auto lb_text = (Text*)panelMain->getChildByName("text");
//        if (g_vMessages[i].type == 0) { //admin message
//            
//            panelMain->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onMessagePanelClicked, this, i));
//            lb_name->setString(g_strSocial[18][g_nLangType]);
//            lb_title->setString(getProperName(g_vMessages[i].title.c_str(), 30));
//            
//            lb_text->setFontSize(48);
//            lb_text->setString(getProperName(g_vMessages[i].msg.c_str(), 50));
//        }
//        else if (g_vMessages[i].type == 1) { //friend request
//            
//            lb_name->setString(g_vMessages[i].name);
//            lb_title->setString(g_strSocial[20][g_nLangType]);
//            
//            lb_text->setFontSize(36);
//            lb_text->setString("");
//        }
//        else if (g_vMessages[i].type == 2) { //table invite
//            
//            lb_name->setString(g_vMessages[i].name);
//            lb_title->setString(g_strSocial[21][g_nLangType]);
//            
//            int pass_time = getDurationTime(g_vMessages[i].sent_time, MINUTE_TYPE);
//            
//            lb_text->setFontSize(36);
//            lb_text->setString(StringUtils::format(g_strSocial[22][g_nLangType].c_str(), pass_time));
//        }
//        
//        if (g_vMessages[i].type != 0) {
//            
//            //clipping node
//            auto clippingNode = ClippingNode::create();
//            clippingNode->setAlphaThreshold(MASKING_THRESHOLD);
//            clippingNode->setPosition(0, 0);
//            clippingNode->setAnchorPoint(Vec2(0.0, 0.0));
//            panelMain->addChild(clippingNode);
//            
//            auto mask = Sprite::createWithSpriteFrameName("img_mask.png");
//            float scaleX = (panelPhoto->getBoundingBox().size.width) / (float)mask->getBoundingBox().size.width;
//            float scaleY = (panelPhoto->getBoundingBox().size.height) / (float)mask->getBoundingBox().size.height;
//            mask->setScale(scaleX, scaleY);
//            mask->setPosition(panelPhoto->getPosition());
//            clippingNode->setStencil(mask);
//            
//            ImageView *img_photo = ImageView::create("img_mask.png", TextureResType::PLIST);
//            img_photo->ignoreContentAdaptWithSize(false);
//            img_photo->setScale(scaleX, scaleY);
//            img_photo->setPosition(panelPhoto->getPosition());
//            clippingNode->addChild(img_photo);
//            
//            auto imgBorder = ImageView::create("border.png", TextureResType::PLIST);
//            imgBorder->setScale(scaleX, scaleY);
//            imgBorder->setPosition(panelPhoto->getPosition());
//            panelMain->addChild(imgBorder);
//            
//            WebService::getInstance()->setProfileImage(g_vMessages[i].photo_id, img_photo);
//            
//            //button
//            auto btn_accept = (Button*)panelMain->getChildByName("btn_0");
//            btn_accept->setVisible(true);
//            btn_accept->setScaleX(g_fScaleWH);
//            btn_accept->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onMessageItemClicked, this, i, 1));
//            auto btn_cancel = (Button*)panelMain->getChildByName("btn_1");
//            btn_cancel->setVisible(true);
//            btn_cancel->setScaleX(g_fScaleWH);
//            btn_cancel->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onMessageItemClicked, this, i, 0));
//        } else {
//            
//            auto btn_cancel = (Button*)panelMain->getChildByName("btn_1");
//            btn_cancel->setVisible(true);
//            btn_cancel->setScaleX(g_fScaleWH);
//            btn_cancel->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onMessageItemClicked, this, i, 0));
//        }
//        
//        //panelMain->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onInviteItemClicked, this, i+1, cb_item));
//        
//        itemNode->removeChild(panelMain, true);
//        lv_messages->pushBackCustomItem(panelMain);
//    }
//    lv_messages->setVisible(true);
//    /////////////////////////////   List view module end   ////////////////////////////////////
//}


void SocialLayer::showMessagePanel()
{
    bool bAddedFriendRequest = false;
    ///////////////////////////    List view module begin    //////////////////////////////////
    lv_messages->removeAllChildrenWithCleanup(true);
    
    int cntTotalItems = g_vMessages.size(); //total item number
    if (cntTotalItems == 0) {
        
        lb_status->setString(g_strSocial[9][g_nLangType]);
        return;
    }
    
    
    for (int i = 0; i < cntTotalItems; i++) {
        bAddedFriendRequest = false;
        
        auto itemNode = CSLoader::createNode("MessageCellNode.csb");
        auto panelMain = (Layout*)itemNode->getChildByName("panel");
        panelMain->setTag(i);
        
        //photo
        auto panelPhoto = (ImageView*)panelMain->getChildByName("photo");
        panelPhoto->setScaleX(g_fScaleWH);
        
        auto panelTitle = (ListView*)panelMain->getChildByName("panel_title");
        auto lb_name = (Text*)panelTitle->getChildByName("text_0");
        auto lb_title = (Text*)panelTitle->getChildByName("text_1");
        auto lb_text = (Text*)panelMain->getChildByName("text");
        
        if (g_vMessages[i].type == 0) { //admin message
            
            panelMain->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onMessagePanelClicked, this, i));
            lb_name->setString(g_strSocial[18][g_nLangType]);
            lb_title->setString(getProperName(g_vMessages[i].title.c_str(), 30));
            
            lb_text->setFontSize(48);
            lb_text->setString(getProperName(g_vMessages[i].msg.c_str(), 50));
        }
        else if (g_vMessages[i].type == 1) { //friend request
            
            lb_name->setString(g_vMessages[i].name);
            lb_title->setString(g_strSocial[20][g_nLangType]);
            
            lb_text->setFontSize(36);
            lb_text->setString("");
            
            for(int j = 0 ; j < i ; j++){	//Check Already Added
                if (g_vMessages[j].type == 1){
                    if (g_vMessages[j].name == g_vMessages[i].name){
                        bAddedFriendRequest = true;
                    }
                }
            }
        }
        else if (g_vMessages[i].type == 2) { //table invite
            
            lb_name->setString(g_vMessages[i].name);
            lb_title->setString(g_strSocial[21][g_nLangType]);
            
            int pass_time = getDurationTime(g_vMessages[i].sent_time, MINUTE_TYPE);
            
            lb_text->setFontSize(36);
            lb_text->setString(StringUtils::format(g_strSocial[22][g_nLangType].c_str(), pass_time));
        }
        
        if (g_vMessages[i].type != 0) {
            
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
            
            WebService::getInstance()->setProfileImage(g_vMessages[i].photo_id, img_photo);
            
            //button
            auto btn_accept = (Button*)panelMain->getChildByName("btn_0");
            btn_accept->setVisible(true);
            btn_accept->setScaleX(g_fScaleWH);
            btn_accept->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onMessageItemClicked, this, i, 1));
            auto btn_cancel = (Button*)panelMain->getChildByName("btn_1");
            btn_cancel->setVisible(true);
            btn_cancel->setScaleX(g_fScaleWH);
            btn_cancel->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onMessageItemClicked, this, i, 0));
        } else {
            
            auto btn_cancel = (Button*)panelMain->getChildByName("btn_1");
            btn_cancel->setVisible(true);
            btn_cancel->setScaleX(g_fScaleWH);
            btn_cancel->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onMessageItemClicked, this, i, 0));
        }
        
        //panelMain->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onInviteItemClicked, this, i+1, cb_item));
        if (bAddedFriendRequest == false){
            itemNode->removeChild(panelMain, true);
            lv_messages->pushBackCustomItem(panelMain);
        }
    }
    lv_messages->setVisible(true);
    /////////////////////////////   List view module end   ////////////////////////////////////
}
void SocialLayer::onMessagePanelClicked(Ref* pSender, Widget::TouchEventType type, int index)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        pMsgLayer->showLayer(g_vMessages[index].title, g_vMessages[index].msg);
    }
}

void SocialLayer::onMessageItemClicked(Ref* pSender, Widget::TouchEventType type, int index, int response)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        
        if (g_vMessages[index].type == 1) { //friend message
            
            if(response == 1) {
                
                WebService::getInstance()->sendFriendRequest2User(g_vMessages[index].sender_id);
            }
        }
        else { //invite message for table
            
            if(response == 1) {
                
                std::string strMsg = g_vMessages[index].msg;
                
                std::size_t loc = strMsg.find(':');
                std::string str_buyin = strMsg.substr(0, loc);
                std::string str_fee = strMsg.substr(loc+1);
                
                int buyin = atoi(str_buyin.c_str());
                int fee = atoi(str_fee.c_str());
                
                if (g_mainUserInfo.points < fee) {
                    
                    if(g_nLangType == 0) {
                        
                        pAlertLayer->showLayer("", "You don't have enough points");
                    }
                    else {
                        
                        pAlertLayer->showLayer("", "No tienes suficientes puntos");
                    }
                }
                else {
                 
                    WebService::getInstance()->acceptMsg(g_vMessages[index].index);
                    g_vMessages.erase(g_vMessages.begin() + index);
                    
                    g_nBuyin = buyin;
                    g_nPlayFee = fee;
                    if (g_nBuyin < 0 || g_mainUserInfo.points < g_nBuyin) {
                        
                        g_nBuyin = g_mainUserInfo.points;
                    }
                    
                    AppService::showCircleProgressbar();
                    
                    g_nRoomType = VIP_ROOM;
                    g_roomId = g_vMessages[index].title;
                    Scene *pScene = TransitionFade::create(0.5, GameScene_Online::createScene());
                    Director::getInstance()->replaceScene(pScene);
                    
                    return;
                }
            }
        }
        
        WebService::getInstance()->acceptMsg(g_vMessages[index].index);
        g_vMessages.erase(g_vMessages.begin() + index);
        
        if (cur_tabIndex == 3) {
            
            showMessagePanel();
        }
    }
}

void SocialLayer::showSearchPanel()
{
    ///////////////////////////    scroll view module begin    //////////////////////////////////
    sv_searches->removeAllChildrenWithCleanup(true);
    
    int cntViewCol = 2; //view colum number
    int cntViewRow = 3; //view row number
    int cntTotalItems = g_vSearchUsers.size(); //total item number
    
//    if (cntTotalItems == 0) {
//        
//        lb_status->setString("There is no friends");
//    }
    
    int cntRow = cntTotalItems/cntViewCol;
    if ((cntTotalItems%cntViewCol) > 0) {
        
        cntRow++;
    }
    int offsetNum = 0;
    if (cntRow < cntViewRow) {
        
        offsetNum = cntViewRow - cntRow;
    }
    
    for (int i = 0; i < cntTotalItems; i++) {
        
        auto itemNode = CSLoader::createNode("SearchCellNode.csb");
        auto panelMain = (Layout*)itemNode->getChildByName("panel");
        panelMain->setTag(i);
        
        //photo
        auto panelPhoto = (ImageView*)panelMain->getChildByName("photo");
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
        float scaleX1 = (panelPhoto->getBoundingBox().size.width-10) / (float)img_photo->getBoundingBox().size.width;
        float scaleY1 = (panelPhoto->getBoundingBox().size.height-10) / (float)img_photo->getBoundingBox().size.height;
        img_photo->ignoreContentAdaptWithSize(false);
        img_photo->setScale(scaleX1, scaleY1);
        img_photo->setPosition(panelPhoto->getPosition());
        clippingNode->addChild(img_photo);
        
        auto imgBorder = ImageView::create("border.png", TextureResType::PLIST);
        imgBorder->setScale(scaleX, scaleY);
        imgBorder->setPosition(panelPhoto->getPosition());
        panelMain->addChild(imgBorder);
        
        WebService::getInstance()->setProfileImage(g_vSearchUsers[i].photo_id, img_photo);
        //setImageFromUrl(v_InvitableFrineds[i].photo_id, img_photo);
        
        //id
        auto lb_id = (Text*)panelMain->getChildByName("id");
        lb_id->setString(StringUtils::format("ID %s", g_vSearchUsers[i].user_id.c_str()));
        //name
        auto lb_name = (Text*)panelMain->getChildByName("name");
        lb_name->setString(g_vSearchUsers[i].user_name);
        //level
        auto lb_level = (Text*)panelMain->getChildByName("level");
        lb_level->setString(StringUtils::format("Level %d", g_vSearchUsers[i].level+1));
        //point
        auto lb_point = (Text*)panelMain->getChildByName("point");
        lb_point->setString(getNormalText2Number(g_vSearchUsers[i].points));
        //chip mark
        auto mark = (ImageView*)panelMain->getChildByName("mark");
        mark->setScaleX(g_fScaleWH);
        //button
        auto btn_invite = (Button*)panelMain->getChildByName("btn_0");
        btn_invite->setTitleText(g_strSocial[7][g_nLangType]);
        btn_invite->setScaleX(g_fScaleWH);
        btn_invite->addTouchEventListener(CC_CALLBACK_2(SocialLayer::onSearchItemClicked, this, i, btn_invite));
        
        itemNode->removeChild(panelMain, true);
        
        addItem2ScrollView(panelMain, i, cntViewCol, cntRow+offsetNum, sv_searches);
    }
    
    float wItemView = sv_searches->getBoundingBox().size.width;
    float hItemView = (sv_searches->getBoundingBox().size.height)*cntRow/(float)cntViewRow;
    sv_searches->setInnerContainerSize(Size(wItemView, hItemView));
    sv_searches->jumpToTop();
    
    sv_searches->setVisible(true);
    /////////////////////////////   scroll view module end   ////////////////////////////////////    

}

void SocialLayer::onSearchItemClicked(Ref* pSender, Widget::TouchEventType type, int btn_id, Button *pItem)
{
    if (type == Widget::TouchEventType::ENDED) {        
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        
        WebService::getInstance()->sendFriendRequest2User(g_vSearchUsers[btn_id].user_id);
        
        // pItem->setEnabled(false);
        // pItem->setBright(false);
    }
}

void SocialLayer::showLayer()
{
    //update info
    //WebService::getInstance()->setProfileImage(info.photo_id, img_photo);
    selectTab(0);
    updateString2Lang();
    updatePushMark();
    pCurLayer->setVisible(true);
}

void SocialLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}

void SocialLayer::searchUserList(std::string name)
{
    g_vSearchUsers.clear();
    
    std::string strUrl = StringUtils::format("http://%s:8080/sys/member/mobile/fetchMembers.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(CC_CALLBACK_2(SocialLayer::onUserListDownloadCompleted, this));
    
    // write the post data
    std::string strData = StringUtils::format("NAME=%s", name.c_str());
    request->setRequestData(strData.c_str(), strData.length());
    request->setTag("Search User List");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void SocialLayer::onUserListDownloadCompleted(HttpClient* sender, HttpResponse* response)
{
    AppService::hideCircleProgressbar();
    
    if (response->isSucceed() == false){
        
        //lbStatus->setString("Cannot get data !");
        return;
    }
    
    std::vector<char>buffer = *response->getResponseData();
    std::string str_msg = &buffer[0];
    
    //parsing data
    cJSON *json = cJSON_Parse(str_msg.c_str());
    if (json == NULL) {
        
        //lbStatus->setString("Cannot get data !");
        return;
    }
    
    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
    if (jsonRes->valueint != 0) {
        
        //lbStatus->setString("Cannot get data !");
        return;
    }
    
    cJSON* datas = cJSON_GetObjectItem(json, "members");
    
    int count = cJSON_GetArraySize(datas);
    for (int i = 0; i < count; i++) {
        
        cJSON* data = cJSON_GetArrayItem(datas, i);
        
        USER_INFO info;
        
        //user id
        info.user_id = cJSON_GetObjectItem(data, "userid")->valuestring;
        if (info.user_id.compare(g_mainUserInfo.user_id) == 0) {
            
            continue;
        }
        
        //name
        info.user_name = cJSON_GetObjectItem(data, "name")->valuestring;
        //photo
        info.photo_id = cJSON_GetObjectItem(data, "photo_id")->valuestring;
        //total point
        info.points = cJSON_GetObjectItem(data, "point")->valuedouble;
        //level
        info.level = cJSON_GetObjectItem(data, "level")->valueint;
        //level point
        info.level_point = cJSON_GetObjectItem(data, "levelpoint")->valueint;
        //total play
        info.total_play = cJSON_GetObjectItem(data, "totalplay")->valueint;
        //total win
        info.win_play = cJSON_GetObjectItem(data, "winplay")->valueint;
        //win streak
        info.win_streak = cJSON_GetObjectItem(data, "win_streak")->valueint;
        //max earning
        info.max_earning = cJSON_GetObjectItem(data, "max_earning")->valuedouble;
        //total earning
        info.total_earning = cJSON_GetObjectItem(data, "total_earning")->valuedouble;
        //total hour
        info.total_hour = cJSON_GetObjectItem(data, "total_hour")->valueint;
        //total cleanpot
        info.total_cleanpot = cJSON_GetObjectItem(data, "total_cleanpot")->valueint;
        //total tournament
        info.total_tourplay = cJSON_GetObjectItem(data, "total_tournament")->valueint;
        //total emoticon
        info.total_emoticon = cJSON_GetObjectItem(data, "total_emoticon")->valueint;
        //total avatars
        info.total_avatars = cJSON_GetObjectItem(data, "total_avatars")->valueint;
        //total invite
        info.total_invite = cJSON_GetObjectItem(data, "total_invite")->valueint;
        
        g_vSearchUsers.push_back(info);
    }
    cJSON_Delete(json);
    
    if (g_vSearchUsers.size() > 0) {
        
        showSearchPanel();
    }
    else {
        
        lb_status->setString(g_strSocial[12][g_nLangType]);
        return;
    }
}

void SocialLayer::actionNewMsg()
{
    updatePushMark();
    if (panel_main[3]->isVisible()==true) {
        
        lb_status->setString("");
        showMessagePanel();
    }
}

void SocialLayer::actionNewGift()
{
    updatePushMark();
    if (panel_main[2]->isVisible()==true) {
        
        lb_status->setString("");
        showGiftPanel();
    }
}

void SocialLayer::actionNewFriend()
{
    if (panel_main[2]->isVisible()==true) {
        
        lb_status->setString("");
        showFriendsPanel();
    }
}

void SocialLayer::getGiftList()
{
    AppService::showCircleProgressbar();
    
    //g_vGifts.clear();
    
    std::string strUrl = StringUtils::format("http://%s:8080/sys/gift/mobile/getUserGifts.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(CC_CALLBACK_2(SocialLayer::onGiftListDownloadCompleted, this));
    
    // write the post data
    std::string strData = StringUtils::format("RECEIVER_ID=%s", g_mainUserInfo.user_id.c_str());
    request->setRequestData(strData.c_str(), strData.length());
    request->setTag("Get Gift List");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void SocialLayer::onGiftListDownloadCompleted(HttpClient* sender, HttpResponse* response)
{
    AppService::hideCircleProgressbar();
    
    if (response->isSucceed() == false){
        
        lb_status->setString(g_strSocial[8][g_nLangType]);
        return;
    }
    
    std::vector<char>buffer = *response->getResponseData();
    std::string str_msg = &buffer[0];
    
    //parsing data
    cJSON *json = cJSON_Parse(str_msg.c_str());
    if (json == NULL) {
        
        lb_status->setString(g_strSocial[8][g_nLangType]);
        return;
    }
    
    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
    if (jsonRes->valueint != 0) {
        
        lb_status->setString(g_strSocial[8][g_nLangType]);
        return;
    }
    
    cJSON* datas = cJSON_GetObjectItem(json, "gifts");
    
    int count = cJSON_GetArraySize(datas);
    int num = 0;
    for (int i = 0; i < count; i++) {
        
        cJSON* data = cJSON_GetArrayItem(datas, i);
        
        GIFT_INFO info;
        
        //no
        info.index = cJSON_GetObjectItem(data, "no")->valueint;
        //type
        info.type = cJSON_GetObjectItem(data, "type")->valueint;
        //bonus
        info.bonus = cJSON_GetObjectItem(data, "bonus")->valuedouble;
        
        bool flag = addGift(info);
        if (flag == true) {
            
            num++;
        }
    }
    cJSON_Delete(json);
    
//    if (num > 0) {
//        
//        WebService::getInstance()->updateMsgGiftInfo();
//    }
    
    if (g_vGifts.size() > 0) {
        
        showGiftPanel();
    }
    else {
        
        lb_status->setString(g_strSocial[8][g_nLangType]);
        return;
    }
}

void SocialLayer::getMsgList()
{
    AppService::showCircleProgressbar();
    
    //g_vMessages.clear();
    
    std::string strUrl = StringUtils::format("http://%s:8080/sys/msg/mobile/getUserMsgs.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(CC_CALLBACK_2(SocialLayer::onMsgListDownloadCompleted, this));
    
    // write the post data
    std::string strData = StringUtils::format("RECEIVER_ID=%s", g_mainUserInfo.user_id.c_str());
    request->setRequestData(strData.c_str(), strData.length());
    request->setTag("Get Msg List");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void SocialLayer::onMsgListDownloadCompleted(HttpClient* sender, HttpResponse* response)
{
    AppService::hideCircleProgressbar();
    
    if (response->isSucceed() == false){
        
        lb_status->setString(g_strSocial[9][g_nLangType]);
        return;
    }
    
    std::vector<char>buffer = *response->getResponseData();
    std::string str_msg = &buffer[0];
    
    //parsing data
    cJSON *json = cJSON_Parse(str_msg.c_str());
    if (json == NULL) {
        
        lb_status->setString(g_strSocial[9][g_nLangType]);
        return;
    }
    
    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
    if (jsonRes->valueint != 0) {
        
        lb_status->setString(g_strSocial[9][g_nLangType]);
        return;
    }
    
    cJSON* datas = cJSON_GetObjectItem(json, "msgs");
    
    int count = cJSON_GetArraySize(datas);
    for (int i = 0; i < count; i++) {
        
        cJSON* data = cJSON_GetArrayItem(datas, i);
        
        MSG_INFO info;
        
        //no
        info.index = cJSON_GetObjectItem(data, "no")->valueint;
        //type
        info.type = cJSON_GetObjectItem(data, "type")->valueint;
        //sender id
        info.sender_id = cJSON_GetObjectItem(data, "sender_id")->valuestring;
        //name
        info.name = cJSON_GetObjectItem(data, "name")->valuestring;
        //photo
        info.photo_id = cJSON_GetObjectItem(data, "photo")->valuestring;
        //title
        info.title = cJSON_GetObjectItem(data, "title")->valuestring;
        //msg
        info.msg = cJSON_GetObjectItem(data, "msg")->valuestring;
        //time
        info.sent_time = cJSON_GetObjectItem(data, "time")->valueint;
        
        addMsg(info);
    }
    cJSON_Delete(json);
    
    if (g_vMessages.size() > 0) {
        
        showMessagePanel();
    }
    else {
        
        lb_status->setString(g_strSocial[9][g_nLangType]);
        return;
    }
}


//////////////////////// editbox /////////////////////////////
////////  virtual functions ----------------
void SocialLayer::editBoxEditingDidBegin(EditBox *editBox)
{
    //    Sequence * act2 = Sequence::create(
    //                                       DelayTime::create(0.1),
    //                                       MoveTo::create(0.3, Vec2(0, getScreenHeight()*0.3)),
    //                                       NULL);
    //    vipLayer->runAction(act2);
    
    //vipLayer->setPosition(0, getScreenHeight()*0.3);
}

void SocialLayer::editBoxEditingDidEnd(EditBox *editBox)
{
    const char *strTxt = editBox->getText();
    
    if(cur_tabIndex == 1) { //invite tab
        
        if (strlen(strTxt) == 0) {
            
            //total facebook friends
            for (int i = 0; i < g_vFBFriends.size(); i++) {
                
                v_InvitableFrineds.push_back(g_vFBFriends[i]);
            }
        }
        else {
        
            //find only firends with name
            v_InvitableFrineds.clear();
            for (int i = 0; i < g_vFBFriends.size(); i++) {
                
                if(g_vFBFriends[i].user_name.find(strTxt) == std::string::npos) continue;
                
                v_InvitableFrineds.push_back(g_vFBFriends[i]);
            }
        }
        
        showInvitePanel();
    }
    else if(cur_tabIndex == 4) { //search user tab
        
        //remove search list
        if (strlen(strTxt) == 0) {
            
            g_vSearchUsers.clear();
            sv_searches->removeAllChildrenWithCleanup(true);
            
            lb_status->setString(g_strSocial[12][g_nLangType]);
            return;
        }
        
        lb_status->setString("");
        sv_searches->removeAllChildrenWithCleanup(true);
        
        AppService::showCircleProgressbar();
        searchUserList(strTxt);
    }
}

void SocialLayer::editBoxTextChanged(EditBox *editBox, std::string &text) {
    
    bool flag = true;
}

void SocialLayer::editBoxReturn(EditBox *editBox) {
    
    bool flag = true;
}





