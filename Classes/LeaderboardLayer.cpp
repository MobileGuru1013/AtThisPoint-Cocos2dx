//
//  LeaderboardLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 5/1/16.
//
//

#include "LeaderboardLayer.h"

#include "WebService.h"
#include "cJSON.h"
#include "AppService.h"

//define string
std::string g_strLeaderboard[4][2] = {{"LEADERBOARD", "TABLA DE LIDERES"},
                                      {"EARNED POINTS", "PUNTOS GANADOS"},
                                      {"FRIENDS", "AMIGOS"},
                                      {"GLOBAL", "GLOBAL"}
                                     };

Layer* LeaderboardLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool LeaderboardLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("LeaderboardLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    auto panel = (Layout*)rootNode->getChildByName("main");
    panel->setScaleX(g_fScaleWH);
    
    lb_title[0] = (Text*)panel->getChildByName("title");
    lb_title[1] = (Text*)panel->getChildByName("comment");
    
    auto btnMain = static_cast<Button*>(panel->getChildByName("btn_0"));
    //btnMain->setScaleX(g_fScaleWH);
    btnMain->addTouchEventListener(CC_CALLBACK_2(LeaderboardLayer::onMainBtnClicked, this, 0));
    
    auto tabPanel = (Layout*)(panel->getChildByName(StringUtils::format("panel_%d", 0)));
    //tabPanel->setScaleX(g_fScaleWH);
    
    //tab buttons
    for (int i = 0; i < 2; i++) {
        
        std::string strName = StringUtils::format("btn_%d", i);
        btn_tab[i] = static_cast<Button*>(tabPanel->getChildByName(strName));
        btn_tab[i]->addTouchEventListener(CC_CALLBACK_2(LeaderboardLayer::onMainBtnClicked, this, i+1));
        
        lb_title[i+2] = (Text*)btn_tab[i]->getChildByName("text");
    }
    
    for (int i = 0; i < 2; i++) {
        
        panel_main[i] = (Layout*)panel->getChildByName(StringUtils::format("panel_%d", i+1));
        lv_list[i] = (ListView*)panel_main[i]->getChildByName("lv_list");
        lv_list[i]->setTouchEnabled(true);
        lv_list[i]->setClippingEnabled(true);
        lv_list[i]->setClippingType(Layout::ClippingType::SCISSOR);
        lv_list[i]->setVisible(false);
    }
    
    updateString2Lang();
    
    return true;
}

void LeaderboardLayer::updateString2Lang()
{
    for (int i = 0; i < 4; i++) {
        
        lb_title[i]->setString(g_strLeaderboard[i][g_nLangType]);
    }
}

void LeaderboardLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
                
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        
        if(btn_id == 0) { //close
            
            hideLayer();
        }
        else { //tab buttons
            
            selectTab(btn_id-1);
        }
    }
}

void LeaderboardLayer::selectTab(int index)
{
    select_index = index;
    
    for (int i = 0; i < 2; i++) {
        
        if (i == index) {
         
            btn_tab[i]->setBright(true);
            
            panel_main[i]->setVisible(true);
        }
        else {
            
            btn_tab[i]->setBright(false);
            
            panel_main[i]->setVisible(false);
        }
    }
    
    if(index == 0) {
        
        if (g_vFriendsRanking.size() > 0) {
            
            showFriendLeaderboard();
        }
        else {
            
            AppService::showCircleProgressbar();
            getFriendRankingList();
        }
    }
    else {
        
        if (g_vGlobalRanking.size() > 0) {
         
            showGlobalLeaderboard();
        }
        else {
            
            AppService::showCircleProgressbar();
            getGlobalRankingList();
        }
    }
}

void LeaderboardLayer::getGlobalRankingList()
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/money/mobile/rankingGlobal.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(CC_CALLBACK_2(LeaderboardLayer::onGlobalRankingCompleted, this));
    
    //    // write the post data
//    std::string strData = StringUtils::format("USER_ID=%s",
//                                              g_mainUserInfo.user_id.c_str()
//                                              );
//    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("get Global Ranking");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void LeaderboardLayer::getFriendRankingList()
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/money/mobile/rankingFriends.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(CC_CALLBACK_2(LeaderboardLayer::onFriendRankingCompleted, this));
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s",
                                              g_mainUserInfo.user_id.c_str()
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("get Friend Ranking");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void LeaderboardLayer::onGlobalRankingCompleted(HttpClient* sender, HttpResponse* response)
{
    AppService::hideCircleProgressbar();
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
    
    g_vGlobalRanking.clear();
    
    cJSON* datas = cJSON_GetObjectItem(json, "ranks");
    
    int count = cJSON_GetArraySize(datas);
    for (int i = 0; i < count; i++) {
        
        RANK_INFO info;
        cJSON* data = cJSON_GetArrayItem(datas, i);
        
        //user id
        info.user_id = cJSON_GetObjectItem(data, "userid")->valuestring;
        //name
        info.user_name = cJSON_GetObjectItem(data, "name")->valuestring;
        //photo
        info.photo_id = cJSON_GetObjectItem(data, "photo_id")->valuestring;
        //diamonds
        info.level = cJSON_GetObjectItem(data, "level")->valueint;
        //bonus
        info.total_earning = cJSON_GetObjectItem(data, "total_earning")->valuedouble;
        
        g_vGlobalRanking.push_back(info);
    }
    
    cJSON_Delete(json);
    
    if (count > 0) {
        
        showGlobalLeaderboard();
    }
}

