//
//  PublicProfileLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 4/28/16.
//
//

#include "PublicProfileLayer.h"

#include "WebService.h"
#include "cJSON.h"

//define string
std::string g_strPublicProfile[12][2] = {{"Leaderboard Rank", "Ranking de Lideres"},
    {"Max Earning", "Ganancia Máxima"},
    {"Tournament Played", "Torneos Jugados"},
    {"Win Ratio", "Victorias"},
    {"SEND TO ALL", "Enviar A Todos"},
    {"You will be charge", "Tendras un cargo de"},
    {"for each Emoticon", "por cada Emoticon"},
    {"REQUEST SENT", "SOLICITUD ENVIADA"}
};

Layer* PublicProfileLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool PublicProfileLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("PublicProfileLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    for (int i = 1; i < 5; i++) {
        
        lb_title[i-1] = (Text*)rootNode->getChildByName(StringUtils::format("comment_%d", i));
    }
    
    auto img_mark = (ImageView*)rootNode->getChildByName(StringUtils::format("mark_%d", 0));
    img_mark->setScaleX(g_fScaleWH);
    
    auto imgPhotoPanel = (Layout*)(rootNode->getChildByName(StringUtils::format("panel_%d", 0)));
    //imgPhotoPanel->addTouchEventListener(CC_CALLBACK_2(RegisterLayer::onCheckBtnClicked, this, i));
    imgPhotoPanel->setScaleX(g_fScaleWH);
    imgPhotoPanel->setLocalZOrder(10);
    
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
    
    img_photo = ImageView::create(StringUtils::format("avatar_%d.jpg", 0), TextureResType::PLIST);
    img_photo->setScale(scaleX, scaleY);
    img_photo->setPosition(imgPhotoPanel->getPosition());
    clippingNode->addChild(img_photo);
    
    auto imgBorder = ImageView::create("border.png", TextureResType::PLIST);
    imgBorder->setScale(scaleX, scaleY);
    imgBorder->setPosition(imgPhotoPanel->getPosition());
    rootNode->addChild(imgBorder);
    imgBorder->setLocalZOrder(12);
    
    //main buttons
    for (int i = 0; i < 5; i++) {
        
        std::string strName = StringUtils::format("btn_%d", i);
        auto btnMain = static_cast<Button*>(rootNode->getChildByName(strName));
        btnMain->addTouchEventListener(CC_CALLBACK_2(PublicProfileLayer::onMainBtnClicked, this, i));
        btnMain->setScaleX(g_fScaleWH);
        
        if (i > 0) {
            
            btn_action[i-1] = btnMain;
        }
        
        if (i == 2) {
            
            lb_bantitle = (Text*)btnMain->getChildByName("text");
        }
        
        if (i == 4) {
            
            lb_title[4] = (Text*)btnMain->getChildByName("text");
        }
    }
    
    img_emoticonPanel = (ImageView*)rootNode->getChildByName("panel_1");
    lb_title[5] = (Text*)img_emoticonPanel->getChildByName("comment");
    lb_title[6] = (Text*)lb_title[5]->getChildByName("text");
    
    //emoticon buttons
    for (int i = 0; i < 12; i++) {
        
        std::string strName = StringUtils::format("btn_gift_%d", i);
        auto btnMain = static_cast<Button*>(img_emoticonPanel->getChildByName(strName));
        btnMain->addTouchEventListener(CC_CALLBACK_2(PublicProfileLayer::onEmoticonBtnClicked, this, i));
        btnMain->setScaleX(g_fScaleWH);
        
        img_emoticons[i] = (ImageView*)btnMain->getChildByName("mark");
    }
    
    lb_name = (Text*)(rootNode->getChildByName("name"));
    lb_level = (Text*)(rootNode->getChildByName("level"));
    lb_point = (Text*)(rootNode->getChildByName("lb_point"));
    
    lb_toprank = (Text*)(rootNode->getChildByName("lb_rank"));
    lb_maxearning = (Text*)(rootNode->getChildByName("lb_maxearning"));
    lb_tourplay = (Text*)(rootNode->getChildByName("lb_tourplay"));
    lb_winpro = (Text*)(rootNode->getChildByName("lb_winratio"));
    
    lb_text = (Text*)(rootNode->getChildByName("lb_text"));
    lb_title[7] = lb_text;
    
    return true;
}

void PublicProfileLayer::updateString2Lang()
{
    for (int i = 0; i < 8; i++) {
        
        lb_title[i]->setString(g_strPublicProfile[i][g_nLangType]);
    }
}