void LeaderboardLayer::onFriendRankingCompleted(HttpClient* sender, HttpResponse* response)
{
    AppService::hideCircleProgressbar();
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
    
    g_vFriendsRanking.clear();
    
    cJSON* datas = cJSON_GetObjectItem(json, "ranks");
    
    int count = cJSON_GetArraySize(datas);
    for (int i = 0; i < count; i++) {
        
        RANK_INFO info;
        cJSON* data = cJSON_GetArrayItem(datas, i);
        
        //user id
        info.user_id = cJSON_GetObjectItem(data, "userid")->valuestring;
        //name
        info.user_name = cJSON_GetObjectItem(data, "name")->valuestring;
        //photo
        info.photo_id = cJSON_GetObjectItem(data, "photo_id")->valuestring;
        //diamonds
        info.level = cJSON_GetObjectItem(data, "level")->valueint;
        //bonus
        info.total_earning = cJSON_GetObjectItem(data, "total_earning")->valuedouble;
        
        g_vFriendsRanking.push_back(info);
    }
    
    cJSON_Delete(json);
    
    if (count > 0) {
        
        showFriendLeaderboard();
    }
}

void LeaderboardLayer::showFriendLeaderboard()
{
    ///////////////////////////    List view module begin    //////////////////////////////////
    lv_list[0]->removeAllChildrenWithCleanup(true);
    
    int cntTotalItems = g_vFriendsRanking.size(); //total item number
    for (int i = 0; i < cntTotalItems; i++) {
        
        auto itemNode = CSLoader::createNode("LeaderboardCellNode.csb");
        auto panelMain = (Layout*)itemNode->getChildByName("panel");
        panelMain->setTag(i);
        
        //photo
        auto panelPhoto = (ImageView*)panelMain->getChildByName("panel_0");
        //panelPhoto->setScaleX(g_fScaleWH);
        
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
        clippingNode->setLocalZOrder(1);
        
        ImageView *img_photo = ImageView::create("img_mask.png", TextureResType::PLIST);
        img_photo->ignoreContentAdaptWithSize(false);
        img_photo->setScale(scaleX, scaleY);
        img_photo->setPosition(panelPhoto->getPosition());
        clippingNode->addChild(img_photo);
        
        auto imgBorder = ImageView::create("border.png", TextureResType::PLIST);
        imgBorder->setScale(scaleX, scaleY);
        imgBorder->setPosition(panelPhoto->getPosition());
        panelMain->addChild(imgBorder);
        
        //level bg
        auto img_levelbg = (ImageView*)panelMain->getChildByName("level_bg");
        img_levelbg->setLocalZOrder(3);
        //img_levelbg->setScaleX(g_fScaleWH);
        
        //level
        auto lb_level = (Text*)panelMain->getChildByName("level");
        lb_level->setLocalZOrder(5);
        lb_level->setString(StringUtils::format("%d", g_vFriendsRanking[i].level+1));
        
        WebService::getInstance()->setProfileImage(g_vFriendsRanking[i].photo_id, img_photo);
        
        //name
        auto lb_name = (Text*)panelMain->getChildByName("name");
        lb_name->setString(g_vFriendsRanking[i].user_name);
        /*
        //mark
        auto img_mark0 = (ImageView*)panelMain->getChildByName("mark_0");
        //img_mark0->setScaleX(g_fScaleWH);
        
        auto img_mark1 = (ImageView*)panelMain->getChildByName("mark_1");
        //img_mark1->setScaleX(g_fScaleWH);
        
        //rank
        auto lb_rank = (Text*)panelMain->getChildByName("rank");
        
        if(i > 2) { //if rank is above 3
            
            img_mark1->setVisible(false);
            lb_rank->setString(StringUtils::format("%d", i+1));
        }
        else { //if rank is top 3
            
            img_mark0->setVisible(false);
            img_mark1->loadTexture(StringUtils::format("rank_mark%d.png", i), TextureResType::PLIST);
            lb_rank->setVisible("");
        }
        */
        auto img_mark0 = (ImageView*)panelMain->getChildByName("mark_0");
        //img_mark0->setScaleX(g_fScaleWH);
        img_mark0->setVisible(true);
        
        auto img_mark1 = (ImageView*)panelMain->getChildByName("mark_1");
        //img_mark1->setScaleX(g_fScaleWH);
        
        //rank
        auto lb_rank = (Text*)panelMain->getChildByName("rank");
        lb_rank->setString(StringUtils::format("%d", i+1));
        lb_rank->setVisible(true);
        
        img_mark1->setVisible(false);
        //points
        auto lb_point = (Text*)panelMain->getChildByName("point");
        lb_point->setString(StringUtils::format("%s", getNormalText2Number(g_vFriendsRanking[i].total_earning).c_str()));
        
        itemNode->removeChild(panelMain, true);
        
        lv_list[0]->pushBackCustomItem(panelMain);
    }
    lv_list[0]->setVisible(true);
    /////////////////////////////   List view module end   ////////////////////////////////////

}