void PublicProfileLayer::getUserRank()
{
    std::string strURL = StringUtils::format("http://%s:8080/sys/money/mobile/getUserRank.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strURL.c_str());
    request->setResponseCallback(CC_CALLBACK_2(PublicProfileLayer::onGetUserRankDone, this));
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s",
                                              user_info.user_id.c_str()
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("get Rank");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void PublicProfileLayer::onGetUserRankDone(HttpClient* sender, HttpResponse* response)
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
    
    std::string user_id = cJSON_GetObjectItem(json, "user_id")->valuestring;
    if (user_info.user_id.compare(user_id) != 0) {
        
        return;
    }
    
    user_info.rank = cJSON_GetObjectItem(json, "rank")->valueint;
    user_info.rank_point = cJSON_GetObjectItem(json, "point")->valuedouble;
    
    cJSON_Delete(json);
    
    if(user_info.rank == 0) {
        
        lb_toprank->setString("--");
    }
    else {
        
        lb_toprank->setString(StringUtils::format("#%d", user_info.rank));
    }
}

void PublicProfileLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        if(btn_id == 0) { //close
            
            hideLayer();
        }
        else if(btn_id == 1){ //add friend

            WebService::getInstance()->sendFriendRequest2User(user_info.user_id);
            
            btn_action[0]->setVisible(false);
            lb_text->setVisible(true);
        }
        else if(btn_id == 2){ //ban
            
            g_bChatBan[showing_pos] = !g_bChatBan[showing_pos];
            if (g_bChatBan[showing_pos] == false) {
                
                lb_bantitle->setString("BAN");
            }
            else {
                
                lb_bantitle->setString("UNBAN");
            }
        }
        else if(btn_id == 3){ //send gift
            
            send_type = 0;
            img_emoticonPanel->setVisible(true);
            // hideLayer();
        }
        else if(btn_id == 4){ //send to all
            
            send_type = 1;
            img_emoticonPanel->setVisible(true);
            // hideLayer();
        }
    }
}

void PublicProfileLayer::onEmoticonBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        int cur_avatar = getSelectAvatar();
        int index = cur_avatar*12 + btn_id;
        if (send_type == 0) { //for one player
            
            //pDelegate->onGiftSelectDone(showing_pos, btn_id);
            pDelegate->onEmoticonSelectDone(showing_pos, index);
        }
        else { //for table
            
            //pDelegate->onGiftSelectDone(-1, btn_id);
            pDelegate->onEmoticonSelectDone(-1, index);
        }
        
        img_emoticonPanel->setVisible(false);
        hideLayer();
    }
}

void PublicProfileLayer::updateEmoticons()
{
    int avatar_index = getSelectAvatar();
    for (int i = 0; i < 12; i++) {
        
        img_emoticons[i]->loadTexture(StringUtils::format("emoticon_%d_%d.png", avatar_index, i), TextureResType::PLIST);
    }
}

void PublicProfileLayer::showLayer(USER_INFO info, int pos)
{
    user_info = info;
    //update info
    WebService::getInstance()->setProfileImage(info.photo_id, img_photo);
    
    img_emoticonPanel->setVisible(false);
    
    lb_name->setString(info.user_name);
    lb_level->setString(StringUtils::format("%d", info.level+1));
    lb_point->setString(getNormalText2Number(info.points));
    
    lb_maxearning->setString(getNormalText2Number(info.max_earning));
    lb_tourplay->setString(getNormalText2Number(info.total_tourplay));
    
    if (info.rank == 0) {
        
        lb_toprank->setString("--");
    }
    else {
    
        lb_toprank->setString(StringUtils::format("#%d", info.rank));
    }
    getUserRank();
    
    lb_text->setVisible(false);
    
    float percent = 0;
    if(info.total_play > 0) {
        
        percent = info.win_play/(float)info.total_play*100;
    }
    lb_winpro->setString(StringUtils::format("%.0f %%", percent));
    
    showing_pos = pos;
    
    if (showing_pos == INVALID) {
        
        btn_action[0]->setVisible(false);
        btn_action[1]->setVisible(false);
        btn_action[2]->setVisible(false);
        btn_action[3]->setVisible(false);
    }
    else if(showing_pos == MAIN_USER) {
        
        btn_action[0]->setVisible(false);
        btn_action[1]->setVisible(false);
        btn_action[2]->setVisible(true);
        btn_action[3]->setVisible(true);
    }
    else {
        
        btn_action[0]->setVisible(1-info.friend_flag);
        btn_action[1]->setVisible(true);
        btn_action[2]->setVisible(true);
        btn_action[3]->setVisible(true);
        
        if (g_bChatBan[pos] == false) {
            
            lb_bantitle->setString("BAN");
        }
        else {
            
            lb_bantitle->setString("UNBAN");
        }
    }
    
    updateEmoticons();
    
    updateString2Lang();
    pCurLayer->setVisible(true);
}

void PublicProfileLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}