void LeaderboardLayer::showGlobalLeaderboard()
{
    ///////////////////////////    List view module begin    //////////////////////////////////
    lv_list[1]->removeAllChildrenWithCleanup(true);
    
    int cntTotalItems = g_vGlobalRanking.size(); //total item number
    for (int i = 0; i < cntTotalItems; i++) {
        
        auto itemNode = CSLoader::createNode("LeaderboardCellNode.csb");
        auto panelMain = (Layout*)itemNode->getChildByName("panel");
        panelMain->setTag(i);
        
        //photo
        auto panelPhoto = (ImageView*)panelMain->getChildByName("panel_0");
        //panelPhoto->setScaleX(g_fScaleWH);
        
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
        clippingNode->setLocalZOrder(1);
        
        ImageView *img_photo = ImageView::create("img_mask.png", TextureResType::PLIST);
        img_photo->ignoreContentAdaptWithSize(false);
        img_photo->setScale(scaleX, scaleY);
        img_photo->setPosition(panelPhoto->getPosition());
        clippingNode->addChild(img_photo);
        
        auto imgBorder = ImageView::create("border.png", TextureResType::PLIST);
        imgBorder->setScale(scaleX, scaleY);
        imgBorder->setPosition(panelPhoto->getPosition());
        panelMain->addChild(imgBorder);
        
        //level bg
        auto img_levelbg = (ImageView*)panelMain->getChildByName("level_bg");
        img_levelbg->setLocalZOrder(3);
        //img_levelbg->setScaleX(g_fScaleWH);
        
        //level
        auto lb_level = (Text*)panelMain->getChildByName("level");
        lb_level->setLocalZOrder(5);
        lb_level->setString(StringUtils::format("%d", g_vGlobalRanking[i].level+1));
        
        WebService::getInstance()->setProfileImage(g_vGlobalRanking[i].photo_id, img_photo);
        
        //name
        auto lb_name = (Text*)panelMain->getChildByName("name");
        lb_name->setString(g_vGlobalRanking[i].user_name);
        /*
        //mark
        auto img_mark0 = (ImageView*)panelMain->getChildByName("mark_0");
        //img_mark0->setScaleX(g_fScaleWH);
        
        auto img_mark1 = (ImageView*)panelMain->getChildByName("mark_1");
        //img_mark1->setScaleX(g_fScaleWH);
        
        //rank
        auto lb_rank = (Text*)panelMain->getChildByName("rank");
        
        img_mark1->setVisible(false);
        lb_rank->setString(StringUtils::format("%d", i+1));
        */
        //mark
        auto img_mark0 = (ImageView*)panelMain->getChildByName("mark_0");
        //img_mark0->setScaleX(g_fScaleWH);
        // img_mark0->setVisible(false);
        
        auto img_mark1 = (ImageView*)panelMain->getChildByName("mark_1");
        //img_mark1->setScaleX(g_fScaleWH);
        
        //rank
        auto lb_rank = (Text*)panelMain->getChildByName("rank");
        auto lb_rank_1 = (Text*)panelMain->getChildByName("rank_1");
        
        if(i > 2) { //if rank is above 3
            
            
            img_mark0->setVisible(false);
            img_mark1->loadTexture("rank_mark3.png");
            img_mark1->setVisible(true);
            
            lb_rank->setString(StringUtils::format("%d", i+1));
            lb_rank->setVisible(false);
            lb_rank_1->setString(StringUtils::format("%d", i+1));
            lb_rank_1->setVisible(true);
            // lb_rank->setVisible(false);
        }
        else { //if rank is top 3
            
            img_mark0->setVisible(false);
            img_mark1->loadTexture(StringUtils::format("rank_mark%d.png", i), TextureResType::PLIST);
            
            lb_rank->setVisible(false);
            lb_rank_1->setVisible(false);
        }

        //points
        auto lb_point = (Text*)panelMain->getChildByName("point");
        lb_point->setString(StringUtils::format("%s", getNormalText2Number(g_vGlobalRanking[i].total_earning).c_str()));
        
        itemNode->removeChild(panelMain, true);
        
        lv_list[1]->pushBackCustomItem(panelMain);
    }
    lv_list[1]->setVisible(true);
    /////////////////////////////   List view module end   ////////////////////////////////////

}

void LeaderboardLayer::showLayer()
{
    g_vFriendsRanking.clear();
    g_vGlobalRanking.clear();
    
    selectTab(1);
    
    updateString2Lang();    
    pCurLayer->setVisible(true);
}

void LeaderboardLayer::hideLayer()
{
    pCurLayer->setVisible(false);
}
